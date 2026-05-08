/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/tracked_target.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"
#include "ecs/player/player_components.h"

TrackedTarget::TrackedTarget()
    : entity(entt::null)
    , position(b2Vec2_zero)
    , isEnabled(false)
    , isGrounded(false)
    , ejectionScore(0.f)
{
}

void TrackedTargetSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<TrackedTarget>();
    for (auto [entity, target] : view.each())
    {
        if (m_registry.valid(target.entity) &&
            m_registry.all_of<Rigidbody, PlayerAffiliation, GroundContact, Damageable>(target.entity))
        {
            auto [rigidbody, affiliation, ground, damageable] =
                m_registry.get<Rigidbody, PlayerAffiliation, GroundContact, Damageable>(target.entity);
            target.position = RigidbodyUtils::GetPosition(rigidbody);
            target.isEnabled = false;
            target.isGrounded = ground.isGrounded;
            target.ejectionScore = damageable.ejectionScore;
            target.isEnabled = g_gameCommon.IsPlayerEnabled(affiliation.playerID);
        }
        else
        {
            target.entity = entt::null;
            target.position = b2Vec2_zero;
            target.isEnabled = false;
            target.isGrounded = false;
            target.ejectionScore = 0.f;
        }
    }
}

void TargetUtils::SearchTarget(
    TrackedTarget &target, entt::registry &registry,
    uint64_t teamMask, b2Vec2 position)
{
    float minDist = FLT_MAX;
    auto playerView = registry.view<
        const PlayerController,
        const PlayerAffiliation,
        const Rigidbody
    >();
    for (auto [entity, controller, affiliation, rigidbody] : playerView.each())
    {
        const int playerID = affiliation.playerID;
        if (g_gameCommon.IsPlayerEnabled(playerID) == false) continue;
        if ((g_gameCommon.GetTeamMask(playerID) & teamMask) == 0) continue;
        if (b2Body_IsValid(rigidbody.bodyId) == false) continue;

        b2Vec2 playerPosition = b2Body_GetPosition(rigidbody.bodyId);
        float dist = b2DistanceSquared(position, playerPosition);
        if (dist < minDist)
        {
            target.entity = entity;
            minDist = dist;
        }
    }
}
