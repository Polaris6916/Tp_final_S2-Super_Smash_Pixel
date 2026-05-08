/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ecs/common/damage_utils.h"

enum class AttackType { COMBO, SPECIAL, SMASH };

// : donne l’état des contrôles appliqués au joueur par la manette ou le clavier 
struct PlayerControllerInput
{
    PlayerControllerInput();

    float direction;
    bool jumpPressed;
    bool jumpDown;
    bool attackPressed;
    bool attackDown;
    bool defendDown;
    bool oneWayPassDown;
    AttackType attackType;
};

enum class PlayerState
{
    IDLE, RUN, SKID, ROLL, JUMP, FALL, ROLL_END,
    ATTACK_COMBO, ATTACK_AIR, ATTACK_SPECIAL,
    SMASH_HOLD, SMASH_RELEASE,
    DEFEND, TAKE_DAMAGE, LAUNCHED,
};

// paramètres généraux du joueur
struct PlayerController
{
    PlayerController(PlayerType type);

    PlayerState currState;
    PlayerState prevState;
    bool isStateUpdated;

    b2Vec2 externalVelocity;
    float hVelocity;
    bool hasAutoVelocity;
    bool facingRight;
    float shieldPower;
    float smashMultiplier;
    int airAttackCount;
    int bonusJumpCount;

    float delayEarlyJump;
    float delayAttack;
    float delayRoll;
    float delaySmashReleaseMin;
    float delaySmashReleaseMax;
    float delayCoyoteJump;
    float delayBonusJump;
    float delayClearLastDamager;

    float groundAcc;
    float airAcc;
    float maxSpeed;
    float jumpImpulse;
    int maxBonusJumpCount;

    entt::entity lastDamagerEntity;
    PlayerAffiliation lastDamagerAffiliation;
};

// informe des fins d’animations qui doivent modifier l’état du joueur
struct PlayerAnimInfo
{
    PlayerAnimInfo();

    enum class Event
    {
        NONE, ROLL_END, COMBO_END, SPECIAL_END, SMASH_END, AIR_ATTACK_END, TAKE_HIT_END
    };
    Event type;
    float autoVelocity;
};

// des composants vides utilisés à des fins de déboguage
struct FireKnightTag {};
struct WaterPriestessTag {};
struct LeafRangerTag {};
struct MetalBladekeeperTag {};
