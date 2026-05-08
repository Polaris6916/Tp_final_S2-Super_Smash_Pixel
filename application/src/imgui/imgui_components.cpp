/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui/imgui_components.h"
#include "imgui/imgui_manager.h"

constexpr ImGuiTableFlags membersTableFlags = ImGuiTableFlags_RowBg;

void ImGuiDamageable::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<Damageable>(entity) == false) return;
    auto &damageable = registry.get<Damageable>(entity);

    if (ImGui::BeginTable("DamageableComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Ejection score", damageable.ejectionScore);
        ImGuiUtils::Member("Lock time", damageable.lockTime);
        ImGuiUtils::Member("Lock attack time", damageable.lockAttackTime);

        ImGui::EndTable();
    }
}

void ImGuiGroundContact::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<GroundContact>(entity) == false) return;
    auto &ground = registry.get<GroundContact>(entity);

    if (ImGui::BeginTable("GroundContactComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Is grounded", ground.isGrounded);

        ImGuiUtils::Member("Was grounded", ground.wasGrounded);
        ImGuiUtils::Member("In contact", ground.inContact);
        ImGuiUtils::Member("Ground velocity", ground.groundVelocity);
        ImGuiUtils::Member("Curr velocity", ground.currVelocity);
        ImGuiUtils::Member("Prev velocity", ground.prevVelocity);

        ImGui::EndTable();
    }
}

void ImGuiPlayerAnimInfo::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<PlayerAnimInfo>(entity) == false) return;
    auto &animInfo = registry.get<PlayerAnimInfo>(entity);

    if (ImGui::BeginTable("PlayerAnimInfo Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Event", ENUM_STRING(animInfo.type));
        ImGuiUtils::Member("Auto velocity", animInfo.autoVelocity);
        ImGui::EndTable();
    }
}

void ImGuiPlayerController::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<PlayerController>(entity) == false) return;
    auto &controller = registry.get<PlayerController>(entity);

    if (ImGui::BeginTable("PlayerController Members", 2, membersTableFlags))
    {
        if (ImGuiUtils::BeginSection("Player state"))
        {
            ImGuiUtils::Member("Curr state", ENUM_STRING(controller.currState));
            ImGuiUtils::Member("Prev state", ENUM_STRING(controller.prevState));
            ImGuiUtils::Member("Is state updated", controller.isStateUpdated);
            ImGuiUtils::Member("Facing right", controller.facingRight);
            ImGuiUtils::Member("Shield power", controller.shieldPower);
            ImGuiUtils::Member("Smash multiplier", controller.smashMultiplier);
            ImGuiUtils::Member("Air attack count", controller.airAttackCount);
            ImGuiUtils::Member("Bonus jump count", controller.bonusJumpCount);
            ImGuiUtils::Member("Horizontal velocity", controller.hVelocity);
            ImGuiUtils::Member("External velocity", controller.externalVelocity);
            ImGuiUtils::Member("Has auto velocity", controller.hasAutoVelocity);
            ImGuiUtils::Member("Last damager entity", controller.lastDamagerEntity, registry, manager);
            ImGuiUtils::Member("Last damager affiliation", controller.lastDamagerAffiliation.playerID);

            ImGuiUtils::EndSection();
        }
        if (ImGuiUtils::BeginSection("Delays"))
        {
            ImGuiUtils::Member("Delay early jump", controller.delayEarlyJump);
            ImGuiUtils::Member("Delay coyote jump", controller.delayCoyoteJump);
            ImGuiUtils::Member("Delay bonus jump", controller.delayBonusJump);
            ImGuiUtils::Member("Delay attack", controller.delayAttack);
            ImGuiUtils::Member("Delay smash release min", controller.delaySmashReleaseMin);
            ImGuiUtils::Member("Delay smash release max", controller.delaySmashReleaseMax);
            ImGuiUtils::Member("Delay roll", controller.delayRoll);
            ImGuiUtils::Member("Delay clear last damager", controller.delayClearLastDamager);

            ImGuiUtils::EndSection();
        }
        if (ImGuiUtils::BeginSection("Player parameters"))
        {
            ImGuiUtils::Member("Ground acceleration", controller.groundAcc);
            ImGuiUtils::Member("Air acceleration", controller.airAcc);
            ImGuiUtils::Member("Maximum speed", controller.maxSpeed);
            ImGuiUtils::Member("Jump impulse", controller.jumpImpulse);
            ImGuiUtils::Member("Max bonus jump count", controller.maxBonusJumpCount);

            ImGuiUtils::EndSection();
        }

        ImGui::EndTable();
    }
}

void ImGuiPlayerControllerInput::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<PlayerControllerInput>(entity) == false) return;
    auto &input = registry.get<PlayerControllerInput>(entity);

    if (ImGui::BeginTable("PlayerControllerInput Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Direction", input.direction);
        ImGuiUtils::Member("Jump pressed", input.jumpPressed);
        ImGuiUtils::Member("Jump down", input.jumpDown);
        ImGuiUtils::Member("Attack type", ENUM_STRING(input.attackType));
        ImGuiUtils::Member("Attack pressed", input.attackPressed);
        ImGuiUtils::Member("Attack down", input.attackDown);
        ImGuiUtils::Member("Defend down", input.defendDown);
        ImGui::EndTable();
    }
}

void ImGuiPlayerAI::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<PlayerAI>(entity) == false) return;
    auto &playerAI = registry.get<PlayerAI>(entity);

    if (ImGui::BeginTable("PlayerAI Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Level", playerAI.level);
        ImGuiUtils::Member("Roll count", playerAI.rollCount);
        ImGuiUtils::Member("Special count", playerAI.specialCount);
        ImGuiUtils::Member("Terrain L", playerAI.terrainL);
        ImGuiUtils::Member("Terrain R", playerAI.terrainR);
        ImGuiUtils::Member("Terrain below", playerAI.terrainBelow);
        ImGuiUtils::Member("Jump thresold", playerAI.jumpThresold);
        ImGuiUtils::Member("Desired target dist", playerAI.desiredTargetDistance);
        ImGuiUtils::Member("Target distance", playerAI.targetDistance);
        ImGuiUtils::Member("Delay defend", playerAI.delayDefend);
        ImGui::EndTable();
    }
}

void ImGuiTrackedTarget::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<TrackedTarget>(entity) == false) return;
    auto &target = registry.get<TrackedTarget>(entity);

    if (ImGui::BeginTable("TrackedTarget Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Entity", ImGuiManager::GetEntityName(registry, target.entity).c_str());
        ImGuiUtils::Member("Position", target.position);
        ImGuiUtils::Member("Is enabled", target.isEnabled);
        ImGuiUtils::Member("Is grounded", target.isGrounded);
        ImGuiUtils::Member("Ejection score", target.ejectionScore);
        ImGui::EndTable();
    }
}

void ImGuiOneWayPass::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<OneWayPass>(entity) == false) return;
    auto &oneWayPass = registry.get<OneWayPass>(entity);

    if (ImGui::BeginTable("OneWayPass Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Delay", oneWayPass.delay);
        ImGui::EndTable();
    }
}

void ImGuiPlayerAffiliation::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<PlayerAffiliation>(entity) == false) return;
    auto &affiliation = registry.get<PlayerAffiliation>(entity);

    if (ImGui::BeginTable("PlayerAffiliation Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Player ID", affiliation.playerID);
        ImGui::EndTable();
    }
}

void ImGuiVisualID::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<VisualID>(entity) == false) return;
    auto &visualID = registry.get<VisualID>(entity);

    if (ImGui::BeginTable("VisualID Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Is always visible", visualID.alwaysVisible);
        ImGui::EndTable();
    }
}

void ImGuiCameraFollow::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<CameraFollow>(entity) == false) return;
    auto &follow = registry.get<CameraFollow>(entity);

    if (ImGui::BeginTable("CameraFollow Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Target", follow.target);
        ImGuiUtils::Member("Velocity", follow.velocity);
        ImGui::EndTable();
    }
}

void ImGuiReferencePosition::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<ReferencePosition>(entity) == false) return;
    auto &ref = registry.get<ReferencePosition>(entity);

    if (ImGui::BeginTable("ReferencePosition Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Position", ref.position);
        ImGui::EndTable();
    }
}

void ImGuiKinematicTargetPosition::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<KinematicTargetPosition>(entity) == false) return;
    auto &targetPos = registry.get<KinematicTargetPosition>(entity);

    if (ImGui::BeginTable("KinematicTargetPosition Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Target", targetPos.target);
        ImGuiUtils::Member("Smooth time", targetPos.smoothTime);
        ImGuiUtils::Member("Max speed", targetPos.maxSpeed);
        ImGui::EndTable();
    }
}

void ImGuiKinematicTargetRotation::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<KinematicTargetRotation>(entity) == false) return;
    auto &targetRot = registry.get<KinematicTargetRotation>(entity);

    if (ImGui::BeginTable("KinematicTargetRotation Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Target", targetRot.target);
        ImGuiUtils::Member("Smooth time", targetRot.smoothTime);
        ImGuiUtils::Member("Max speed", targetRot.maxSpeed);
        ImGui::EndTable();
    }
}
