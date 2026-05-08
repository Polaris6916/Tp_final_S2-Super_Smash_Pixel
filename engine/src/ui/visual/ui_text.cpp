/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/visual/ui_text.h"

UIText::UIText(Scene *scene, const std::string &str, TTF_Font *font, Color color)
    : UIObject(scene)
    , m_anchor(Anchor::CENTER)
    , m_renderMode(RenderMode::ANCHOR)
    , m_pixelW(0)
    , m_pixelH(0)
    , m_ttfText(nullptr)
{
    SetName("UIText");
    SetColor(color);

    m_ttfText = TTF_CreateText(g_textEngine, font, str.c_str(), str.length());
    assert(m_ttfText);

    bool success = true;
    success = TTF_SetTextColor(m_ttfText, color.r, color.g, color.b, color.a);
    assert(success);
    success = TTF_GetTextSize(m_ttfText, &m_pixelW, &m_pixelH);
    assert(success);
}

UIText::~UIText()
{
    TTF_DestroyText(m_ttfText);
}

void UIText::SetString(const std::string &str)
{
    bool success = true;
    success = TTF_SetTextString(m_ttfText, str.c_str(), str.length());
    assert(success);
    success = TTF_GetTextSize(m_ttfText, &m_pixelW, &m_pixelH);
    assert(success);
}

void UIText::Render()
{
    if (IsEnabled() == false) return;

    bool success = true;
    float fadeAlpha = GetAlpha();
    Color color = GetColor();
    success = TTF_SetTextColor(
        m_ttfText, color.r, color.g, color.b,
        static_cast<Uint8>(fadeAlpha * color.a)
    );
    assert(success);

    SDL_FRect dstRect = GetRenderRect();
    dstRect.w -= static_cast<float>(m_pixelW);
    dstRect.h -= static_cast<float>(m_pixelH);

    switch (m_anchor)
    {
    case Anchor::NORTH_WEST:
        break;
    case Anchor::NORTH:
        dstRect.x += 0.5f * dstRect.w;
        break;
    case Anchor::NORTH_EAST:
        dstRect.x += dstRect.w;
        break;
    case Anchor::WEST:
        dstRect.y += 0.5f * dstRect.h;
        break;
    case Anchor::CENTER:
        dstRect.x += 0.5f * dstRect.w;
        dstRect.y += 0.5f * dstRect.h;
        break;
    case Anchor::EAST:
        dstRect.x += dstRect.w;
        dstRect.y += 0.5f * dstRect.h;
        break;
    case Anchor::SOUTH_WEST:
        dstRect.y += dstRect.h;
        break;
    case Anchor::SOUTH:
        dstRect.x += 0.5f * dstRect.w;
        dstRect.y += dstRect.h;
        break;
    case Anchor::SOUTH_EAST:
        dstRect.x += dstRect.w;
        dstRect.y += dstRect.h;
        break;
    default:
        break;
    }
    dstRect.x = roundf(dstRect.x);
    dstRect.y = roundf(dstRect.y);
    
    success = TTF_DrawRendererText(m_ttfText, dstRect.x, dstRect.y);
    assert(success);
}

void UIText::Update()
{
    UIObject::Update();
    SetVisible(true);

    bool success = true;
    success = TTF_GetTextSize(m_ttfText, &m_pixelW, &m_pixelH);
    assert(success);
}

void UIText::DrawImGui()
{
    UIObject::DrawImGui();

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIText"))
    {
        if (ImGui::BeginTable("UIText Members", 2, ImGuiTableFlags_RowBg))
        {
            ImGuiUtils::Member("Anchor", ENUM_STRING(m_anchor));
            ImGuiUtils::Member("Render mode", ENUM_STRING(m_renderMode));
            ImGuiUtils::Member(
                "Text",
                m_ttfText ? m_ttfText->text : "nullptr"
            );

            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}

void UIText::GetNativePixelSize(int &pixelWidth, int &pixelHeight) const
{
    pixelWidth = m_pixelW;
    pixelHeight = m_pixelH;
}

b2Vec2 UIText::GetNativeUISize() const
{
    float pixelsPerUnit = m_scene->GetUIPixelsPerUnit();
    return 1.0f / pixelsPerUnit * b2Vec2((float)m_pixelW, (float)m_pixelH);
}

b2Vec2 UIText::GetUISize() const
{
    if (m_renderMode == RenderMode::STRETCH)
    {
        return GetRectSize();
    }
    else
    {
        return GetNativeUISize();
    }
}
