/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "input/mouse_input.h"
#include "common/game_common.h"

MouseInput::MouseInput()
    : InputGroup()
    , leftClickCount(0), leftDown(false), leftReleased(false)
    , rightClickCount(0), rightDown(false), rightReleased(false)
    , middleClick(false), wheel(0.f)
    , viewPos(b2Vec2_zero), lastViewPos(b2Vec2_zero), deltaPos(b2Vec2_zero)
{
}

void MouseInput::OnPreEventProcess()
{
    leftClickCount = 0;
    leftReleased = false;
    rightClickCount = 0;
    rightReleased = false;
    middleClick = false;
    wheel = 0;
    lastViewPos = viewPos;
}

void MouseInput::OnEventProcess(SDL_Event event)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    Uint8 mouseButton;
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        SDL_RenderCoordinatesFromWindow(g_renderer, event.motion.x, event.motion.y, &(viewPos.x), &(viewPos.y));
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        mouseButton = event.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_MIDDLE:
            middleClick = true;
            break;
        case SDL_BUTTON_LEFT:
            leftClickCount = event.button.clicks;
            leftDown = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightClickCount = event.button.clicks;
            rightDown = true;
            break;
        default:
            break;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        mouseButton = event.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_LEFT:
            leftDown = false;
            leftReleased = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightDown = false;
            rightReleased = true;
            break;
        default:
            break;
        }
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        wheel += event.wheel.y;
        break;

    default:
        break;
    }
}

void MouseInput::OnPostEventProcess()
{
    deltaPos = viewPos - lastViewPos;
}

void MouseInput::Reset()
{
    viewPos = b2Vec2_zero;
    lastViewPos = b2Vec2_zero;

    leftClickCount = 0;
    leftDown = false;
    leftReleased = false;

    rightClickCount = 0;
    rightDown = false;
    rightReleased = false;

    middleClick = false;
    wheel = 0;
}

MouseInput *MouseInput::GetFromManager(InputManager *inputManager)
{
    MouseInput *inputGroup = dynamic_cast<MouseInput *>(
        inputManager->GetInputGroup((int)InputID::MOUSE));
    AssertNew(inputGroup);

    return inputGroup;
}
