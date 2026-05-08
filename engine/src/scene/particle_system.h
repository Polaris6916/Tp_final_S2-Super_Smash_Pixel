/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "rendering/anim.h"
#include "rendering/sprite_sheet.h"
#include "rendering/sprite_anim.h"
#include "ecs/basic_components.h"

class Scene;
struct Particle;

class ParticleSystem
{
public:
    ParticleSystem(Scene *scene) : m_scene(scene) {}
    virtual ~ParticleSystem();

    void EmitParticle(int layer, Particle particle);

    void Update();
    void renderParticleLayers(
        const Camera &camera, const Transform &cameraTransform,
        int lowerLayer, int upperLayer
    );

    int GetParticleCount() const;

protected:
    Scene *m_scene;
    std::map<int, std::vector<Particle>> particlePools;
};

struct Particle
{
public:
    Particle(uint32_t animId);
    ~Particle() = default;

    virtual bool Update(SpriteAnimManager *animManager, float dt);
    virtual void Render(
        SpriteAnimManager *animManager, SDL_Renderer *renderer, 
        const Camera &camera, const Transform &cameraTransform
    );

    void SetLifetimeFromAnim(SpriteAnimManager *animManager);
    void SetLifetime(float lifetime);
    void SetDelay(float delay);


    float lifetime;
    float remainingLifetime;
    float startSize;
    float opacity;
    float angle;
    float angularVelocity;
    float pixPerUnit;
    SpriteAnimState animState;

    Anchor anchor;

    SDL_FlipMode flip;
    SDL_BlendMode blendMode;

    b2Vec2 velocity;
    b2Vec2 position;
    b2Vec2 gravity;
    b2Vec2 damping;

    AnimState alphaState;
    AnimState scaleState;
    LerpValues<float> alphaValues;
    LerpValues<float> scaleValues;
};
