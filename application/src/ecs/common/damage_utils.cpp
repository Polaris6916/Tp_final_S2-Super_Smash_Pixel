/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/damage_utils.h"

bool DamageUtils::AttackCircle(
    Scene *scene,
    entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
    const Damage &damage, const QueryFilter &filter,
    b2Vec2 center, float radius)
{
    entt::registry &registry = scene->GetRegistry();
    bool hitSomething = false;

    std::vector<OverlapResult> result;
    scene->OverlapCircleGizmos(center, radius, filter, result);
    for (OverlapResult &overlap : result)
    {
        entt::entity entity = overlap.entity;
        if (registry.all_of<Damageable>(entity))
        {
            Damageable &damageable = registry.get<Damageable>(entity);
            CumuleDamage(damageable, damage, damagerEntity, damagerAffiliation);
            hitSomething = true;
        }
    }
    return hitSomething;
}

bool DamageUtils::AttackPolygon(
    Scene *scene,
    entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
    const Damage &damage, const QueryFilter &filter,
    const b2Vec2 *vertices, int vertexCount)
{
    entt::registry &registry = scene->GetRegistry();
    bool hitSomething = false;

    std::vector<OverlapResult> result;
    scene->OverlapPolygonGizmos(vertices, vertexCount, filter, result);
    for (OverlapResult &overlap : result)
    {
        entt::entity entity = overlap.entity;
        if (registry.all_of<Damageable>(entity))
        {
            Damageable &damageable = registry.get<Damageable>(entity);
            CumuleDamage(damageable, damage, damagerEntity, damagerAffiliation);
            hitSomething = true;
        }
    }
    return hitSomething;
}

bool DamageUtils::AttackBox(
    Scene *scene,
    entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
    const Damage &damage, const QueryFilter &filter,
    b2Vec2 center, float hx, float hy)
{
    entt::registry &registry = scene->GetRegistry();
    bool hitSomething = false;

    std::vector<OverlapResult> result;
    b2AABB aabb = { 0 };
    aabb.lowerBound = { center.x - hx, center.y - hy };
    aabb.upperBound = { center.x + hx, center.y + hy };
    scene->OverlapAABBGizmos(aabb, filter, result);
    for (OverlapResult &overlap : result)
    {
        entt::entity entity = overlap.entity;
        if (registry.all_of<Damageable>(entity))
        {
            Damageable &damageable = registry.get<Damageable>(entity);
            CumuleDamage(damageable, damage, damagerEntity, damagerAffiliation);
            hitSomething = true;
        }
    }
    return hitSomething;
}

void DamageUtils::CumuleDamage(
    Damageable &damageable, const Damage &damage,
    entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation)
{
    damageable.cumulativeDamage.amount += damage.amount;
    damageable.cumulativeDamage.lockTime = fmaxf(
        damageable.cumulativeDamage.lockTime,
        damage.lockTime
    );
    damageable.cumulativeDamage.lockAttackTime = fmaxf(
        damageable.cumulativeDamage.lockAttackTime,
        damage.lockAttackTime
    );
    damageable.cumulativeDamage.attackCenter = damage.attackCenter;

    if ((damage.ejectionType == Damage::Type::CENTER || damage.ejectionType == Damage::Type::FIXED_DIRECTION)
        && damage.ejectionSpeed > damageable.cumulativeDamage.ejectionSpeed)
    {
        damageable.cumulativeDamage.direction = damage.direction;
        damageable.cumulativeDamage.ejectionType = damage.ejectionType;
        damageable.cumulativeDamage.ejectionSpeed = damage.ejectionSpeed;
    }

    damageable.lastDamagerEntity = damagerEntity;
    damageable.lastDamagerAffiliation = damagerAffiliation;
}

void DamageUtils::ResetCumulativeDamage(Damageable &damageable)
{
    damageable.cumulativeDamage = Damage();
}

void DamageUtils::ResetLastDamager(Damageable &damageable)
{
    damageable.lastDamagerEntity = entt::null;
    damageable.lastDamagerAffiliation = PlayerAffiliation(-1);
}

void DamageUtils::ResetDamageable(Damageable &damageable)
{
    damageable.ejectionScore = 0.f;
    damageable.lockAttackTime = -1.f;
    damageable.lockTime = -1.f;
    damageable.cumulativeDamage = Damage();
    damageable.lastDamagerEntity = entt::null;
    damageable.lastDamagerAffiliation = PlayerAffiliation(-1);
}

b2Vec2 DamageUtils::GetEjection(const Damageable &damageable, b2Vec2 targetCenter)
{
    const float coeff = (1.f + 0.01f * damageable.ejectionScore);
    const float length = coeff * damageable.cumulativeDamage.ejectionSpeed;
    switch (damageable.cumulativeDamage.ejectionType)
    {
    case Damage::Type::FIXED_DIRECTION:
        return length * damageable.cumulativeDamage.direction;

    case Damage::Type::CENTER:
    {
        b2Vec2 ejection = targetCenter - damageable.cumulativeDamage.attackCenter;
        if (b2LengthSquared(ejection) < 0.1f)
        {
            return length * b2Vec2(0.f, 1.f);
        }
        return length * b2Normalize(ejection);
    }
    default: return b2Vec2_zero;
    }
}

PlayerAffiliation::PlayerAffiliation(int playerIDIn)
    : playerID(playerIDIn)
{
}
