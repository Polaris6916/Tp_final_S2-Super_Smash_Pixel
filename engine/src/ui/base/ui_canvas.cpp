/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/base/ui_canvas.h"
#include "scene/scene.h"

UICanvas::UICanvas(Scene *scene)
    : UIObject(scene)
{
    SetName("UICanvas");
    m_rect.offsetMin = { 0.f, 0.f };
    m_rect.offsetMax = { 640.f, 360.f };
    m_rect.anchorMin = { 0.f, 0.f };
    m_rect.anchorMax = { 0.f, 0.f };

    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);

    m_pixelPerUnit = (float)viewport.w / 640.f;
}

void UICanvas::Update()
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);

    m_pixelPerUnit = (float)viewport.w / 640.f;
}

void UICanvas::DrawImGui()
{
    UIObject::DrawImGui();
}
