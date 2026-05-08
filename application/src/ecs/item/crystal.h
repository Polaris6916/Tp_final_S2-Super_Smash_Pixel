/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct CrystalTag {};

struct CrystalCommand : public EntityCommand
{
public:
    CrystalCommand(EntityCommandBuffer &ecb, Scene *scene, b2Vec2 position, b2Vec2 velocity)
        : EntityCommand(ecb, ecb.CreateEntity())
        , m_scene(scene)
        , m_position(position)
        , m_velocity(velocity)
    {}

    virtual ~CrystalCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, b2Vec2 position, b2Vec2 velocity
    );

protected:
    static b2BodyId CreateBody(entt::entity entity, Scene *scene, b2Vec2 position, b2Vec2 velocity);

    Scene *m_scene;
    b2Vec2 m_position;
    b2Vec2 m_velocity;
};

class CrystalSystem : public System
{
public:
    CrystalSystem(Scene *scene) : System(scene, "Crystal system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

protected:
    void EmitParticles(b2Vec2 position);
};
