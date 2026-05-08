/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct StarFieldsTerrainCommand : public EntityCommand
{
public:
    StarFieldsTerrainCommand(EntityCommandBuffer &ecb, uint32_t id, Scene *scene, b2Vec2 position)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_position(position)
    {}

    virtual ~StarFieldsTerrainCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, b2Vec2 position
    );

protected:
    static b2BodyId CreateBody(
        entt::entity entity, Scene *scene, b2Vec2 position, float scale
    );
    static void CreateTiles(
        entt::registry &registry, entt::entity entity, Scene *scene, float scale
    );

    b2Vec2 m_position;
    Scene *m_scene;
};
