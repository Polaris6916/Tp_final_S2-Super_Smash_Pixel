/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ecs/common/tracked_target.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"

#include "ecs/player/player_components.h"

struct PlayerAI
{
    PlayerAI(int level);

    int level;
    int rollCount;
    int specialCount;

    bool terrainL;
    bool terrainR;
    bool terrainBelow;

    float jumpThresold;
    float desiredTargetDistance;
    float targetDistance;
    float delayDefend;
};

class PlayerAISystem : public System
{
public:
    PlayerAISystem(Scene *scene) : System(scene, "Player AI system") {}

    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;

protected:
    void OnStateUpdated(PlayerAI &playerAI, const PlayerController &controller);
    void ComeBack(
        PlayerAI &playerAI, PlayerControllerInput &input,
        const Rigidbody &rigidbody, const Damageable &damageable,
        const PlayerController &controller, const GroundContact &ground
    );
    void JoinTarget(
        PlayerAI &playerAI, PlayerControllerInput &input, TrackedTarget &target,
        const Rigidbody &rigidbody, const Damageable &damageable,
        const PlayerController &controller, const GroundContact &ground
    );
    void SearchTerrain(
        PlayerAI &playerAI, PlayerControllerInput &input,
        const Rigidbody &rigidbody, const Damageable &damageable,
        const PlayerController &controller, const GroundContact &ground
    );
    void ResetInput(PlayerControllerInput &input);
};
