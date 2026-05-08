/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/terrain_utils.h"

void GroundSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<
        Rigidbody, GroundContact
    >();
    for (auto [entity, rigidbody, ground] : view.each())
    {
        b2BodyId bodyId = rigidbody.bodyId;
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);

        ground.inContact = false;

        b2ContactData contactData[8] = { 0 };
        int contactCount = b2Body_GetContactData(bodyId, contactData, 8);
        for (int i = 0; i < contactCount; i++)
        {
            const b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
            const b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);

            if (B2_ID_EQUALS(bodyId, bodyIdA))
            {
                const b2Filter filterB = b2Shape_GetFilter(contactData[i].shapeIdB);
                if ((filterB.categoryBits & CATEGORY_TERRAIN) != 0) ground.inContact = true;
            }
            else
            {
                const b2Filter filterA = b2Shape_GetFilter(contactData[i].shapeIdA);
                if ((filterA.categoryBits & CATEGORY_TERRAIN) != 0) ground.inContact = true;
            }
        }

        ground.prevVelocity = ground.currVelocity;
        ground.currVelocity = velocity;
        ground.wasGrounded = ground.isGrounded;
        ground.isGrounded = false;

        if (velocity.y < 8.f)
        {
            for (int i = 0; i < 2; i++)
            {
                b2Vec2 point1 = position + ground.points1[i];
                b2Vec2 point2 = position + ground.points2[i];
                QueryFilter filter(CATEGORY_TERRAIN);
                RayHit rayHit = m_scene->RayCastFirstGizmos(point1, point2, filter);
                ground.isGrounded = b2Shape_IsValid(rayHit.shapeId);
                if (ground.isGrounded)
                {
                    ground.groundVelocity = b2Body_GetLinearVelocity(b2Shape_GetBody(rayHit.shapeId));
                    break;
                }
            }
        }
        if (ground.wasGrounded == false && ground.inContact == false)
        {
            ground.isGrounded = false;
        }
    }
}

void OneWaySystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    float delta = m_scene->GetDelta();

    auto view = m_registry.view<OneWayPass>();
    for (auto [entity, oneWayPass] : view.each())
    {
        oneWayPass.delay -= delta;
    }
}

OneWayCallback::OneWayCallback(Scene *scene)
    : m_scene(scene)
    , m_registry(scene->GetRegistry())
{
    b2World_SetPreSolveCallback(scene->GetWorld(), PreSolveStatic, this);
}

bool OneWayCallback::PreSolveStatic(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context)
{
    OneWayCallback *oneWay = static_cast<OneWayCallback *>(context);
    return oneWay->PreSolve(shapeIdA, shapeIdB, manifold);
}

bool OneWayCallback::PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold) const
{
    assert(b2Shape_IsValid(shapeIdA));
    assert(b2Shape_IsValid(shapeIdB));
    const b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    const b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);
    assert(b2Body_IsValid(bodyIdA));
    assert(b2Body_IsValid(bodyIdB));

    float sign = 0.0f;
    b2Vec2 velocity = b2Vec2_zero;
    entt::entity entity = entt::null;

    const b2Filter filterA = b2Shape_GetFilter(shapeIdA);
    if ((filterA.categoryBits & CATEGORY_TERRAIN) != 0)
    {
        const BodyUserData *userData = static_cast<BodyUserData *>(b2Body_GetUserData(bodyIdB));
        if (userData == nullptr) return true;
        velocity = b2Body_GetLinearVelocity(bodyIdB);
        entity = userData->entity;
        sign = +1.f;
    }

    const b2Filter filterB = b2Shape_GetFilter(shapeIdB);
    if ((filterB.categoryBits & CATEGORY_TERRAIN) != 0)
    {
        const BodyUserData *userData = static_cast<BodyUserData *>(b2Body_GetUserData(bodyIdA));
        if (userData == nullptr) return true;
        velocity = b2Body_GetLinearVelocity(bodyIdA);
        entity = userData->entity;
        sign = -1.f;
    }

    if (m_registry.all_of<OneWayPass>(entity))
    {
        const auto &oneWayPass = m_registry.get<OneWayPass>(entity);
        if (oneWayPass.delay >= 0.f)
        {
            // disable contact
            return false;
        }
    }

    float separation = 0.0f;
    for (int i = 0; i < manifold->pointCount; ++i)
    {
        float s = manifold->points[i].separation;
        separation = separation < s ? separation : s;
    }

    b2Vec2 normal = manifold->normal;
    if (sign * normal.y > 0.95f)
    {
        if (separation < -0.5f || velocity.y > 0.1f)
        {
            // disable contact
            return false;
        }
        return true;
    }

    // disable contact
    return false;
}
