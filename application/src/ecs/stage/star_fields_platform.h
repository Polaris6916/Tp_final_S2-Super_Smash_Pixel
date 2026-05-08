/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct StarFieldsPlatformCommand : public EntityCommand
{
public:
    StarFieldsPlatformCommand(
        EntityCommandBuffer &ecb, uint32_t id, Scene *scene, b2Vec2 position, int type)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_position(position)
        , m_type(type)
    {}

    virtual ~StarFieldsPlatformCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, b2Vec2 position, int type
    );

protected:
    static b2BodyId CreateBody(
        entt::entity entity, Scene *scene, b2Vec2 position, float scale, int type
    );
    static void CreateTilesA(
        entt::registry &registry, entt::entity entity, Scene *scene, float scale
    );
    static void CreateTilesB(
        entt::registry &registry, entt::entity entity, Scene *scene, float scale
    );

    b2Vec2 m_position;
    Scene *m_scene;
    int m_type;
};

struct FloatingPlatform
{
    FloatingPlatform();

    float freqX;
    float freqY;
    float phaseX;
    float phaseY;
    float amplitudeX;
    float amplitudeY;
};

class FloatingPlatformSystem : public System
{
public:
    FloatingPlatformSystem(Scene *scene) : System(scene, "Floating platform system") {}

    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};

