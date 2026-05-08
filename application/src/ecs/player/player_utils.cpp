/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_utils.h"

void PlayerUtils::EmitSmallDust(
    Scene *scene, b2Vec2 position, bool facingRight, float xShift)
{
    SpriteAnimManager *animManager = scene->GetAnimManager();
    ParticleSystem *particleSystem = scene->GetParticleSystem();
    AnimType type = (rand() & 1) ? AnimType::SMALL_DUST_1 : AnimType::SMALL_DUST_2;
    Particle particle(AnimID_Make(AnimCategory::DUST, type));

    particle.SetLifetimeFromAnim(animManager);
    particle.anchor = Anchor::SOUTH;
    particle.position = position + b2Vec2{ xShift, 0.0f };
    particle.flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    particle.pixPerUnit = 20.f;
    particle.alphaValues.value0 = 0.6f;
    particle.alphaValues.value1 = 0.0f;
    particle.alphaValues.easing = EasingFct_In;

    particleSystem->EmitParticle(LAYER_PARTICLES, particle);
}

void PlayerUtils::EmitRollDust(Scene *scene, b2Vec2 position, bool facingRight)
{
    // Bonus : Poussière lors d'un roulade
    SpriteAnimManager* animManager = scene->GetAnimManager();
    ParticleSystem* particleSystem = scene->GetParticleSystem();
    Particle particle(AnimID_Make(AnimCategory::DUST, AnimType::ROLL_DUST));

    particle.SetLifetimeFromAnim(animManager);
    particle.anchor = Anchor::SOUTH;
    particle.position = position + b2Vec2{ 0.f, -0.1f };
    particle.SetDelay(0.1f);
    particle.flip = facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    particle.pixPerUnit = 44.f;
    particle.alphaValues.value0 = 0.8f;
    particle.alphaValues.value1 = 0.4f;
    particle.alphaValues.easing = EasingFct_In;

    particleSystem->EmitParticle(LAYER_PARTICLES, particle);
}

void PlayerUtils::EmitSkidDust(Scene *scene, b2Vec2 position, bool facingRight)
{
    // Bonus : Poussière lors d'un dérapage

}

void PlayerUtils::EmitJumpDust(Scene *scene, b2Vec2 position, bool facingRight)
{
    SpriteAnimManager *animManager = scene->GetAnimManager();
    ParticleSystem *particleSystem = scene->GetParticleSystem();
    Particle particle(AnimID_Make(AnimCategory::DUST, AnimType::JUMP_DUST));

    particle.SetLifetimeFromAnim(animManager);
    particle.anchor = Anchor::SOUTH;
    particle.position = position + b2Vec2{ 0.f, 0.0f };
    particle.pixPerUnit = 44.f;
    particle.alphaValues.value0 = 0.5f;
    particle.alphaValues.value1 = 0.0f;
    particle.alphaValues.easing = EasingFct_In;

    particleSystem->EmitParticle(LAYER_PARTICLES, particle);
}

void PlayerUtils::EmitLandDust(Scene *scene, b2Vec2 position, bool facingRight)
{
    // TODO 10 - Décommentez cette partie 

    SpriteAnimManager *animManager = scene->GetAnimManager();
    ParticleSystem *particleSystem = scene->GetParticleSystem();
    Particle particle(AnimID_Make(AnimCategory::DUST, AnimType::LAND_DUST));

    particle.SetLifetimeFromAnim(animManager);
    particle.anchor = Anchor::SOUTH;
    particle.position = position + b2Vec2{ 0.f, -0.1f };
    particle.SetDelay(0.1f);
    particle.flip = facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    particle.pixPerUnit = 44.f;
    particle.alphaValues.value0 = 0.8f;
    particle.alphaValues.value1 = 0.4f;
    particle.alphaValues.easing = EasingFct_In;

    particleSystem->EmitParticle(LAYER_PARTICLES, particle);
}

void PlayerUtils::EmitPowerAura(Scene *scene, b2Vec2 position)
{
    // Bonus : Aura lors du chargement d'un smash
}

void PlayerUtils::EmitChargedParticule(Scene *scene, b2Vec2 position)
{
    // Bonus : Particules lors du chargement d'un smash
}

void PlayerUtils::EmitEjectionParticles(Scene *scene, b2Vec2 position, b2Vec2 ejection)
{
    const float speed = b2Length(ejection);
    const float angle = math::AngleDeg(b2Vec2(1.f, 0.f), ejection);

    // Bonus : Améliorer le comportement des particules

    SpriteAnimManager *animManager = scene->GetAnimManager();
    ParticleSystem *particleSystem = scene->GetParticleSystem();
    for (int i = 0; i < 30; i++)
    {
        Particle particle(AnimID_Make(AnimCategory::VFX, AnimType::SQUARE_PARTICULE));
        b2Vec2 velocity = math::UnitVectorDeg(angle + random::RangeF(-15.f, 15.f));
        velocity *= random::RangeF(5.f, 15.0f);

        particle.SetLifetime(random::RangeF(0.8f, 1.2f));
        particle.position = position + b2Vec2{ 0.f, 1.f };
        particle.velocity = velocity;
        particle.damping = b2Vec2{ 1.f, 1.f };
        particle.gravity = b2Vec2{ 0.f, -10.f };
        particle.pixPerUnit = 24.f;
        particle.blendMode = SDL_BLENDMODE_ADD;
        particle.alphaValues.value0 = 1.0f;
        particle.alphaValues.value1 = 0.0f;
        particle.alphaValues.easing = EasingFct_In;
        particle.scaleValues.value0 = 1.0f;
        particle.scaleValues.value1 = 0.2f;

        particleSystem->EmitParticle(LAYER_PARTICLES, particle);
    }
}

void PlayerUtils::EmitHealAura(Scene *scene, b2Vec2 position)
{
    // Aura autour du joueur lors d'un soin avec une potion
    SpriteAnimManager* animManager = scene->GetAnimManager();
    ParticleSystem* particleSystem = scene->GetParticleSystem();
    Particle particle(AnimID_Make(AnimCategory::DUST, AnimType::HEAL));

    particle.SetLifetimeFromAnim(animManager);
    particle.anchor = Anchor::SOUTH;
    particle.position = position + b2Vec2{ 0.f, 0.f };
    particle.SetDelay(0.1f);
    particle.pixPerUnit = 44.f;
    particle.alphaValues.value0 = 0.8f;
    particle.alphaValues.value1 = 0.4f;
    particle.alphaValues.easing = EasingFct_In;

    particleSystem->EmitParticle(LAYER_PARTICLES, particle);
}

void PlayerUtils::PlaySFXAttack(Scene *scene, int playerID, SoundID soundID, SFXIntensity intensity)
{
    AssetManager *assets = scene->GetAssetManager();
    float volume = 1.f;

    // Vous pouvez modifier le volume en fonction par exemple de l'intensité

    switch (intensity)
    {
    default:
    case SFXIntensity::MAX:
        volume = 1.f;
        break;
    }

    assets->SetSoundVolume(soundID, volume);
    assets->PlaySoundFX(soundID);
}

void PlayerUtils::PlaySFXHit(Scene *scene, int playerID, SoundID soundID, SFXIntensity intensity, bool hit)
{
    if (hit == false) return;

    AssetManager *assets = scene->GetAssetManager();
    float volume = 1.f;

    // Vous pouvez modifier le volume en fonction par exemple de l'intensité

    switch (intensity)
    {
    default:
    case SFXIntensity::MAX:
        volume = 1.f;
        break;
    }

    assets->SetSoundVolume(soundID, volume);
    assets->PlaySoundFX(soundID);
}

bool PlayerUtils::IsAttacking(const PlayerController &controller)
{
    return 
        (controller.currState == PlayerState::ATTACK_COMBO) ||
        (controller.currState == PlayerState::ATTACK_SPECIAL) ||
        (controller.currState == PlayerState::ATTACK_AIR) ||
        (controller.currState == PlayerState::SMASH_HOLD) ||
        (controller.currState == PlayerState::SMASH_RELEASE);
}

void PlayerUtils::SetState(PlayerController &controller, PlayerState state)
{
    if (controller.currState == state) return;
    controller.currState = state;
    controller.isStateUpdated = true;
}

bool PlayerUtils::HealIfPlayer(entt::registry &registry, entt::entity entity, float amount)
{
    if (registry.valid(entity) &&
        registry.all_of<PlayerController, PlayerAffiliation, Damageable>(entity))
    {
        const auto &affiliation = registry.get<PlayerAffiliation>(entity);
        auto &damageable = registry.get<Damageable>(entity);
        damageable.ejectionScore -= amount;
        damageable.ejectionScore = fmaxf(0.f, damageable.ejectionScore);

        PlayerStats *stats = g_gameCommon.GetPlayerStats(affiliation.playerID);
        if (stats)
        {
            stats->ejectionScore = damageable.ejectionScore;
        }
        return true;
    }
    return false;
}
