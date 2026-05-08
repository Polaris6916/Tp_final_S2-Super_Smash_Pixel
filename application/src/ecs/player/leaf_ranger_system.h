#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"

class LeafRangerSystem : public System
{
public:
    LeafRangerSystem(Scene* scene) : System(scene, "Leaf Ranger System") {}

    virtual void OnFixedUpdate(EntityCommandBuffer& ecb) override;
    virtual void OnUpdate(EntityCommandBuffer& ecb) override;
    void AddArrow(EntityCommandBuffer& ecb, b2Vec2 position, const PlayerAffiliation& affiliation, const PlayerController& controller, float s, AnimType anim);

protected:
    
    void OnAnimFrameChanged(
        entt::entity entity,
        EntityCommandBuffer& ecb,
        const SpriteAnimEvent& animEvent,
        const Transform& transform,
        const PlayerAffiliation& affiliation,
        const PlayerController& controller,
        PlayerAnimInfo& animInfo,
        const PlayerControllerInput& input

    );

    void OnAnimCycleEnd(
        entt::entity entity,
        const SpriteAnimEvent& animEvent,
        SpriteAnimState& anim,
        const PlayerAffiliation& affiliation,
        const PlayerControllerInput& input,
        PlayerAnimInfo& animInfo);
};
