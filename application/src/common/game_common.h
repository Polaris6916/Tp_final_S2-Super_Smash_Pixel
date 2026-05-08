/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_assets.h"

#include "input/application_input.h"
#include "input/mouse_input.h"
#include "input/controls_input.h"

#define MAX_PLAYER_COUNT 4

extern const float PLAYER_ATTACK_FRAME_TIME;

enum class InputID : int
{
    APPLICATION, MOUSE, CONTROLS
};

void InitInputConfig(InputManager *inputManager);

enum class PlayerType : int
{
    FIRE_KNIGHT, WATER_PRIESTESS, LEAF_RANGER, METAL_BLADEKEEPER
};

class PlayerConfig
{
public:
    PlayerConfig();

    void Update();

    bool isCPU;
    bool enabled;
    bool isKO;
    int playerID;
    PlayerType type;
    AnimCategory category;
    int skinID;
    int level;

    int teamID;
    uint64_t teamMask;
    uint64_t otherTeamMask;
};

class PlayerStats
{
public:
    PlayerStats();

    int fallCount;
    int koCount;
    float damageGiven;
    float damageTaken;
    float maxSpeed;
    float koTime;
    float ejectionScore;
};

class StageConfig
{
public:
    StageConfig();

    enum class Type : int
    {
        STAR_FIELDS = 0, ROCKY_PASS,
    };
    enum class Mode : int
    {
        LIMITED_LIVES = 0, LIMITED_TIME
    };
    enum class Frequency : int
    {
        NEVER = 0, RARELY, SOMETIMES, OFTEN
    };

    Type type;
    Mode mode;

    Frequency bombsFrequency;
    Frequency potionFrequency;
    Frequency crystalFrequency;

    int duration;
    int lifeCount;
};

class GameCommon
{
public:
    GameCommon();

    std::array<PlayerConfig, MAX_PLAYER_COUNT> playerConfigs;
    std::array<PlayerStats, MAX_PLAYER_COUNT> playerStats;
    StageConfig stageConfig;
    int playerCount;

    PlayerConfig *GetPlayerConfig(int playerID);
    const PlayerConfig *GetPlayerConfig(int playerID) const;
    PlayerStats *GetPlayerStats(int playerID);
    bool IsPlayerEnabled(int playerID) const;
    uint64_t GetTeamMask(int playerID) const;
    uint64_t GetOtherTeamMask(int playerID) const;

    void UpdatePlayerConfigs();
    void ResetPlayerStats();
};

extern GameCommon g_gameCommon;

namespace game
{
    float GetUIPixelScale(Scene *scene);
}
