/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/item/potion.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"
#include "ecs/player/player_utils.h"

void PotionCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position);
}

void PotionCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, b2Vec2 position)
{
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnimManager *animManager = scene->GetAnimManager();

    uint32_t animId = AnimID_Make(AnimCategory::ITEM, AnimType::POTION);
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

    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_PLAYER;
    renderLayer.orderInLayer = 1;

    b2BodyId bodyId = CreateBody(entity, scene, position);

    registry.emplace<NameComponent>(entity, "Potion");
    registry.emplace<PotionTag>(entity);
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Damageable>(entity);
}

b2BodyId PotionCommand::CreateBody(entt::entity entity, Scene *scene, b2Vec2 position)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.fixedRotation = false;
    bodyDef.enableSleep = true;
    bodyDef.angularDamping = 0.05f;
    bodyDef.linearDamping = 0.05f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    // Box shape
    b2Polygon box = b2MakeBox(0.30f, 0.35f);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.1f;
    shapeDef.filter.categoryBits = CATEGORY_ITEM;
    shapeDef.filter.maskBits = CATEGORY_TERRAIN | CATEGORY_ATTACK;
    shapeDef.material.friction = 0.2f;
    shapeDef.material.restitution = 0.f;
    shapeDef.enableContactEvents = true;

    b2ShapeId polygonShape = b2CreatePolygonShape(bodyId, &shapeDef, &box);

    return bodyId;
}

void PotionSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<PotionTag, Damageable, const Transform>();
    for (auto [entity, damageable, transform] : view.each())
    {
        if (damageable.cumulativeDamage.amount <= 0) continue;

        entt::entity damagerEntity = damageable.lastDamagerEntity;
        if (PlayerUtils::HealIfPlayer(m_registry, damagerEntity, 30.f))
        {
            ecb.DestroyEntity(entity);
            PotionSystem::EmitParticles(transform.position);
        }
        else
        {
            DamageUtils::ResetCumulativeDamage(damageable);
        }
    }
}

void PotionSystem::EmitParticles(b2Vec2 position)
{
    // Bonus : Ajoutez un effet lors de l'utilisation de la potion

}
