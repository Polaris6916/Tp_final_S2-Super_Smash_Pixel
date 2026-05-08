/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "input/application_input.h"
#include "common/game_common.h"

ApplicationInput::ApplicationInput()
    : InputGroup()
    , quitPressed(false)
    , pausePressed(false)
    , debugPressed(false)
    , makeStepPressed(false)
    , makeStepDown(false)
    , realtimePressed(false)
{
}

void ApplicationInput::OnPreEventProcess()
{
    quitPressed = false;
    pausePressed = false;
    debugPressed = false;
    makeStepPressed = false;
    realtimePressed = false;

    for (UIInput &input : uiInputs)
    {
        input.up = false;
        input.down = false;
        input.left = false;
        input.right = false;
        input.validatePressed = false;
        input.validateReleased = false;
        input.cancel = false;
        input.start = false;
    }
}

void ApplicationInput::OnEventProcess(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
        quitPressed = true;
        break;

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

void ApplicationInput::Reset()
{
    quitPressed = false;
    pausePressed = false;
    debugPressed = false;
    makeStepPressed = false;
    makeStepDown = false;
    realtimePressed = false;

    for (UIInput &input : uiInputs)
    {
        input.SetAxisLeftX(0);
        input.SetAxisLeftY(0);

        input.up = false;
        input.down = false;
        input.left = false;
        input.right = false;
        input.validatePressed = false;
        input.validateReleased = false;
        input.cancel = false;
        input.start = false;
    }
}

ApplicationInput *ApplicationInput::GetFromManager(InputManager *inputManager)
{
    ApplicationInput *inputGroup = dynamic_cast<ApplicationInput *>(
        inputManager->GetInputGroup((int)InputID::APPLICATION));
    AssertNew(inputGroup);

    return inputGroup;
}

void ApplicationInput::OnControllerButtonDown(SDL_JoystickID which, int button)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
    case SDL_GAMEPAD_BUTTON_WEST:
        uiInputs[playerID].validatePressed = true;
        break;

    case SDL_GAMEPAD_BUTTON_NORTH:
    case SDL_GAMEPAD_BUTTON_EAST:
        uiInputs[playerID].cancel = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        uiInputs[playerID].up = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        uiInputs[playerID].down = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        uiInputs[playerID].left = true;
        break;

    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        uiInputs[playerID].right = true;
        break;

    case SDL_GAMEPAD_BUTTON_START:
        uiInputs[playerID].start = true;
        pausePressed = true;
        break;

    default: break;
    }
}

void ApplicationInput::OnControllerButtonUp(SDL_JoystickID which, int button)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
    case SDL_GAMEPAD_BUTTON_WEST:
        uiInputs[playerID].validateReleased = true;
        break;
    default: break;
    }
}

void ApplicationInput::OnControllerAxisMotion(SDL_JoystickID which, int axis, Sint16 value)
{
    const int playerID = SDL_GetGamepadPlayerIndexForID(which);
    assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

    switch (axis)
    {
    case SDL_GAMEPAD_AXIS_LEFTX:
        uiInputs[playerID].SetAxisLeftX(value);
        break;

    case SDL_GAMEPAD_AXIS_LEFTY:
        uiInputs[playerID].SetAxisLeftY(value);
        break;

    default: break;
    }
}

void ApplicationInput::OnKeyDown(bool repeat, SDL_Scancode scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_ESCAPE:
        quitPressed = true;
        break;

    case SDL_SCANCODE_RETURN:
        pausePressed = true;
        uiInputs[0].start = true;
        break;

    case SDL_SCANCODE_BACKSPACE:
        uiInputs[0].cancel = true;
        break;

    case SDL_SCANCODE_UP:
        uiInputs[0].up = true;
        break;

    case SDL_SCANCODE_DOWN:
        uiInputs[0].down = true;
        break;

    case SDL_SCANCODE_LEFT:
        uiInputs[0].left = true;
        break;

    case SDL_SCANCODE_RIGHT:
        uiInputs[0].right = true;
        break;

    case SDL_SCANCODE_SPACE:
        uiInputs[0].validatePressed = true;
        break;

    case SDL_SCANCODE_F1:
    case SDL_SCANCODE_F2:
    case SDL_SCANCODE_F3:
    case SDL_SCANCODE_F4:
    case SDL_SCANCODE_F5:
    case SDL_SCANCODE_F6:
    case SDL_SCANCODE_F7:
    case SDL_SCANCODE_F8:
        debugPressed = true;
        break;

    case SDL_SCANCODE_TAB:
        makeStepPressed = true;
        makeStepDown = true;
        break;

    case SDL_SCANCODE_LCTRL:
        realtimePressed = true;
        break;

    default: break;
    }
}

void ApplicationInput::OnKeyUp(bool repeat, SDL_Scancode scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_SPACE:
        uiInputs[0].validateReleased = true;
        break;

    case SDL_SCANCODE_TAB:
        makeStepDown = false;
        break;

    default: break;
    }
}
