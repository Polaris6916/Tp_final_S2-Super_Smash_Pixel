/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ecs/common/damage_utils.h"

struct VisualID
{
    VisualID(bool alwaysVisible);
    bool alwaysVisible;
};

struct VisualIDCommand : public EntityCommand
{
public:
    VisualIDCommand(
        EntityCommandBuffer &ecb, uint32_t id,
        Scene *scene, uint32_t parentID, const PlayerAffiliation &playerID,
        b2Vec2 offset, bool alwaysVisible)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_parentID(parentID)
        , m_affiliation(playerID)
        , m_offset(offset)
        , m_alwaysVisible(alwaysVisible)
    {}

    virtual ~VisualIDCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, entt::entity parent, const PlayerAffiliation &affiliation,
        b2Vec2 offset, bool alwaysVisible = true);

protected:
    Scene *m_scene;
    uint32_t m_parentID;
    PlayerAffiliation m_affiliation;
    b2Vec2 m_offset;
    bool m_alwaysVisible;
};

class VisualIDSimulationSystem : public System
{
public:
    VisualIDSimulationSystem(Scene *scene) : System(scene, "Visual ID simulation system") {}

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};
