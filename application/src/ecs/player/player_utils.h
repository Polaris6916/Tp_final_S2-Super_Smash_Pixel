/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

#include "ecs/player/player_components.h"

enum class SFXIntensity
{
    WEAK, NORMAL, STRONG, MAX
};

class PlayerUtils
{
public:
    static void EmitSmallDust(Scene *scene, b2Vec2 position, bool facingRight, float xShift);
    static void EmitRollDust(Scene *scene, b2Vec2 position, bool facingRight);
    static void EmitSkidDust(Scene *scene, b2Vec2 position, bool facingRight);
    static void EmitJumpDust(Scene *scene, b2Vec2 position, bool facingRight);
    static void EmitLandDust(Scene *scene, b2Vec2 position, bool facingRight);
    static void EmitPowerAura(Scene *scene, b2Vec2 position);
    static void EmitChargedParticule(Scene *scene, b2Vec2 position);
    static void EmitEjectionParticles(Scene *scene, b2Vec2 position, b2Vec2 ejection);
    static void EmitHealAura(Scene *scene, b2Vec2 position);

    static void PlaySFXAttack(Scene *scene, int playerID, SoundID soundID, SFXIntensity intensity);
    static void PlaySFXHit(Scene *scene, int playerID, SoundID soundID, SFXIntensity intensity, bool hit);

    static bool IsAttacking(const PlayerController &controller);
    static void SetState(PlayerController &controller, PlayerState state);
    static bool HealIfPlayer(entt::registry &registry, entt::entity entity, float amount);

private:
    PlayerUtils() = delete;
};
