/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/item/bomb.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"

void BombCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position, m_velocity);
}

void BombCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, b2Vec2 position, b2Vec2 velocity)
{
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnimManager *animManager = scene->GetAnimManager();

    uint32_t animId = AnimID_Make(AnimCategory::ITEM, AnimType::BOMB_NORMAL);

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

    b2BodyId bodyId = CreateBody(entity, scene, position, velocity);

    registry.emplace<NameComponent>(entity, "Bomb");
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId, true);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Damageable>(entity);
    registry.emplace<PlayerAffiliation>(entity);
    registry.emplace<BombTag>(entity);
}

b2BodyId BombCommand::CreateBody(entt::entity entity, Scene *scene, b2Vec2 position, b2Vec2 velocity)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.linearVelocity = velocity;
    bodyDef.fixedRotation = false;
    bodyDef.enableSleep = true;
    bodyDef.angularDamping = 0.05f;
    bodyDef.linearDamping = 0.05f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    // Shape
    b2Circle circle{};
    circle.center = b2Vec2{ 0.f, 0.0f };
    circle.radius = 0.40f;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.filter.categoryBits = CATEGORY_ITEM;
    shapeDef.filter.maskBits = CATEGORY_TERRAIN | CATEGORY_ATTACK;
    shapeDef.material.friction = 0.5f;
    shapeDef.material.restitution = 0.2f;

    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

    return bodyId;
}

void BombSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<BombTag, Damageable, PlayerAffiliation, const Transform>();
    for (auto [entity, damageable, affiliation, transform] : view.each())
    {
        if (damageable.cumulativeDamage.amount <= 0) continue;

        if ((m_registry.valid(damageable.lastDamagerEntity)) &&
            (m_registry.all_of<PlayerController>(damageable.lastDamagerEntity)))
        {
            affiliation = damageable.lastDamagerAffiliation;
            QueryFilter filter(CATEGORY_ALL_TEAMS);
            Damage damage;
            damage.amount = 20.f;
            damage.ejectionType = Damage::Type::CENTER;
            damage.attackCenter = transform.position + b2Vec2{ 0.f, -1.5f };
            damage.ejectionSpeed = 15.f;
            DamageUtils::AttackCircle(m_scene, entity, affiliation, damage, filter, transform.position, 1.5f);

            // Changer sinmplement l'animation pour ITEM::BOMB_DANGER
            // Puis modifier OnUpdate pour déclencher l'attaque à la fin de l'animation

            ecb.DestroyEntity(entity);
        }
        else
        {
            DamageUtils::ResetCumulativeDamage(damageable);
        }
    }
}

void BombSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    // Bonus : Utilisez cette fonction pour déclencher l'attaque et les particules
    // à la fin de l'anim ITEM::BOMB_DANGER
}

void BombSystem::EmitParticles(b2Vec2 position)
{
    // Bonus : Ajoutez un effet d'explosion + des particules
}
