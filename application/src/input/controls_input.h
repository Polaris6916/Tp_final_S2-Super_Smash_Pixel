/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"

class PlayerInput
{
public:
    PlayerInput();

    float axisX;
    float axisY;
    bool jumpDown;
    bool jumpPressed;
    bool oneWayPassDown;
    bool attackPressed;
    bool attackDown;
    bool defendDown;
    bool smashPressed;
    bool smashDown;
    bool specialPressed;
    bool specialDown;

    void SetAxisLeftX(Sint16 value);
    void SetAxisLeftY(Sint16 value);
    void SetTriggerLeft(Sint16 value);
    void SetTriggerRight(Sint16 value);

private:
    Sint16 m_deadZone;
};

class ControlsInput : public InputGroup
{
public:
    ControlsInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event evt) override;
    virtual void Reset() override;

    std::array<PlayerInput, MAX_PLAYER_COUNT> players;

    static ControlsInput *GetFromManager(InputManager *inputManager);

private:
    void OnControllerButtonDown(SDL_JoystickID which, int button);
    void OnControllerButtonUp(SDL_JoystickID which, int button);
    void OnControllerAxisMotion(SDL_JoystickID which, int axis, Sint16 value);
    void OnKeyDown(bool repeat, SDL_Scancode scancode);
    void OnKeyUp(bool repeat, SDL_Scancode scancode);
};
