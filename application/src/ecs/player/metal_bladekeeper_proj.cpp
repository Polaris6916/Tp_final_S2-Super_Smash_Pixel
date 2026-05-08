
#include "ecs/player/metal_bladekeeper_proj.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"
#include "ecs/player/player_controller_system.h"



void KnifeCommand::Execute(entt::registry& registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_affiliation, m_controller, m_position, m_velocity, m_s, m_anim);
}

void KnifeCommand::Create(
    entt::registry& registry, entt::entity entity, Scene* scene,
    const PlayerAffiliation& affiliation,
    const PlayerController& controller,
    b2Vec2 position, b2Vec2 velocity, float s, AnimType anim)
{
    SpriteGroup* spriteGroup = nullptr;
    SpriteAnimManager* animManager = scene->GetAnimManager();

    uint32_t animId;
 
    animId = AnimID_Make(AnimCategory::VFX, anim);
    

    spriteGroup = animManager->GetSpriteGroup(animId);
    AssertNew(spriteGroup);

    Sprite spriteComponent;
    spriteComponent.texture = spriteGroup->GetTexture();
    spriteComponent.srcRect = *spriteGroup->GetSourceRect(0);
    spriteComponent.dimensions = {
        spriteComponent.srcRect.w / 22.f,
        spriteComponent.srcRect.h / 22.f
    };
    spriteComponent.offset = b2Vec2_zero;
    if (s < 0)
    {
        spriteComponent.flip = SDL_FLIP_HORIZONTAL;
    }

        
    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_PLAYER;
    renderLayer.orderInLayer = 1;

    b2BodyId bodyId = CreateBody(entity, scene, affiliation, position, velocity,s, anim);

    registry.emplace<NameComponent>(entity, "Knife");
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId, true);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Damageable>(entity);
    registry.emplace<PlayerAffiliation>(entity);
    registry.emplace<KnifeTag>(entity);
    registry.emplace<KnifeComponent>(entity, KnifeComponent{ anim , controller, s});
}

b2BodyId KnifeCommand::CreateBody(entt::entity entity, Scene* scene,
    const PlayerAffiliation& affiliation,
    b2Vec2 position, b2Vec2 velocity, float s, AnimType anim)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.linearVelocity = velocity;
    bodyDef.fixedRotation = true;
    bodyDef.enableSleep = true;
    bodyDef.angularDamping = 0.05f;
    bodyDef.linearDamping = 0.05f;
    bodyDef.gravityScale = 0.f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    // Shape
    
    if (anim == AnimType::KNIFE)
    {
        b2Segment segment;
        segment.point1 = b2Vec2(0.8f, 0.f);
        segment.point2 = b2Vec2(-0.8f, 0.f);

        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = CATEGORY_ATTACK;
        shapeDef.filter.maskBits = otherTeamMask;
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true;

        b2ShapeId shapeId = b2CreateSegmentShape(bodyId, &shapeDef, &segment);
    }
    else if (anim == AnimType::BLADE_1 || anim == AnimType::BLADE_2 || anim == AnimType::BLADE_3 || anim == AnimType::BLADE_4 || anim == AnimType::BLADE_5)
    {
        b2Circle circle;
        circle.center = b2Vec2(s * 0.f, 0.f);
        circle.radius = 2.6f;

        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = CATEGORY_ATTACK;
        shapeDef.filter.maskBits = otherTeamMask;
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true;

        b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }

    return bodyId;
}
void KnifeSystem::OnFixedUpdate(EntityCommandBuffer& ecb)
{
    b2WorldId worldId = m_scene->GetWorld();
    b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);
    for (int i = 0; i < sensorEvents.beginCount; i++)
    {
        // Boucle pour parcourir tous les events du dernier pas du moteur physique

        b2SensorBeginTouchEvent* beginEvent = sensorEvents.beginEvents + i;
        entt::entity sensorEntity = RigidbodyUtils::GetEntity(beginEvent->sensorShapeId);
        entt::entity visitorEntity = RigidbodyUtils::GetEntity(beginEvent->visitorShapeId);
        if (m_registry.valid(sensorEntity) == false) continue;
        if (m_registry.valid(visitorEntity) == false) continue;

        // Vérifiez bien les composants que vous souhaitez
        if (m_registry.all_of<PlayerAffiliation>(sensorEntity) == false) continue;
        if (m_registry.all_of<Damageable>(visitorEntity) == false) continue;
        auto& affiliation = m_registry.get<PlayerAffiliation>(sensorEntity);
        auto& damageable = m_registry.get<Damageable>(visitorEntity);

        // Exemple pour obtenir la position et la vitesse d'un des deux corps
        b2BodyId sensorBodyId = b2Shape_GetBody(beginEvent->sensorShapeId);
        b2Vec2 position = b2Body_GetPosition(sensorBodyId);
        b2Vec2 velocity = b2Body_GetLinearVelocity(sensorBodyId);

        // récup du m_anim
        if (!m_registry.all_of<KnifeComponent>(sensorEntity)) continue;
        auto& Knife = m_registry.get<KnifeComponent>(sensorEntity);

        QueryFilter filter(CATEGORY_ALL_TEAMS);
        Damage damage;
        if (Knife.animType == AnimType::KNIFE)
        {
            damage.amount = 2.f;
            damage.ejectionType = Damage::Type::FIXED_DIRECTION;
            damage.attackCenter = position + b2Vec2{ 0.f, -1.5f };
            damage.ejectionSpeed = 2.f;

            const b2Vec2 vertices[3] = {
                position + b2Vec2(Knife.s * 0.5f, 0.2f),
                position + b2Vec2(Knife.s * 0.5f, -0.2f),
                position + b2Vec2(Knife.s * 1.0f, 0.0f),
            };
            DamageUtils::AttackPolygon(m_scene, sensorEntity, affiliation, damage, filter, vertices, 3);
            ecb.DestroyEntity(sensorEntity);
        }
        else if (Knife.animType == AnimType::BLADE_1 || Knife.animType == AnimType::BLADE_2 || Knife.animType == AnimType::BLADE_3 || Knife.animType == AnimType::BLADE_4 || Knife.animType == AnimType::BLADE_5)
        {
            damage.amount = 10.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.attackCenter = position;
            damage.ejectionSpeed = 0.f;
            const b2Vec2 center = position;
            DamageUtils::AttackBox(m_scene, sensorEntity, affiliation, damage, filter, center, 2.6f, 1.f);
        }
    }    
}

void KnifeSystem::OnUpdate(EntityCommandBuffer& ecb)
{
    auto viewAnim = m_registry.view<
        const KnifeTag,
        const PlayerAffiliation,
        const Transform,
        SpriteAnimState
    >();
    //printf("ok 1\n");
    for (auto [entity, affiliation, transform, spriteAnim] : viewAnim.each())
    {
        assert(spriteAnim.saveEvents);
        for (int i = 0; i < spriteAnim.eventCount; i++)
        {
            SpriteAnimEvent& animEvent = spriteAnim.events[i];

            if (animEvent.type == SpriteAnimEvent::Type::CYCLE_END)
            {
                //OnAnimCycleEnd(ecb, entity, animEvent, spriteAnim, affiliation, input, animInfo);
                ecb.DestroyEntity(entity);
            }
        }
    }
}


