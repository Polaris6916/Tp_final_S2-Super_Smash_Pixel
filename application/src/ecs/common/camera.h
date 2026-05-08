/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct CameraFollow
{
    CameraFollow(b2Vec2 target);
    b2Vec2 target;
    b2Vec2 velocity;
};

struct CameraCommand : public EntityCommand
{
public:
    CameraCommand(EntityCommandBuffer &ecb, uint32_t id, Scene *scene, b2Vec2 position)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_position(position)
    {}

    virtual ~CameraCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, b2Vec2 position
    );

protected:
    b2Vec2 m_position;
    Scene *m_scene;
};

class CameraSystem : public System
{
public:
    CameraSystem(Scene *scene) : System(scene, "Camera system"), useDebugCamera(false) {}

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

    bool useDebugCamera;

protected:
    b2AABB ComputePlayersBox();
    void UpdateMainCamera(
        Camera &camera,
        CameraFollow &follow,
        Transform &transform
    );
    void UpdateDebugCamera(
        Camera &camera,
        Transform &transform
    );
};
