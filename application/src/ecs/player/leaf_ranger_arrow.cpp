
#include "ecs/player/leaf_ranger_arrow.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"
#include "ecs/player/player_controller_system.h"



void ArrowCommand::Execute(entt::registry& registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_affiliation, m_controller, m_position, m_velocity, m_s, m_anim);
    //registry.emplace<ArrowComponent>(entity, ArrowComponent{ m_anim });
}

void ArrowCommand::Create(
    entt::registry& registry, entt::entity entity, Scene* scene,
    const PlayerAffiliation& affiliation,
    const PlayerController& controller,
    b2Vec2 position, b2Vec2 velocity, float s, AnimType anim)
{
    SpriteGroup* spriteGroup = nullptr;
    SpriteAnimManager* animManager = scene->GetAnimManager();

    uint32_t animId = AnimID_Make(AnimCategory::VFX, anim);

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
    if (s < 0 && anim != AnimType::ARROW_SMASH)
    {
        spriteComponent.flip = SDL_FLIP_HORIZONTAL;
    }
    else if (s > 0 && anim == AnimType::ARROW_SMASH)
    {
        spriteComponent.flip = SDL_FLIP_HORIZONTAL;
    }
        
    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_PLAYER;
    renderLayer.orderInLayer = 1;

    b2BodyId bodyId = CreateBody(entity, scene, affiliation, position, velocity,s, anim);

    registry.emplace<NameComponent>(entity, "Arrow");
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId, true);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Damageable>(entity);
    registry.emplace<PlayerAffiliation>(entity);
    registry.emplace<ArrowTag>(entity);
    registry.emplace<ArrowComponent>(entity, ArrowComponent{ anim , controller, s});
}

b2BodyId ArrowCommand::CreateBody(entt::entity entity, Scene* scene, 
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
    
    if (anim == AnimType::ARROW_3 || anim == AnimType::ARROW_2_DEBUT)
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
    else if (anim == AnimType::ARROW_2_FIN)
    {
        b2Circle circle;
        circle.center = b2Vec2(s * 0.f, -1.5f);
        circle.radius = 1.6f;

        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = CATEGORY_ATTACK;
        shapeDef.filter.maskBits = otherTeamMask;
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true;

        b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }
    else if (anim == AnimType::ARROW_SMASH)
    {
        b2Segment segment;
        segment.point1 = b2Vec2(5.f, 0.f);
        segment.point2 = b2Vec2(-6.f, 0.f);

        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = CATEGORY_ATTACK;
        shapeDef.filter.maskBits = otherTeamMask;
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true;

        b2ShapeId shapeId = b2CreateSegmentShape(bodyId, &shapeDef, &segment);
    }
    if (anim == AnimType::ARROW_AIR)
    {
        b2Segment segment;
        segment.point1 = b2Vec2(s * 0.4f, -0.4f);
        segment.point2 = b2Vec2(s * -0.1f, 0.5f);

        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = CATEGORY_ATTACK;
        shapeDef.filter.maskBits = otherTeamMask;
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true;

        b2ShapeId shapeId = b2CreateSegmentShape(bodyId, &shapeDef, &segment);
    }


    return bodyId;
}
void ArrowSystem::OnFixedUpdate(EntityCommandBuffer& ecb)
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
        if (!m_registry.all_of<ArrowComponent>(sensorEntity)) continue;
        auto& arrow = m_registry.get<ArrowComponent>(sensorEntity);

        //printf("Arrow anim type: %d\n", static_cast<int>(arrow.animType));

        QueryFilter filter(CATEGORY_ALL_TEAMS);
        Damage damage;
        if (arrow.animType == AnimType::ARROW_3)
        {
            damage.amount = 10.f;
            damage.ejectionType = Damage::Type::FIXED_DIRECTION;
            damage.attackCenter = position + b2Vec2{ 0.f, -1.5f };
            damage.ejectionSpeed = 2.f;

            const b2Vec2 vertices[3] = {
                position + b2Vec2(arrow.s * 0.5f, 0.2f),
                position + b2Vec2(arrow.s * 0.5f, -0.2f),
                position + b2Vec2(arrow.s * 1.0f, 0.0f),
            };
            DamageUtils::AttackPolygon(m_scene, sensorEntity, affiliation, damage, filter, vertices, 3);
            ecb.DestroyEntity(sensorEntity);
        }
        else if (arrow.animType == AnimType::ARROW_2_FIN)
        {
            damage.amount = 20.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.attackCenter = position;
            damage.ejectionSpeed = 0.f;
            const b2Vec2 center = position;
            DamageUtils::AttackBox(m_scene, sensorEntity, affiliation, damage, filter, center, 1.5f, 2.8f);
        }
        else if (arrow.animType == AnimType::ARROW_SMASH)
        {
            damage.amount = 10.f;
            damage.ejectionType = Damage::Type::FIXED_DIRECTION;
            damage.attackCenter = b2Vec2{ 0.f, 0.f };
            damage.ejectionSpeed = 10.f;

            const b2Vec2 vertices[4] = {
               position + b2Vec2(arrow.s * -5.5f, 0.f),
               position + b2Vec2(arrow.s * 5.f, -0.4f),
               position + b2Vec2(arrow.s * 5.8f, 0.f),
               position + b2Vec2(arrow.s * 5.f, 0.4f),
            };
            DamageUtils::AttackPolygon(m_scene, sensorEntity, affiliation, damage, filter, vertices, 4);
        }
        if (arrow.animType == AnimType::ARROW_AIR)
        {
            damage.amount = 10.f;
            damage.attackCenter = position;
            damage.ejectionType = Damage::Type::CENTER;
            damage.lockAttackTime = 5.5f * PLAYER_ATTACK_FRAME_TIME;

            const b2Vec2 center = position + b2Vec2( arrow.s * 0.4f, -0.4f);
            DamageUtils::AttackCircle(m_scene, sensorEntity, affiliation, damage, filter, center, 0.5f);
            ecb.DestroyEntity(sensorEntity);
        }
        
    }    
}

void ArrowSystem::OnUpdate(EntityCommandBuffer& ecb)
{
    auto viewAnim = m_registry.view<
        const ArrowTag,
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
                ecb.DestroyEntity(entity);
                //printf("ok 2\n");
            }
        }
    }
}
