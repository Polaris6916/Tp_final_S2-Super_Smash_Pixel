/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_input_system.h"
#include "ecs/player/player_components.h"
#include "ecs/player/player_ai.h"
#include "scene_manager/stage_manager.h"

void PlayerInputSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    if (stageManager == nullptr) return;
    if (stageManager->IsPaused()) return;

    ControlsInput *controlsInput = ControlsInput::GetFromManager(m_scene->GetInputManager());

    auto view = m_registry.view<PlayerControllerInput, const PlayerAffiliation>(entt::exclude<PlayerAI>);
    for (auto [entity, controllerInput, affiliation] : view.each())
    {
        if (affiliation.playerID < 0) continue;
        if (affiliation.playerID >= MAX_PLAYER_COUNT) continue;

        const PlayerInput &input = controlsInput->players[affiliation.playerID];
        controllerInput.attackPressed = false;
        controllerInput.jumpPressed = false;

        controllerInput.direction = input.axisX;

        if (input.jumpPressed)
        {
            // TODO 3 - Modifiez le champ jumpPressed du controllerInput.
            controllerInput.jumpPressed = true;
        }


        if (input.smashPressed)
        {
            controllerInput.attackPressed = true;
            controllerInput.attackType = AttackType::SMASH;
        }
        else if (input.specialPressed)
        {
            controllerInput.attackPressed = true;
            controllerInput.attackType = AttackType::SPECIAL;
        }
        else if (input.attackPressed)
        {
            // TODO 11 - Lors de la gestion des attaques, plus loin dans le tuto.
            //      - Décommentez les lignes suivantes.
            //      - Modifiez les deux tests précédents pour attribuer le bon type d'attaque.

            controllerInput.attackPressed = true;
            controllerInput.attackType = AttackType::COMBO;
        }

        switch (controllerInput.attackType)
        {
        case AttackType::COMBO: controllerInput.attackDown = input.attackDown; break;
        case AttackType::SPECIAL: controllerInput.attackDown = input.specialDown; break;
        case AttackType::SMASH: controllerInput.attackDown = input.smashDown; break;
        default: break;
        }

        controllerInput.defendDown = input.defendDown;
        controllerInput.jumpDown = input.jumpDown;
        controllerInput.oneWayPassDown = input.oneWayPassDown;
    }
}
