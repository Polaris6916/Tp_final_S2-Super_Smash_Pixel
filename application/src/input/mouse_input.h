/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

class MouseInput : public InputGroup
{
public:
    MouseInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event event) override;
    virtual void OnPostEventProcess() override;
    virtual void Reset() override;

    // Position de la souris
    b2Vec2 viewPos;
    b2Vec2 lastViewPos;
    b2Vec2 deltaPos;

    // Clic gauche
    int  leftClickCount;
    bool leftDown;
    bool leftReleased;

    // Clic droit
    int  rightClickCount;
    bool rightDown;
    bool rightReleased;

    // Roulette
    bool middleClick;
    float wheel;

    static MouseInput *GetFromManager(InputManager *inputManager);
};
