/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_object.h"

class UICanvas : public UIObject
{
public:
    UICanvas(Scene *scene);

    float GetPixelsPerUnit() const;

    virtual void Update() override;
    virtual void DrawImGui() override;

protected:
    float m_pixelPerUnit;
};

inline float UICanvas::GetPixelsPerUnit() const
{
    return m_pixelPerUnit;
}
