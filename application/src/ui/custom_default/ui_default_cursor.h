/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

class UIDefaultCursor : public UIImage
{
public:
    UIDefaultCursor(Scene *scene);

    virtual void Update() override;

protected:
    AnimState m_shiftAnim;
    AnimState m_alphaAnim;
    LerpValues<b2Vec2> m_shiftValues;
    LerpValues<float> m_alphaValues;
};
