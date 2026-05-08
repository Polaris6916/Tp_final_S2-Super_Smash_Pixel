/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct ReferencePosition
{
    b2Vec2 position;
};

struct KinematicTargetPosition
{
    KinematicTargetPosition(b2Vec2 target = b2Vec2_zero, float smoothTime = 1.f, float maxSpeed = 100.f);

    b2Vec2 target;
    float smoothTime;
    float maxSpeed;
};

struct KinematicTargetRotation
{
    KinematicTargetRotation(float target = 0.f, float smoothTime = 1.f, float maxSpeed = 100.f);

    float target;
    float smoothTime;
    float maxSpeed;
};

class KinematicTargetSystem : public System
{
public:
    KinematicTargetSystem(Scene *scene) : System(scene, "Kinematic target system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};
