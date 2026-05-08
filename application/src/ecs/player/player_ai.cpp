/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_ai.h"
#include "ecs/player/player_utils.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"

PlayerAI::PlayerAI(int level)
    : level(level)
    , rollCount(0)
    , specialCount(0)
    , terrainL(false)
    , terrainR(false)
    , terrainBelow(false)
    , jumpThresold(0.f)
    , targetDistance(0.f)
    , desiredTargetDistance(2.5f)
    , delayDefend(-1.f)
{}

void PlayerAISystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<
        PlayerAI,
        TrackedTarget,
        PlayerControllerInput,
        const Rigidbody,
        const Damageable,
        const PlayerAffiliation,
        const PlayerController,
        const GroundContact
    >();
    for (auto [entity, playerAI, target, input, rigidbody, damageable, affiliation, controller, ground] : view.each())
    {
        ResetInput(input);

        // Recherche le sol
        SearchTerrain(playerAI, input, rigidbody, damageable, controller, ground);
        if (playerAI.terrainBelow == false)
        {
            ComeBack(playerAI, input, rigidbody, damageable, controller, ground);
            continue;
        }

        if (controller.isStateUpdated)
        {
            OnStateUpdated(playerAI, controller);
        }

        // Recherche une cible
        b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);
        const uint64_t otherTeamMask = g_gameCommon.GetOtherTeamMask(affiliation.playerID);
        TargetUtils::SearchTarget(target, m_registry, otherTeamMask, position);
        if (m_registry.valid(target.entity) == false) continue;


        // Se dirige vers la cible
        b2Vec2 targetPosition = target.position;
        playerAI.targetDistance = fabsf(targetPosition.x - position.x);
        if (playerAI.targetDistance > playerAI.desiredTargetDistance)
        {
            JoinTarget(playerAI, input, target, rigidbody, damageable, controller, ground);
        }

        // Attaque
        if (damageable.lockAttackTime >= 0.f) continue;
        if (playerAI.targetDistance <= playerAI.desiredTargetDistance)
        {
            input.attackType = AttackType::COMBO;
            input.attackPressed = true;
            input.attackDown = true;
        }
    }
}

void PlayerAISystem::OnStateUpdated(PlayerAI &playerAI, const PlayerController &controller)
{
    switch (controller.currState)
    {
    case PlayerState::ATTACK_COMBO:
    case PlayerState::ATTACK_AIR:
    case PlayerState::ATTACK_SPECIAL:
    case PlayerState::SMASH_HOLD:
    case PlayerState::SMASH_RELEASE:
        playerAI.desiredTargetDistance = random::RangeF(0.5f, 3.5f);
        break;
    default: break;
    }
}

void PlayerAISystem::ComeBack(
    PlayerAI &playerAI, PlayerControllerInput &input,
    const Rigidbody &rigidbody, const Damageable &damageable,
    const PlayerController &controller, const GroundContact &ground)
{
    b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);
    b2Vec2 velocity = b2Body_GetLinearVelocity(rigidbody.bodyId);

    float axisValue = 1.f;
    if (position.x > 0.f)
    {
        input.direction = -axisValue;
    }
    else if (position.x < 0.f)
    {
        input.direction = axisValue;
    }

    if (velocity.y < 0.0f && position.y < 0.f)
    {
        if (controller.bonusJumpCount > 0)
        {
            input.jumpPressed = true;
        }
    }

    if (velocity.y > 0.0f) input.jumpDown = true;
}

void PlayerAISystem::JoinTarget(
    PlayerAI &playerAI, PlayerControllerInput &input, TrackedTarget &target,
    const Rigidbody &rigidbody, const Damageable &damageable,
    const PlayerController &controller, const GroundContact &ground)
{
    if (m_registry.valid(target.entity) == false)
        return;

    const b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);

    if (position.x < target.position.x - playerAI.desiredTargetDistance)
    {
        input.direction = +1.f;
    }
    else if (position.x > target.position.x + playerAI.desiredTargetDistance)
    {
        input.direction = -1.f;
    }
}

void PlayerAISystem::SearchTerrain(
    PlayerAI &playerAI, PlayerControllerInput &input,
    const Rigidbody &rigidbody, const Damageable &damageable,
    const PlayerController &controller, const GroundContact &ground)
{
    QueryFilter filter(CATEGORY_TERRAIN);
    b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);
    b2Vec2 point1, point2;
    RayHit rayHit;

    if (position.y < -2.f)
    {
        playerAI.terrainL = false;
        playerAI.terrainR = false;
        playerAI.terrainBelow = false;
        return;
    }

    point1 = position + b2Vec2(-2.f, 1.f);
    point2 = point1 + b2Vec2(0.f, -5.0f);
    rayHit = m_scene->RayCastFirstGizmos(point1, point2, filter);
    playerAI.terrainL = (rayHit.entity != entt::null);

    point1 = position + b2Vec2(+2.f, 1.f);
    point2 = point1 + b2Vec2(0.f, -5.0f);
    rayHit = m_scene->RayCastFirstGizmos(point1, point2, filter);
    playerAI.terrainR = (rayHit.entity != entt::null);

    if (ground.isGrounded)
    {
        playerAI.terrainBelow = true;
    }
    else
    {
        point1 = position;
        point2 = point1 + b2Vec2(0.f, -20.0f);
        rayHit = m_scene->RayCastFirstGizmos(point1, point2, filter);
        playerAI.terrainBelow = (rayHit.entity != entt::null);
    }
}

void PlayerAISystem::ResetInput(PlayerControllerInput &input)
{
    input.direction = 0.f;
    input.jumpPressed = false;
    input.jumpDown = false;
    input.attackPressed = false;
    input.attackDown = false;
    input.defendDown = false;
}
