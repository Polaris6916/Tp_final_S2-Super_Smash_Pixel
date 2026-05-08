/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/visual/ui_fill_rect.h"

UIFillRect::UIFillRect(Scene *scene, Color color) :
    UIObject(scene)
{
    SetName("UIFillRect");
    SetColor(color);
}

void UIFillRect::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UIFillRect::Render()
{
    if (IsEnabled() == false) return;

    float alpha = GetAlpha();
    SDL_FRect rect = GetRenderRect();
    Color color = GetColor();
    SDL_SetRenderDrawColor(
        g_renderer, color.r, color.g, color.b,
        (Uint8)(color.a * alpha)
    );
    SDL_RenderFillRect(g_renderer, &rect);
}

void UIFillRect::DrawImGui()
{
    UIObject::DrawImGui();

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIFillRect"))
    {
        if (ImGui::BeginTable("UIFillRect Members", 2, ImGuiTableFlags_RowBg))
        {
            Color color = GetColor();
            ImGuiUtils::Member("Color R", color.r);
            ImGuiUtils::Member("Color G", color.g);
            ImGuiUtils::Member("Color B", color.b);
            ImGuiUtils::Member("Color A", color.a);

            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}
