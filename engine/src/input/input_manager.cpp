/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "input/input_manager.h"
#include "utils/utils.h"

InputManager::InputManager()
    : m_inputMap()
    , m_gamepads()
    , m_keyboardIsPlayer(false)
    , m_playerCount(0)
    , m_maxPlayerCount(4)
{
    int gamepadCount = 0;
    SDL_JoystickID *joysticks = SDL_GetGamepads(&gamepadCount);
    if (joysticks)
    {
        for (int i = 0; i < gamepadCount; i++)
        {
            SDL_Gamepad *gamepad = SDL_OpenGamepad(joysticks[i]);
            m_gamepads.insert(gamepad);
        }
        SDL_free(joysticks);
    }

    ResetPlayerMapping();
}

InputManager::~InputManager()
{
    for (auto pair : m_inputMap)
    {
        delete pair.second;
    }

    for (SDL_Gamepad *gamepad : m_gamepads)
    {
        SDL_CloseGamepad(gamepad);
    }
}

void InputManager::ProcessEvents()
{
    for (auto &pair : m_inputMap)
    {
        InputGroup *group = pair.second;
        if (group->IsEnabled())
        {
            group->OnPreEventProcess();
        }
    }

    SDL_Event event;
    SDL_Gamepad *gamepad = nullptr;
    int playerID = 0;
    while (SDL_PollEvent(&event))
    {
        bool skipEvent = false;

        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_EVENT_GAMEPAD_ADDED:
            AddGameController(event.gdevice.which);
            break;

        case SDL_EVENT_GAMEPAD_REMOVED:
            RemoveGameController(event.gdevice.which);
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
        case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
        case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
        case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
        case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
        case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
        case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
            playerID = SDL_GetGamepadPlayerIndexForID(event.gbutton.which);
            if (playerID < 0 || playerID >= m_maxPlayerCount)
            {
                skipEvent = true;
                break;
            }
            break;

        default:
            break;
        }

        if (skipEvent) continue;


        for (auto &pair : m_inputMap)
        {
            InputGroup *group = pair.second;
            if (group->IsEnabled())
            {
                group->OnEventProcess(event);
            }
        }
    }

    for (auto &pair : m_inputMap)
    {
        InputGroup *group = pair.second;
        if (group->IsEnabled())
        {
            group->OnPostEventProcess();
        }
    }
}

void InputManager::AddGameController(SDL_JoystickID joystickID)
{
    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);

    for (SDL_Gamepad *gamepad : m_gamepads)
    {
        int playerID = SDL_GetGamepadPlayerIndex(gamepad);
        if (playerID < 0 || playerID >= m_maxPlayerCount) continue;

        playerIDs[playerID] = true;

        if (SDL_GetGamepadID(gamepad) == joystickID)
        {
            // Le gamepad existe déjà
            delete[] playerIDs;
            return;
        }
    }

    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (int i = playerID; i < m_maxPlayerCount; i++)
    {
        if (playerIDs[i]) continue;

        playerID = i;
        break;
    }
    delete[] playerIDs; playerIDs = nullptr;

    SDL_Gamepad *gamepad = SDL_OpenGamepad(joystickID);
    SDL_SetGamepadPlayerIndex(gamepad, playerID);
    m_gamepads.insert(gamepad);

    UpdatePlayerCount();
}

void InputManager::RemoveGameController(SDL_JoystickID joystickID)
{
    SDL_Gamepad *gamepad = SDL_GetGamepadFromID(joystickID);
    if (gamepad == nullptr) return;

    m_gamepads.erase(gamepad);
    SDL_CloseGamepad(gamepad);

    UpdatePlayerCount();
}

void InputManager::UpdatePlayerCount()
{
    m_playerCount = 1;

    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);
    playerIDs[0] = true;

    for (SDL_Gamepad *gamepad : m_gamepads)
    {
        int playerID = SDL_GetGamepadPlayerIndex(gamepad);
        if (playerID < 0 || playerID >= m_maxPlayerCount) continue;

        m_playerCount += playerIDs[playerID] ? 0 : 1;
        playerIDs[playerID] = true;
    }
}

void InputManager::ResetPlayerMapping()
{
    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (SDL_Gamepad *gamepad : m_gamepads)
    {
        playerID = math::Clamp(playerID, 0, m_maxPlayerCount - 1);
        SDL_SetGamepadPlayerIndex(gamepad, playerID++);
    }
    UpdatePlayerCount();
}

void InputManager::AddInputGroup(int groupID, InputGroup *inputGroup)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end())
    {
        delete it->second;
        m_inputMap.erase(groupID);
    }

    m_inputMap.insert(std::make_pair(groupID, inputGroup));
}

InputGroup *InputManager::GetInputGroup(int groupID)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end()) return it->second;
    return nullptr;
}

std::set<SDL_Gamepad *> InputManager::GetGameControllers()
{
    return m_gamepads;
}

int InputManager::GetGameControllerCount() const
{
    return (int)m_gamepads.size();
}
