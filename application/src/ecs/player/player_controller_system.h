/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"
#include "ecs/player/player_components.h"

// très gros système permettant d’implémenter la logique du joueur
class PlayerControllerSystem : public System
{
public:
    PlayerControllerSystem(Scene *scene) : System(scene, "Player controller system") {}

    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

protected:
    bool OnPlayerFall(
        entt::entity entity,
        const Rigidbody &rigidbody,
        const PlayerAffiliation &affiliation,
        Damageable &damageable,
        PlayerController &controller);

    void FixedUpdateShieldAndDamage(
        const PlayerAffiliation &affiliation,
        const GroundContact &ground,
        PlayerController &controller,
        Damageable &damageable);

    void FixedUpdateState(
        const Rigidbody &rigidbody,
        Damageable &damageable,
        const PlayerControllerInput &input,
        PlayerController &controller,
        const GroundContact &ground,
        PlayerAnimInfo &event);

    void FixedUpdateOnStateChanged(
        const Rigidbody &rigidbody,
        Damageable &damageable,
        const PlayerControllerInput &input,
        PlayerController &controller,
        const GroundContact &ground);

    void FixedUpdateAutoVelocity(
        PlayerController &controller,
        const Rigidbody &rigidbody,
        const GroundContact &ground,
        PlayerAnimInfo &animInfo);

    void FixedUpdatePhysics(
        entt::entity entity,
        Rigidbody &rigidbody,
        const Damageable &damageable,
        const PlayerControllerInput &input,
        PlayerController &controller,
        const GroundContact &ground,
        PlayerAnimInfo &animInfo);
};
