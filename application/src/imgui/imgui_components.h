/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

#include "ecs/common/camera.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/kinematic_utils.h"
#include "ecs/common/terrain_utils.h"
#include "ecs/common/tracked_target.h"
#include "ecs/common/visual_id.h"

#include "ecs/player/player_components.h"
#include "ecs/player/player_command.h"
#include "ecs/player/player_controller_system.h"
#include "ecs/player/shield.h"
#include "ecs/player/fire_knight_system.h"
#include "ecs/player/player_ai.h"

#include "ecs/item/potion.h"
#include "ecs/item/bomb.h"

#include "ecs/stage/star_fields_terrain.h"
#include "ecs/stage/star_fields_platform.h"

class ImGuiDamageable : public ImGuiBaseComponent
{
public:
    ImGuiDamageable()
        : ImGuiBaseComponent("Damageable")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<Damageable>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiGroundContact : public ImGuiBaseComponent
{
public:
    ImGuiGroundContact()
        : ImGuiBaseComponent("Ground contact")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<GroundContact>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiReferencePosition : public ImGuiBaseComponent
{
public:
    ImGuiReferencePosition()
        : ImGuiBaseComponent("Reference position")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<ReferencePosition>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiKinematicTargetPosition : public ImGuiBaseComponent
{
public:
    ImGuiKinematicTargetPosition()
        : ImGuiBaseComponent("Kinematic target position")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<KinematicTargetPosition>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiKinematicTargetRotation: public ImGuiBaseComponent
{
public:
    ImGuiKinematicTargetRotation()
        : ImGuiBaseComponent("Kinematic target rotation")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<KinematicTargetRotation>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiPlayerControllerInput : public ImGuiBaseComponent
{
public:
    ImGuiPlayerControllerInput()
        : ImGuiBaseComponent("Player controller input")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<PlayerControllerInput>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiPlayerController : public ImGuiBaseComponent
{
public:
    ImGuiPlayerController()
        : ImGuiBaseComponent("Player controller")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<PlayerController>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiPlayerAnimInfo : public ImGuiBaseComponent
{
public:
    ImGuiPlayerAnimInfo()
        : ImGuiBaseComponent("Player anim info")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<PlayerAnimInfo>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiPlayerAI : public ImGuiBaseComponent
{
public:
    ImGuiPlayerAI()
        : ImGuiBaseComponent("Player AI")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<PlayerAI>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiTrackedTarget : public ImGuiBaseComponent
{
public:
    ImGuiTrackedTarget()
        : ImGuiBaseComponent("Tracked target")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<TrackedTarget>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiOneWayPass : public ImGuiBaseComponent
{
public:
    ImGuiOneWayPass()
        : ImGuiBaseComponent("OneWay pass")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<OneWayPass>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiPlayerAffiliation : public ImGuiBaseComponent
{
public:
    ImGuiPlayerAffiliation()
        : ImGuiBaseComponent("Player affiliation")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<PlayerAffiliation>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiVisualID : public ImGuiBaseComponent
{
public:
    ImGuiVisualID()
        : ImGuiBaseComponent("Visual ID")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<VisualID>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiCameraFollow : public ImGuiBaseComponent
{
public:
    ImGuiCameraFollow()
        : ImGuiBaseComponent("Camera follow")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<CameraFollow>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};
