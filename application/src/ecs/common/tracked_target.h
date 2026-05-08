/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct TrackedTarget
{
    TrackedTarget();

    entt::entity entity;
    b2Vec2 position;
    bool isEnabled;
    bool isGrounded;
    float ejectionScore;
};

class TrackedTargetSystem : public System
{
public:
    TrackedTargetSystem(Scene *scene) : System(scene, "Tracked target system") {}

    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};

class TargetUtils
{
public:
    static void SearchTarget(
        TrackedTarget &target, entt::registry &registry,
        uint64_t teamMask, b2Vec2 position);

private:
    TargetUtils() = delete;
};
