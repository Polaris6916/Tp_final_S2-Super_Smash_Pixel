/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "scene/particle_system.h"
#include "scene/scene.h"
#include "utils/utils.h"

int ParticleSystem::GetParticleCount() const
{
    return 0;
}

Particle::Particle(uint32_t animId)
    : animState(animId)
    , lifetime(1.f)
    , remainingLifetime(lifetime)
    , startSize(1.f)
    , opacity(1.f)
    , angle(0.f)
    , angularVelocity(0.f)
    , pixPerUnit(24.f)
    , anchor(Anchor::CENTER)
    , flip(SDL_FLIP_NONE)
    , blendMode(SDL_BLENDMODE_BLEND)
    , velocity(b2Vec2_zero)
    , position(b2Vec2_zero)
    , gravity(b2Vec2_zero)
    , damping(b2Vec2_zero)
    , scaleState(1.f, 1, false)
    , alphaState(1.f, 1, false)
    , scaleValues{ 1.f, 1.f, EasingFct_Linear }
    , alphaValues{ 1.f, 1.f, EasingFct_Linear }
{
}

bool Particle::Update(SpriteAnimManager *animManager, float dt)
{
    animManager->Update(animState, dt);
    if (SpriteAnimUtils::IsDelayed(animState)) return true;

    remainingLifetime -= dt;

    if (remainingLifetime < 0.f)
        return false;

    velocity += dt * gravity;
    velocity.x *= 1.0f / (1.0f + dt * damping.x);
    velocity.y *= 1.0f / (1.0f + dt * damping.y);

    position += dt * velocity;
    angle += dt * angularVelocity;

    AnimUtils::Update(alphaState, dt);
    AnimUtils::Update(scaleState, dt);

    return true;
}

void ParticleSystem::Update()
{
    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    const float dt = m_scene->GetDelta();
    for (auto &[layer, particles] : particlePools)
    {
        for (int i = 0; i < particles.size();)
        {
            Particle &particle = particles[i];
            const bool isAlive = particle.Update(animManager, dt);
            if (isAlive == false)
            {
                particles[i] = particles.back();
                particles.pop_back();
            }
            else i++;
        }
    }
}

void Particle::Render(
    SpriteAnimManager *animManager, SDL_Renderer *renderer, 
    const Camera &camera, const Transform &cameraTransform)
{
    SpriteAnim *spriteAnim = animManager->GetSpriteAnim(animState);
    if (spriteAnim == nullptr || SpriteAnimUtils::IsDelayed(animState))
    {
        return;
    }
    SpriteGroup *spriteGroup = spriteAnim->GetSpriteGroup();
    if (spriteGroup == nullptr) return;

    SDL_Texture *texture = spriteGroup->GetTexture();
    const SDL_FRect *srcRect = spriteGroup->GetSourceRect(animState.frameIdx);

    const float scale = AnimUtils::GetValue(scaleState, scaleValues);
    const float scaleX = camera.GetWorldToViewScaleX() * scale;
    const float scaleY = camera.GetWorldToViewScaleY() * scale;
    SDL_FRect dstRect = { 0 };
    dstRect.x = camera.WorldToViewX(cameraTransform.position.x, position.x);
    dstRect.y = camera.WorldToViewY(cameraTransform.position.y, position.y);
    dstRect.w = scaleX * srcRect->w / pixPerUnit;
    dstRect.h = scaleY * srcRect->h / pixPerUnit;

    const float alpha = opacity * AnimUtils::GetValue(alphaState, alphaValues);
    bool success = SDL_SetTextureAlphaModFloat(texture, alpha);
    assert(success);
    success = SDL_SetTextureBlendMode(texture, blendMode);
    assert(success);

    RenderTextureRotated(
        g_renderer, texture, srcRect, &dstRect, anchor,
        -angle, flip
    );
}

void Particle::SetLifetimeFromAnim(SpriteAnimManager *animManager)
{
    SpriteAnim *spriteAnim = animManager->GetSpriteAnim(animState);
    assert(spriteAnim);
    lifetime = spriteAnim->GetTotalTime();
    remainingLifetime = lifetime;
    alphaState.cycleTime = lifetime;
    scaleState.cycleTime = lifetime;
}

void Particle::SetLifetime(float lifetimeIn)
{
    lifetime = lifetimeIn;
    alphaState.cycleTime = lifetime;
    scaleState.cycleTime = lifetime;
}

void Particle::SetDelay(float delay)
{
    SpriteAnimUtils::Play(animState, delay);
    AnimUtils::Play(alphaState, delay);
    AnimUtils::Play(scaleState, delay);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::EmitParticle(int layer, Particle particle)
{
    particle.remainingLifetime = particle.lifetime;
    particlePools[layer].push_back(particle);
}

void ParticleSystem::renderParticleLayers(
    const Camera &camera,
    const Transform &cameraTransform,
    int lowerLayer, int upperLayer)
{
    auto itEnd = particlePools.upper_bound(upperLayer);
    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    for (auto it = particlePools.lower_bound(lowerLayer); it != itEnd; ++it)
    {
        std::vector<Particle> &particles = it->second;
        for (auto &particle : particles)
        {
            particle.Render(animManager, g_renderer, camera, cameraTransform);
        }
    }
}
