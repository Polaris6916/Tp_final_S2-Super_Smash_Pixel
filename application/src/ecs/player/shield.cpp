/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/shield.h"
#include "ecs/player/player_components.h"

void ShieldCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_ecb.GetEntity(m_parentId));
}

void ShieldCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, entt::entity parent)
{
    SpriteGroup *spriteGroup = nullptr;

    uint32_t animId = AnimID_Make(AnimCategory::DEFAULT, AnimType::SHIELD);

    SpriteAnimManager *animManager = scene->GetAnimManager();
    spriteGroup = animManager->GetSpriteGroup(animId);
    AssertNew(spriteGroup);

    Sprite spriteComponent;
    spriteComponent.texture = spriteGroup->GetTexture();
    spriteComponent.srcRect = *spriteGroup->GetSourceRect(0);
    spriteComponent.dimensions = {
        spriteComponent.srcRect.w / 22.f,
        spriteComponent.srcRect.h / 22.f
    };

    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_FOREGROUND;
    renderLayer.orderInLayer = 0;

    registry.emplace<NameComponent>(entity, "Shield");
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<SpriteAnimState>(entity, animId);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Transform>(entity);
    registry.emplace<LocalTransform>(entity, parent, b2Vec2{ 0.f, 1.f });
    registry.emplace<ShieldTag>(entity);
}

void PlayerShieldSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<
        ShieldTag,
        const LocalTransform,
        RenderSortingLayer
    >();
    for (auto [entity, local, renderLayer] : view.each())
    {
        if (m_registry.valid(local.parent) == false) continue;
        if (m_registry.all_of<PlayerController>(local.parent) == false) continue;
        const PlayerController &controller = m_registry.get<const PlayerController>(local.parent);

        renderLayer.enabled = (controller.currState == PlayerState::DEFEND);
    }
}
