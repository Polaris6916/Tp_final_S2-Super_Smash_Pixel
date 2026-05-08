/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct OneWayPass
{
    OneWayPass() : delay(0.f) {}

    float delay;
};

struct GroundContact
{
    GroundContact()
        : isGrounded(false)
        , wasGrounded(false)
        , inContact(false)
        , points1{ b2Vec2{ +0.3f, +0.3f }, b2Vec2{ -0.3f, +0.3f } }
        , points2{ b2Vec2{ +0.3f, -0.1f }, b2Vec2{ -0.3f, -0.1f } }
        , currVelocity(b2Vec2_zero)
        , prevVelocity(b2Vec2_zero)
        , groundVelocity(b2Vec2_zero)
    {}

    bool isGrounded;
    bool wasGrounded;
    bool inContact;
    b2Vec2 points1[2];
    b2Vec2 points2[2];
    b2Vec2 currVelocity;
    b2Vec2 prevVelocity;
    b2Vec2 groundVelocity;
};

class GroundSystem : public System
{
public:
    GroundSystem(Scene *scene) : System(scene, "Ground system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};

class OneWaySystem : public System
{
public:
    OneWaySystem(Scene *scene) : System(scene, "OneWay system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};

class OneWayCallback
{
public:
    explicit OneWayCallback(Scene *scene);

    static bool PreSolveStatic(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context);
    bool PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold) const;

protected:
    Scene *m_scene;
    entt::registry &m_registry;
};
