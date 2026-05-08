/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct PotionTag {};

struct PotionCommand : public EntityCommand
{
public:
    PotionCommand(EntityCommandBuffer &ecb, Scene *scene, b2Vec2 position)
        : EntityCommand(ecb, ecb.CreateEntity())
        , m_scene(scene)
        , m_position(position)
    {}

    virtual ~PotionCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, b2Vec2 position
    );

protected:
    static b2BodyId CreateBody(entt::entity entity, Scene *scene, b2Vec2 position);

    b2Vec2 m_position;
    Scene *m_scene;
};

class PotionSystem : public System
{
public:
    PotionSystem(Scene *scene) : System(scene, "Potion system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;

protected:
    void EmitParticles(b2Vec2 position);
};
