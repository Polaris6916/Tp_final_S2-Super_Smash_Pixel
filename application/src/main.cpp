/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/


#include "common/game_settings.h"
#include "common/game_common.h"

#include "scene_manager/stage_manager.h"
#include "scene_manager/title_manager.h"

//#define FULLSCREEN
//#define WINDOW_FHD
//#define SKIP_MENU

#ifdef WINDOW_FHD
#define WINDOW_WIDTH   FHD_WIDTH
#define WINDOW_HEIGHT  FHD_HEIGHT
#else
#define WINDOW_WIDTH   HD_WIDTH
#define WINDOW_HEIGHT  HD_HEIGHT
#endif
#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT

enum class GameState
{
    MAIN_MENU, STAGE
};

int main(int argc, char *argv[])
{
    // Initialisation de la SDL
    const Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO;
    const Uint32 mixFlags = 0;
    game::Init(sdlFlags, mixFlags);

    game::SetMusicVolume(0.8f);
    game::SetFXChannelsVolume(0.5f);

    // Crée la fenêtre et le moteur de rendu
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
#ifdef FULLSCREEN
    windowFlags |= SDL_WINDOW_FULLSCREEN;
#endif
    game::CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Super Pixel Smash", windowFlags);
    game::CreateRenderer(LOGICAL_WIDTH, LOGICAL_HEIGHT);

    srand((unsigned int)time(nullptr));

    // Input manager
    SceneManager *sceneManger = nullptr;
    bool quitGame = false;
    GameState state = GameState::MAIN_MENU;

    InputManager *inputManager = new InputManager();
    InitInputConfig(inputManager);
    ApplicationInput *appInput = ApplicationInput::GetFromManager(inputManager);

    inputManager->SetKeyboardIsPlayer(false);

    // Config du jeu par défaut

    g_gameCommon.playerCount = 2;

    PlayerConfig *playerConfig = nullptr;
    playerConfig = g_gameCommon.GetPlayerConfig(0);
    playerConfig->type = PlayerType::FIRE_KNIGHT;
    playerConfig->teamID = 0;
    playerConfig->playerID = 0;
    playerConfig->skinID = 0;
    playerConfig->isCPU = false;

    playerConfig = g_gameCommon.GetPlayerConfig(1);
    playerConfig->type = PlayerType::WATER_PRIESTESS;
    playerConfig->teamID = 1;
    playerConfig->playerID = 1;
    playerConfig->skinID = 1;
    playerConfig->isCPU = true;


    g_gameCommon.stageConfig.mode = StageConfig::Mode::LIMITED_TIME;
    g_gameCommon.stageConfig.lifeCount = 2;
    g_gameCommon.stageConfig.duration = 3;

    g_gameCommon.UpdatePlayerConfigs();

#ifdef SKIP_MENU
    state = GameState::STAGE;
#endif

    // Boucle de jeu
    while (quitGame == false)
    {
        // Construction de la scène
        switch (state)
        {
        case GameState::STAGE:
            sceneManger = new StageManager(inputManager);
            break;

        case GameState::MAIN_MENU:
        default:
            sceneManger = new TitleManager(inputManager);
            break;
        }

        // Boucle de rendu
        while (true)
        {
            g_time->Update();

            // Met à jour la scène
            Scene *scene = sceneManger->GetScene();
            scene->Update();

            if (appInput->quitPressed)
            {
                quitGame = true;
                break;
            }

            if (sceneManger->ShouldQuitScene())
                break;

            // Efface le rendu précédent
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            // Dessine la scène
            scene->Render();

            // Affiche le nouveau rendu
            SDL_RenderPresent(g_renderer);
        }

        if (sceneManger->ShouldQuitGame())
        {
            quitGame = true;
        }

        switch (state)
        {
        case GameState::STAGE:
            state = GameState::MAIN_MENU;
            break;

        case GameState::MAIN_MENU:
        default:
            state = GameState::STAGE;
            break;
        }

        if (sceneManger)
        {
            delete sceneManger;
            sceneManger = nullptr;
        }
    }

    delete inputManager; inputManager = nullptr;
    game::DestroyRenderer();
    game::DestroyWindow();
    game::Quit();

    return EXIT_SUCCESS;
}
