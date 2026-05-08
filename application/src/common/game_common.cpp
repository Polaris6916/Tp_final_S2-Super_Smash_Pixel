/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/game_common.h"

const float PLAYER_ATTACK_FRAME_TIME = 0.045f;

GameCommon g_gameCommon{};

PlayerConfig::PlayerConfig()
    : type(PlayerType::FIRE_KNIGHT)
    , skinID(0)
    , playerID(-1)
    , teamID(0)
    , enabled(true)
    , isCPU(false)
    , isKO(false)
    , teamMask(CATEGORY_TEAM_0)
    , otherTeamMask(CATEGORY_ALL_TEAMS & (~CATEGORY_TEAM_0))
    , category(AnimCategory::FIRE_KNIGHT_0)
    , level(0)
{
}

void PlayerConfig::Update()
{
    switch (teamID)
    {
    default:
    case 0: teamMask = CATEGORY_TEAM_0; break;
    case 1: teamMask = CATEGORY_TEAM_1; break;
    case 2: teamMask = CATEGORY_TEAM_2; break;
    case 3: teamMask = CATEGORY_TEAM_3; break;
    }
    otherTeamMask = CATEGORY_ALL_TEAMS & (~teamMask);

    switch (type)
    {
    case PlayerType::METAL_BLADEKEEPER:
    {
        switch (skinID)
        {
        default:
        case 0: category = AnimCategory::METAL_BLADEKEEPER_0; break;
        case 1: category = AnimCategory::METAL_BLADEKEEPER_1; break;
        }
        break;
    }
    case PlayerType::FIRE_KNIGHT:
    {
        switch (skinID)
        {
        default:
        case 0: category = AnimCategory::FIRE_KNIGHT_0; break;
        case 1: category = AnimCategory::FIRE_KNIGHT_1; break;
        }
        break;
    }
    case PlayerType::WATER_PRIESTESS:
    {
        switch (skinID)
        {
        default:
        case 0: category = AnimCategory::WATER_PRIESTESS_0; break;
        case 1: category = AnimCategory::WATER_PRIESTESS_1; break;
        }
        break;
    }
    case PlayerType::LEAF_RANGER:
    {
        switch (skinID)
        {
        default:
        case 0: category = AnimCategory::LEAF_RANGER_0; break;
        case 1: category = AnimCategory::LEAF_RANGER_1; break;
        }
        break;
    }
    default:
        break;
}

}

StageConfig::StageConfig()
    : type(Type::STAR_FIELDS)
    , mode(Mode::LIMITED_TIME)
    , duration(3)
    , lifeCount(3)
    , bombsFrequency(Frequency::SOMETIMES) //Changer la config de base
    , potionFrequency(Frequency::SOMETIMES)
    , crystalFrequency(Frequency::SOMETIMES)
{
}

PlayerStats::PlayerStats()
    : fallCount(0)
    , koCount(0)
    , damageGiven(0.f)
    , damageTaken(0.f)
    , maxSpeed(0.f)
    , koTime(0.f)
    , ejectionScore(0.f)
{
}

GameCommon::GameCommon()
    : playerConfigs{}
    , playerStats{}
    , stageConfig{}
    , playerCount(1)
{
}

PlayerConfig *GameCommon::GetPlayerConfig(int playerID)
{
    assert(playerCount <= MAX_PLAYER_COUNT);
    if (playerID < 0 || playerID >= playerCount) return nullptr;
    return &(playerConfigs[playerID]);
}

const PlayerConfig *GameCommon::GetPlayerConfig(int playerID) const
{
    assert(playerCount <= MAX_PLAYER_COUNT);
    if (playerID < 0 || playerID >= playerCount) return nullptr;
    return &(playerConfigs[playerID]);
}

PlayerStats *GameCommon::GetPlayerStats(int playerID)
{
    assert(playerCount <= MAX_PLAYER_COUNT);
    if (playerID < 0 || playerID >= playerCount) return nullptr;
    return &(playerStats[playerID]);
}

bool GameCommon::IsPlayerEnabled(int playerID) const
{
    if (playerID < 0 || playerID >= playerCount) return false;
    return playerConfigs[playerID].enabled;
}

uint64_t GameCommon::GetTeamMask(int playerID) const
{
    if (playerID < 0 || playerID >= playerCount) return 0;
    return playerConfigs[playerID].teamMask;
}

uint64_t GameCommon::GetOtherTeamMask(int playerID) const
{
    if (playerID < 0 || playerID >= playerCount) return CATEGORY_ALL_TEAMS;
    return playerConfigs[playerID].otherTeamMask;
}

void GameCommon::UpdatePlayerConfigs()
{
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        playerConfigs[i].Update();
    }
}

void GameCommon::ResetPlayerStats()
{
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        playerStats[i] = PlayerStats();
    }
}

void InitInputConfig(InputManager *inputManager)
{
    ApplicationInput *applicationInput = new ApplicationInput();
    MouseInput *mouseInput = new MouseInput();
    ControlsInput *controlsInput = new ControlsInput();

    inputManager->AddInputGroup(
        (int)InputID::APPLICATION, applicationInput
    );
    inputManager->AddInputGroup(
        (int)InputID::MOUSE, mouseInput
    );
    inputManager->AddInputGroup(
        (int)InputID::CONTROLS, controlsInput
    );
}

float game::GetUIPixelScale(Scene *scene)
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);
    return fmaxf(1.f, roundf(4.f * viewport.w / 1920.0f));
}
