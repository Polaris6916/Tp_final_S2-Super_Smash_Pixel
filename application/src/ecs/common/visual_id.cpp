/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/visual_id.h"

VisualID::VisualID(bool alwaysVisibleIn)
    : alwaysVisible(alwaysVisibleIn)
{}

void VisualIDCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(
        registry, entity, m_scene, m_ecb.GetEntity(m_parentID),
        m_affiliation, m_offset, m_alwaysVisible
    );
}

void VisualIDCommand::Create(
    entt::registry &registry, entt::entity entity,
    Scene *scene, entt::entity parent, const PlayerAffiliation &affiliation,
    b2Vec2 offset, bool alwaysVisible)
{
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI_STAGE);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = nullptr;

    PlayerConfig *config = g_gameCommon.GetPlayerConfig(affiliation.playerID);
    if (config == nullptr) return;

    // Bonus : Vous pouvez changer le groupe en fonction de la config pour pouvoir afficher CPU

    spriteGroup = spriteSheet->GetGroup("P" + std::to_string(affiliation.playerID + 1));
    AssertNew(spriteGroup);

    // Bonus : Vous pouvez changer le rectangle source pour avoir la couleur de l'équipe

    Sprite spriteComponent;
    spriteComponent.texture = spriteGroup->GetTexture();
    spriteComponent.srcRect = *spriteGroup->GetSourceRect(0);
    spriteComponent.dimensions = {
        spriteComponent.srcRect.w / 66.f,
        spriteComponent.srcRect.h / 66.f
    };
    spriteComponent.anchor = Anchor::CENTER;

    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_FOREGROUND;
    renderLayer.orderInLayer = 0;

    registry.emplace<NameComponent>(entity, "Visual ID");
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);
    registry.emplace<Transform>(entity);
    registry.emplace<LocalTransform>(entity, parent, offset);
    registry.emplace<VisualID>(entity, alwaysVisible);
}

void VisualIDSimulationSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<const VisualID, const LocalTransform>();
    for (auto [entity, visualID, local] : view.each())
    {
        if (m_registry.valid(local.parent)) continue;

        ecb.DestroyEntity(entity);
    }
}
