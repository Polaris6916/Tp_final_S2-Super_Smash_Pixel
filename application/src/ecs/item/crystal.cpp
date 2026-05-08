/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/item/crystal.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"
#include "ecs/player/player_utils.h"

void CrystalCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position, m_velocity);
}

void CrystalCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, b2Vec2 position, b2Vec2 velocity)
{
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnimManager *animManager = scene->GetAnimManager();

    uint32_t animId = AnimID_Make(AnimCategory::ITEM, AnimType::CRYSTAL);

    spriteGroup = animManager->GetSpriteGroup(animId);
    AssertNew(spriteGroup);

    Sprite spriteComponent;
    spriteComponent.texture = spriteGroup->GetTexture();
    spriteComponent.srcRect = *spriteGroup->GetSourceRect(0);
    spriteComponent.dimensions = {
        spriteComponent.srcRect.w / 32.f,
        spriteComponent.srcRect.h / 32.f
    };
    spriteComponent.offset = b2Vec2_zero;

    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_PLAYER;
    renderLayer.orderInLayer = 1;

    b2BodyId bodyId = CreateBody(entity, scene, position, velocity);

    registry.emplace<NameComponent>(entity, "Crystal");
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId, true);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Damageable>(entity);
    registry.emplace<PlayerAffiliation>(entity);
    registry.emplace<CrystalTag>(entity);
}

b2BodyId CrystalCommand::CreateBody(entt::entity entity, Scene *scene, b2Vec2 position, b2Vec2 velocity)
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

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    // Shape
    b2Circle circle{};
    circle.center = b2Vec2{ 0.f, 0.0f };
    circle.radius = 0.70f;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.filter.categoryBits = CATEGORY_ITEM;
    shapeDef.filter.maskBits = CATEGORY_TERRAIN | CATEGORY_ATTACK;
    shapeDef.material.friction = 0.5f;
    shapeDef.material.restitution = 0.2f;

    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

    return bodyId;
}

void CrystalSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<CrystalTag, Damageable, const Transform>();
    for (auto [entity, damageable, transform] : view.each())
    {
        if (damageable.cumulativeDamage.amount <= 0) continue;

        entt::entity damagerEntity = damageable.lastDamagerEntity;
        if (PlayerUtils::HealIfPlayer(m_registry, damagerEntity, 60.f))
        {
            ecb.DestroyEntity(entity);
            CrystalSystem::EmitParticles(transform.position);
        }
        else
        {
            DamageUtils::ResetCumulativeDamage(damageable);
        }
    }
}

void CrystalSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    // Bonus : Utilisez cette fonction pour déclencher l'attaque et les particules
    // à la fin de l'anim ITEM::BOMB_DANGER
}

void CrystalSystem::EmitParticles(b2Vec2 position)
{
    // Bonus : Ajoutez un effet d'explosion + des particules
}
