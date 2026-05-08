#include "ecs/player/leaf_ranger_system.h"
#include "ecs/player/leaf_ranger_arrow.h"
#include "ecs/player/player_controller_system.h"
#include "ecs/player/player_utils.h"
#include "ecs/player/player_components.h"


void LeafRangerSystem::OnFixedUpdate(EntityCommandBuffer& ecb)
{
    auto viewAnim = m_registry.view<
        const LeafRangerTag,
        const PlayerAffiliation,
        PlayerController,
        SpriteAnimState>();

    for (auto [entity, affiliation, controller, anim] : viewAnim.each())
    {
        if (controller.isStateUpdated == false) continue;

        PlayerConfig* config = g_gameCommon.GetPlayerConfig(affiliation.playerID);
        if (config == nullptr) continue;

        AnimCategory animCat = config->category;
        AnimType type = AnimType::UNDEFINED;
        switch (controller.currState)
        {
        case PlayerState::IDLE: type = AnimType::IDLE; break;
        case PlayerState::RUN: type = AnimType::RUN; break;
        case PlayerState::JUMP: type = AnimType::JUMP_UP; break;
        case PlayerState::FALL: type = AnimType::JUMP_TOP; break;
        case PlayerState::DEFEND: type = AnimType::DEFEND_START; break;
        case PlayerState::ROLL: type = AnimType::ROLL; break;
        case PlayerState::LAUNCHED: type = AnimType::JUMP_UP; break;
        case PlayerState::TAKE_DAMAGE: type = AnimType::TAKE_HIT; break;
        case PlayerState::ATTACK_COMBO: type = AnimType::ATTACK_1; break;
        case PlayerState::SMASH_HOLD: type = AnimType::SMASH_START; break;
        case PlayerState::SMASH_RELEASE: type = AnimType::SMASH_RELEASE; break;
        case PlayerState::ATTACK_SPECIAL: type = AnimType::ATTACK_3; break;
        case PlayerState::ATTACK_AIR: type = AnimType::ATTACK_AIR; break;

        default: break;
        }
        printf("AnimType = %u\n", static_cast<uint32_t>(type));

        if (type != AnimType::UNDEFINED)
        {
            SpriteAnimUtils::SetAnimation(anim, AnimID_Make(config->category, type));
        }
    }
}

void LeafRangerSystem::OnUpdate(EntityCommandBuffer& ecb)
{
    auto viewAnim = m_registry.view<
        const LeafRangerTag,
        const PlayerAffiliation,
        const PlayerControllerInput,
        const Transform,
        SpriteAnimState,
        const PlayerController,
        PlayerAnimInfo
    >();
    for (auto [entity, affiliation, input, transform, spriteAnim, controller, animInfo] : viewAnim.each())
    {
        assert(spriteAnim.saveEvents);
        for (int i = 0; i < spriteAnim.eventCount; i++)
        {
            SpriteAnimEvent& animEvent = spriteAnim.events[i];

            if (animEvent.type == SpriteAnimEvent::Type::CYCLE_END)
            {
                OnAnimCycleEnd(entity, animEvent, spriteAnim, affiliation, input, animInfo);
            }
            else if (animEvent.type == SpriteAnimEvent::Type::FRAME_CHANGED)
            {
                OnAnimFrameChanged(entity, ecb, animEvent, transform, affiliation, controller, animInfo, input);
            }
        }
    }
}

void LeafRangerSystem::OnAnimFrameChanged(
    entt::entity entity,
    EntityCommandBuffer& ecb,
    const SpriteAnimEvent& animEvent,
    const Transform& transform,
    const PlayerAffiliation& affiliation,
    const PlayerController& controller,
    PlayerAnimInfo& animInfo,
    const PlayerControllerInput& input)
{
    const int playerID = affiliation.playerID;
    PlayerConfig* config = g_gameCommon.GetPlayerConfig(playerID);
    if (config == nullptr) return;

    QueryFilter filter;
    filter.categoryBits = CATEGORY_ATTACK;
    filter.maskBits = config->otherTeamMask | CATEGORY_ITEM;
    filter.solidOnly = true;

    const float lockTime = 0.2f;
    float s = controller.facingRight ? 1.f : -1.f;
    AnimType animType = AnimID_GetType(animEvent.id);

    if (animType == AnimType::ATTACK_1)
    {
        animInfo.autoVelocity = 0.0f;

        if (animEvent.index == 1)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_SWORD_ATTACK_3, SFXIntensity::NORMAL);
        }
        if (animEvent.index == 4)
        {

            b2Vec2 position = transform.position;

            Damage damage;
            damage.attackCenter = position;
            damage.amount = 5.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 10.5f * PLAYER_ATTACK_FRAME_TIME;

            const b2Vec2 center = transform.position + b2Vec2{ s * 1.30f, 1.40f };
            const float radius = 2.5f;
            bool hit = DamageUtils::AttackBox(m_scene, entity, affiliation, damage, filter, center, 0.6f, 0.1f);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A1, SFXIntensity::NORMAL, hit);
        }
    }
    else if (animType == AnimType::ATTACK_2)
    {

        if (animEvent.index == 0)
        {
            animInfo.autoVelocity = 0.0f * s;
        }

        const b2Vec2 position = transform.position;
        Damage damage;
        damage.attackCenter = transform.position;
        damage.amount = 3.f;
        damage.ejectionType = Damage::Type::NO_EJECTION;
        damage.lockTime = lockTime;
        damage.lockAttackTime = 5.5f * PLAYER_ATTACK_FRAME_TIME;

        if (animEvent.index == 1)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_SWORD_ATTACK_2, SFXIntensity::NORMAL);
        }
        else if (animEvent.index == 10)
        {
            b2Vec2 position = transform.position + b2Vec2(s * 10.0f, 3.f);
            LeafRangerSystem::AddArrow(ecb, position, affiliation, controller, s, AnimType::ARROW_2_FIN);
        }
    }
    else if (animType == AnimType::ATTACK_3)
    {
        animInfo.autoVelocity = 0.f;

        if (animEvent.index == 2)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_FIRE_2, SFXIntensity::STRONG);
        }
        else if (animEvent.index == 9)
        {
            b2Vec2 position = transform.position + b2Vec2(s * 2.0f,1.2f);
            LeafRangerSystem::AddArrow(ecb, position , affiliation, controller, s, AnimType::ARROW_3);
        }
    }
    else if (animType == AnimType::ATTACK_AIR)
    {
        animInfo.autoVelocity = 0.0f;

        if (animEvent.index == 1)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_SWORD_ATTACK_3, SFXIntensity::NORMAL);
        }
        if (animEvent.index == 5)
        {
            animInfo.autoVelocity = 0.f;
            b2Vec2 position = transform.position + b2Vec2(s * 0.8f, 0.8f);
            LeafRangerSystem::AddArrow(ecb, position, affiliation, controller, s, AnimType::ARROW_AIR);
        }
    }
    else if (animType == AnimType::SMASH_RELEASE)
    {
        switch (animEvent.index)
        {
        case 0:  animInfo.autoVelocity = s * +0.0f; break;
        case 2:  animInfo.autoVelocity = s * +9.0f; break;
        case 3:  animInfo.autoVelocity = s * 14.0f; break;
        case 4:  animInfo.autoVelocity = s * +8.0f; break;
        case 5:  animInfo.autoVelocity = s * +0.0f; break;
        case 8:  animInfo.autoVelocity = s * -9.0f; break;
        case 10: animInfo.autoVelocity = s * +0.0f; break;
        default: break;
        }

        if (animEvent.index == 2)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_FIRE_2, SFXIntensity::MAX);
        }
        else if (animEvent.index == 1)
        {
            animInfo.autoVelocity = 0.f;
            b2Vec2 position = transform.position + b2Vec2(s * 7.0f, 1.0f);
            LeafRangerSystem::AddArrow(ecb, position, affiliation, controller, s, AnimType::ARROW_SMASH);
        }
    }
    else if (animType == AnimType::RUN)
    {
        switch (animEvent.index)
        {
        case 1:
            PlayerUtils::EmitSmallDust(m_scene, transform.position, controller.facingRight, s * 0.6f);
            break;
        case 5:
            PlayerUtils::EmitSmallDust(m_scene, transform.position, controller.facingRight, s * 0.1f);
            break;
        default: break;
        }
    }
    else if (animType == AnimType::ROLL)
    {
        switch (animEvent.index)
        {
        case 2:
            PlayerUtils::EmitRollDust(m_scene, transform.position, controller.facingRight);
            if(input.direction >= 0) animInfo.autoVelocity = 15.0f;
            else animInfo.autoVelocity = -15.0f;
            break;
        default: break;
        }
    }
}

void LeafRangerSystem::OnAnimCycleEnd(
    entt::entity entity,
    const SpriteAnimEvent& animEvent,
    SpriteAnimState& anim,
    const PlayerAffiliation& affiliation,
    const PlayerControllerInput& input,
    PlayerAnimInfo& event)
{
    PlayerConfig* config = g_gameCommon.GetPlayerConfig(affiliation.playerID);
    if (config == nullptr) return;

    AnimType nextAnimType = AnimType::UNDEFINED;
    switch (AnimID_GetType(animEvent.id))
    {
    case AnimType::DEFEND_START:
    {
        nextAnimType = AnimType::DEFEND;
        break;
    }
    case AnimType::JUMP_TOP:
    {
        nextAnimType = AnimType::JUMP_DOWN;
        break;
    }
    case AnimType::ATTACK_1:
    {

        if (input.attackDown)
        {
            nextAnimType = AnimType::ATTACK_2;
        }
        else
        {
            nextAnimType = AnimType::ATTACK_1_END;
        }
        break;
    }
    case AnimType::ATTACK_2:
    {
        if (input.attackDown)
        {
            nextAnimType = AnimType::ATTACK_3;
        }
        else
        {
            nextAnimType = AnimType::ATTACK_2_END;
        }
        break;
    }
    case AnimType::ATTACK_1_END:
    case AnimType::ATTACK_2_END:
    case AnimType::ATTACK_3:
    {
        event.type = PlayerAnimInfo::Event::COMBO_END;
        break;
    }
    case AnimType::TAKE_HIT:
    {
        event.type = PlayerAnimInfo::Event::TAKE_HIT_END;
        break;
    }

    case AnimType::SMASH_START:
    {
        nextAnimType = AnimType::SMASH_HOLD;
        break;
    }
    case AnimType::SMASH_RELEASE:
    {
        event.type = PlayerAnimInfo::Event::SMASH_END;
        break;
    }
    case AnimType::ROLL:
    {
        event.type = PlayerAnimInfo::Event::ROLL_END;
        break;
    }
    case AnimType::ATTACK_AIR:
    {
        nextAnimType = AnimType::ATTACK_1_END;
        break;

    }
    default: break;
    }

    if (nextAnimType != AnimType::UNDEFINED)
    {
        SpriteAnimUtils::SetAnimation(anim, AnimID_Make(config->category, nextAnimType));
    }
}

void LeafRangerSystem::AddArrow(EntityCommandBuffer& ecb, b2Vec2 position, const PlayerAffiliation& affiliation, const PlayerController& controller, float s, AnimType anim)
{
    entt::registry& registry = m_scene->GetRegistry();

    b2Vec2 velocity = { s * 0.0f, 0.f };
    if (anim == AnimType::ARROW_3 || anim == AnimType::ARROW_2_DEBUT)
    {
        velocity = { s * 20.0f, 0.f };
    }
    else if (anim == AnimType::ARROW_AIR)
    {
        velocity = { s * 20.0f, -20.f };
    }
    ecb.AddCommand(
        std::make_unique<ArrowCommand>(
            ecb, m_scene, affiliation, controller, position, velocity, s, anim
        )
    );
};