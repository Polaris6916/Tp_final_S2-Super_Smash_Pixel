/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "input/controls_input.h"
#include "common/game_common.h"

ControlsInput::ControlsInput()
    : InputGroup()
    , players()
{
}

void ControlsInput::OnPreEventProcess()
{
    for (PlayerInput &player : players)
    {
        player.jumpPressed = false;
        player.attackPressed = false;
        player.specialPressed = false;
        player.smashPressed = false;
    }
}

void ControlsInput::OnEventProcess(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        OnControllerButtonDown(event.gbutton.which, event.gbutton.button);
        break;

    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        OnControllerButtonUp(event.gbutton.which, event.gbutton.button);
        break;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        OnControllerAxisMotion(event.gbutton.which,  event.gaxis.axis, event.gaxis.value);
        break;

    case SDL_EVENT_KEY_DOWN:
        OnKeyDown(event.key.repeat, event.key.scancode);
        break;

    case SDL_EVENT_KEY_UP:
        OnKeyUp(event.key.repeat, event.key.scancode);
        break;

    default: break;
    }
}

void ControlsInput::Reset()
{
    for (PlayerInput &player : players)
    {
        player.axisX = 0.f;
        player.jumpDown = false;
        player.jumpPressed = false;
        player.oneWayPassDown = false;
        player.defendDown = false;
    }
}

ControlsInput *ControlsInput::GetFromManager(InputManager *inputManager)
{
    ControlsInput *inputGroup = dynamic_cast<ControlsInput *>(
        inputManager->GetInputGroup((int)InputID::CONTROLS));
    AssertNew(inputGroup);

    return inputGroup;
}

void ControlsInput::OnControllerButtonDown(SDL_JoystickID which, int button)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
    case SDL_GAMEPAD_BUTTON_NORTH:
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        players[playerID].jumpPressed = true;
        players[playerID].jumpDown = true;
        break;

    case SDL_GAMEPAD_BUTTON_WEST:
        players[playerID].attackPressed = true;
        players[playerID].attackDown = true;
        break;

    case SDL_GAMEPAD_BUTTON_EAST:
        players[playerID].specialPressed = true;
        players[playerID].specialDown = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        players[playerID].oneWayPassDown = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        players[playerID].axisX = -1.f;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        players[playerID].axisX = 1.f;
        break;

    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
        players[playerID].defendDown = true;
        break;

    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
        players[playerID].smashPressed = true;
        players[playerID].smashDown = true;
        break;

    default: break;
    }
}

void ControlsInput::OnControllerButtonUp(SDL_JoystickID which, int button)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
    case SDL_GAMEPAD_BUTTON_NORTH:
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        players[playerID].jumpDown = false;
        break;

    case SDL_GAMEPAD_BUTTON_WEST:
        players[playerID].attackDown = false;
        break;

    case SDL_GAMEPAD_BUTTON_EAST:
        players[playerID].specialDown = false;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        players[playerID].oneWayPassDown = false;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        players[playerID].axisX = 0.f;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        players[playerID].axisX = 0.f;
        break;

    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
        players[playerID].defendDown = false;
        break;

    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
        players[playerID].smashDown = false;
        break;

    default: break;
    }
}

void ControlsInput::OnControllerAxisMotion(SDL_JoystickID which, int axis, Sint16 value)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (axis)
    {
    case SDL_GAMEPAD_AXIS_LEFTX:
        players[playerID].SetAxisLeftX(value);
        break;

    case SDL_GAMEPAD_AXIS_LEFTY:
        players[playerID].SetAxisLeftY(value);
        break;

    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        players[playerID].SetTriggerLeft(value);
        break;

    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
        players[playerID].SetTriggerRight(value);
        break;

    default: break;
    }
}

void ControlsInput::OnKeyDown(bool repeat, SDL_Scancode scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_RIGHT:
        // Deplacement à droite
        players[0].axisX = 1.f;
        break;

    case SDL_SCANCODE_LEFT:
        // Deplacement à gauche
        players[0].axisX = -1.f;
        break;

    case SDL_SCANCODE_DOWN:
        // Descente d'une plateforme
        players[0].oneWayPassDown = true;
        break;

    case SDL_SCANCODE_X:
        // Attaque
        players[0].attackPressed = true;
        players[0].attackDown = true;
        break;

    case SDL_SCANCODE_Z:
        // Défence
        players[0].defendDown = true;
        break;

    case SDL_SCANCODE_C:
        // Défence
        players[0].specialPressed = true;
        players[0].specialDown = true;
        break;

    case SDL_SCANCODE_S:
        // Smash
        players[0].smashPressed = true;
        players[0].smashDown = true;
        break;

    case SDL_SCANCODE_SPACE:
    case SDL_SCANCODE_UP:
        // Saut
        players[0].jumpDown = true;
        players[0].jumpPressed = true;
        break;

    default: break;
    }
}

void ControlsInput::OnKeyUp(bool repeat, SDL_Scancode scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_RIGHT:
        // Deplacement à droite
        if (players[0].axisX > 0.f)
            players[0].axisX = 0.f;
        break;

    case SDL_SCANCODE_LEFT:
        // Deplacement à gauche
        if (players[0].axisX < 0.f)
            players[0].axisX = 0.f;
        break;

    case SDL_SCANCODE_DOWN:
        // Descente d'une plateforme
        players[0].oneWayPassDown = false;
        break;

    case SDL_SCANCODE_SPACE:
    case SDL_SCANCODE_UP:
        // Saut
        players[0].jumpDown = false;
        break;

    case SDL_SCANCODE_Z:
        // Défence
        players[0].defendDown = false;
        break;

    case SDL_SCANCODE_S:
        // Smash
        players[0].smashDown = false;
        break;

    case SDL_SCANCODE_C:
        // Special
        players[0].specialDown = false;
        break;

    case SDL_SCANCODE_X:
        // Attaque
        players[0].attackDown = false;
        break;

    default: break;
    }
}

PlayerInput::PlayerInput()
    : axisX(0.f)
    , axisY(0.f)
    , jumpPressed(false)
    , jumpDown(false)
    , attackPressed(false)
    , attackDown(false)
    , specialPressed(false)
    , specialDown(false)
    , smashPressed(false)
    , smashDown(false)
    , oneWayPassDown(false)
    , defendDown(false)
    , m_deadZone(8000)
{
}

void PlayerInput::SetAxisLeftX(Sint16 value)
{
    if (-m_deadZone <= value && value < m_deadZone)
    {
        value = 0;
    }
    axisX = math::Clamp((float)value / 32767.f, -1.f, 1.f);
}

void PlayerInput::SetAxisLeftY(Sint16 value)
{
    if (-m_deadZone <= value && value < m_deadZone)
    {
        value = 0;
    }
    axisY = -math::Clamp((float)value / 32767.f, -1.f, 1.f);

    oneWayPassDown = (axisY < -0.8f);
}

void PlayerInput::SetTriggerLeft(Sint16 value)
{
    if (value > m_deadZone)
    {
        defendDown = true;
    }
    else
    {
        defendDown = false;
    }
}

void PlayerInput::SetTriggerRight(Sint16 value)
{
    if (value > m_deadZone)
    {
        if (smashDown == false) smashPressed = true;
        smashDown = true;
    }
    else
    {
        smashDown = false;
    }
}
