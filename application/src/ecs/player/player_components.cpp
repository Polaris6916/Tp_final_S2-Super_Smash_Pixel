/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_components.h"

PlayerControllerInput::PlayerControllerInput()
    : direction(0.f)
    , jumpPressed(false)
    , jumpDown(false)
    , attackPressed(false)
    , attackDown(false)
    , defendDown(false)
    , attackType(AttackType::COMBO)
    , oneWayPassDown(true)
{
}

PlayerController::PlayerController(PlayerType type)
    : currState(PlayerState::IDLE)
    , prevState(PlayerState::IDLE)
    , isStateUpdated(true)
    , externalVelocity{ 0.f, 0.f }
    , hVelocity(0.f)
    , hasAutoVelocity(false)
    , jumpImpulse(14.f)
    , groundAcc(60.f)
    , airAcc(30.f)
    , maxSpeed(8.f)
    , facingRight(true)
    , delayEarlyJump(-1.f)
    , delayAttack(-1.f)
    , delaySmashReleaseMin(-1.f)
    , delaySmashReleaseMax(-1.f)
    , delayBonusJump(-1.f)
    , delayRoll(-1.f)
    , delayCoyoteJump(-1.f)
    , delayClearLastDamager(-1.f)
    , shieldPower(1.f)
    , airAttackCount(1)
    , bonusJumpCount(1)
    , maxBonusJumpCount(1)
    , smashMultiplier(1.f)
    , lastDamagerEntity(entt::null)
{
    // TODO - Modifiez la vitesse maximale du FIRE_KNIGHT pour 8 unités/s.

    switch (type)
    {
    default:
    case PlayerType::FIRE_KNIGHT:
        airAcc = 40.f;
        groundAcc = 60.f;
        maxSpeed = 8.f;
        maxBonusJumpCount = 2;
        jumpImpulse = 11.f;
        break;
    case PlayerType::METAL_BLADEKEEPER:
        airAcc = 30.f;
        groundAcc = 60.f;
        maxSpeed = 8.f;
        maxBonusJumpCount = 1;
        jumpImpulse = 14.f;
        break;
    case PlayerType::LEAF_RANGER:
        airAcc = 30.f;
        groundAcc = 60.f;
        maxSpeed = 8.f;
        maxBonusJumpCount = 1;
        jumpImpulse = 14.f;
        break;
    case PlayerType::WATER_PRIESTESS:
        airAcc = 40.f;
        groundAcc = 60.f;
        maxSpeed = 6.f;
        maxBonusJumpCount = 2;
        jumpImpulse = 11.f;
        break;
    }
}

PlayerAnimInfo::PlayerAnimInfo()
    : type(Event::NONE), autoVelocity(0.f)
{
}
