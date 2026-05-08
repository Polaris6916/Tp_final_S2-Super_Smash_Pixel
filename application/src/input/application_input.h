/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"

class ApplicationInput : public InputGroup
{
public:
    ApplicationInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event event) override;
    //virtual void OnPostEventProcess() override;
    virtual void Reset() override;

    bool quitPressed;
    bool pausePressed;
    bool debugPressed;
    bool makeStepPressed;
    bool makeStepDown;
    bool realtimePressed;

    std::array<UIInput, MAX_PLAYER_COUNT> uiInputs;

    static ApplicationInput *GetFromManager(InputManager *inputManager);

private:
    void OnControllerButtonDown(SDL_JoystickID which, int button);
    void OnControllerButtonUp(SDL_JoystickID which, int button);
    void OnControllerAxisMotion(SDL_JoystickID which, int axis, Sint16 value);
    void OnKeyDown(bool repeat, SDL_Scancode scancode);
    void OnKeyUp(bool repeat, SDL_Scancode scancode);
};
