/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct ShieldTag {};

struct ShieldCommand : public EntityCommand
{
public:
    ShieldCommand(EntityCommandBuffer &ecb, uint32_t id, Scene *scene, VirtualEntityID parentId)
        : EntityCommand(ecb, id)
        , m_scene(scene)
        , m_parentId(parentId)
    {}

    virtual ~ShieldCommand() = default;

    virtual void Execute(entt::registry &registry, entt::entity entity) override;

    static void Create(
        entt::registry &registry, entt::entity entity,
        Scene *scene, entt::entity parent
    );

protected:
    Scene *m_scene;
    VirtualEntityID m_parentId;
};

class PlayerShieldSystem : public System
{
public:
    PlayerShieldSystem(Scene *scene) : System(scene, "Player shield system") {}

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};
