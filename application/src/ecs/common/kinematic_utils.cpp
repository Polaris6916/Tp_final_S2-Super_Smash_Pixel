/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/kinematic_utils.h"

KinematicTargetPosition::KinematicTargetPosition(b2Vec2 target, float smoothTime, float maxSpeed)
    : target(target)
    , smoothTime(smoothTime)
    , maxSpeed(maxSpeed)
{
}

KinematicTargetRotation::KinematicTargetRotation(float target, float smoothTime, float maxSpeed)
    : target(target)
    , smoothTime(smoothTime)
    , maxSpeed(maxSpeed)
{
}

void KinematicTargetSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    float delta = m_scene->GetDelta();

    auto viewPosition = m_registry.view<const KinematicTargetPosition, const Rigidbody>();
    for (auto [entity, targetPos, rigidbody] : viewPosition.each())
    {
        const b2BodyId bodyId = rigidbody.bodyId;
        if (b2Body_IsValid(bodyId) == false) continue;

        const b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
        const b2Vec2 position = b2Body_GetPosition(bodyId);

        const float targetDist = b2Length(targetPos.target - position);
        if (fabsf(targetDist) > 0.01f)
        {
            float speed = b2Length(velocity);

            (void)math::SmoothDamp(
                0.f, targetDist, speed,
                targetPos.smoothTime, targetPos.maxSpeed, delta
            );
            const b2Vec2 direction = (1.f / targetDist) * (targetPos.target - position);
            b2Body_SetLinearVelocity(bodyId, speed * direction);
        }
        else
        {
            b2Body_SetLinearVelocity(bodyId, b2Vec2_zero);
        }
    }

    auto viewRotation = m_registry.view<const KinematicTargetRotation, const Rigidbody>();
    for (auto [entity, targetRot, rigidbody] : viewRotation.each())
    {
        const b2BodyId bodyId = rigidbody.bodyId;
        if (b2Body_IsValid(bodyId) == false) continue;

        float angle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));
        float angularVelocity = b2Body_GetAngularVelocity(bodyId);
        float angleDiff = math::AngleDifference(targetRot.target, angle);

        if (fabsf(angleDiff) > 0.005f)
        {
            (void)math::SmoothDamp(
                angle, targetRot.target, angularVelocity,
                targetRot.smoothTime, targetRot.maxSpeed, m_scene->GetDelta()
            );
            b2Body_SetAngularVelocity(bodyId, angularVelocity);
        }
        else
        {
            b2Body_SetAngularVelocity(bodyId, 0.f);
        }
    }
}
