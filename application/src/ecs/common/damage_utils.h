/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

struct PlayerAffiliation
{
    PlayerAffiliation(int playerID = -1);

    int playerID;
};

struct Damage
{
    Damage()
        : amount(0)
        , lockTime(0.f)
        , lockAttackTime(0.f)
        , attackCenter(b2Vec2_zero)
        , direction(b2Vec2_zero)
        , ejectionSpeed(0.f)
        , ejectionType(Type::NO_EJECTION)
    {}

    enum class Type
    {
        NO_EJECTION, FIXED_DIRECTION, CENTER
    };

    float lockTime;
    float lockAttackTime;
    float amount;
    float ejectionSpeed;
    Type ejectionType;
    b2Vec2 attackCenter;
    b2Vec2 direction;
};

struct Damageable
{
    Damageable()
        : lastDamagerEntity(entt::null)
        , lastDamagerAffiliation()
        , cumulativeDamage()
        , ejectionScore(0.f)
        , lockTime(0.f)
        , lockAttackTime(0.f)
    {}

    Damage cumulativeDamage;
    PlayerAffiliation lastDamagerAffiliation;
    entt::entity lastDamagerEntity;
    float ejectionScore;
    float lockTime;
    float lockAttackTime;
};

class DamageUtils
{
public:
    static void CumuleDamage(
        Damageable &damageable, const Damage &damage,
        entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation
    );
    static void ResetCumulativeDamage(Damageable &damageable);
    static void ResetLastDamager(Damageable &damageable);
    static void ResetDamageable(Damageable &damageable);
    static b2Vec2 GetEjection(const Damageable &damageable, b2Vec2 targetCenter);

    static bool AttackCircle(
        Scene *scene,
        entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
        const Damage &damage, const QueryFilter &filter,
        b2Vec2 center, float radius);

    static bool AttackPolygon(
        Scene *scene,
        entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
        const Damage &damage, const QueryFilter &filter,
        const b2Vec2 *vertices, int vertexCount);

    static bool AttackBox(
        Scene *scene,
        entt::entity damagerEntity, const PlayerAffiliation &damagerAffiliation,
        const Damage &damage, const QueryFilter &filter,
        b2Vec2 center, float hx, float hy);

private:
    DamageUtils() = delete;
};
