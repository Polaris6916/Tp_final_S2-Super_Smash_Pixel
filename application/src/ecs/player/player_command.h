/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct PlayerCommand : public EntityCommand
{
public:
    PlayerCommand(EntityCommandBuffer &ecb, uint32_t id, Scene *scene, int playerID, b2Vec2 position)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_playerID(playerID)
        , m_position(position)
    {}

    virtual ~PlayerCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, int playerID, b2Vec2 position
    );

protected:
    static b2BodyId CreateBody(entt::entity entity, Scene *scene, const PlayerConfig *config, b2Vec2 position);

    Scene *m_scene;
    int m_playerID;
    b2Vec2 m_position;
};
