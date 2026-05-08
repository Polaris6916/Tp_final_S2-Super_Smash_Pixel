/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/visual/ui_image.h"

UIImage::UIImage(Scene *scene, SpriteGroup *spriteGroup, int index)
    : UIObject(scene)
    , m_spriteGroup(spriteGroup)
    , m_borders(0, 0, 0, 0)
    , m_renderMode(RenderMode::STRETCH)
    , m_spriteID(index)
    , m_scale(1.f)
    , m_anchor(Anchor::CENTER)
    , m_scaleWithViewport(true)
{
    SetName("UIImage");
}

void UIImage::Render()
{
    if (IsEnabled() == false) return;
    if (m_spriteGroup == nullptr) return;

    SDL_Texture *texture = m_spriteGroup->GetTexture();
    const SDL_FRect *srcRect = m_spriteGroup->GetSourceRect(m_spriteID);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(texture, GetAlpha());

    const float viewportScale = m_scaleWithViewport ? game::GetViewportScale() : 1.f;

    SDL_FRect dstRect = GetRenderRect();
    switch (m_renderMode)
    {
    default:
    case UIImage::RenderMode::STRETCH:
        SDL_RenderTexture(g_renderer, texture, srcRect, &dstRect);
        break;

    case UIImage::RenderMode::ANCHOR:
    {
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
        dstRect.h = srcRect->h * m_scale * viewportScale;
        dstRect.w = srcRect->w * m_scale * viewportScale;
        RenderTexture(g_renderer, texture, srcRect, &dstRect, m_anchor);
        break;
    }

    case UIImage::RenderMode::BORDERS:
        RenderTexture9Grid(
            g_renderer, texture, srcRect, &dstRect, Anchor::NORTH_WEST,
            m_borders.left, m_borders.right,
            m_borders.top, m_borders.bottom,
            m_borders.scale * viewportScale
        );
        break;
    }
}

void UIImage::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UIImage::DrawImGui()
{
    UIObject::DrawImGui();

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIImage"))
    {
        if (ImGui::BeginTable("UIImage Members", 2, ImGuiTableFlags_RowBg))
        {
            ImGuiUtils::Member("Anchor", ENUM_STRING(m_anchor));
            ImGuiUtils::Member("Render mode", ENUM_STRING(m_renderMode));
            ImGuiUtils::Member("Scale", m_scale);
            ImGuiUtils::Member("Scale with viewport", m_scaleWithViewport);
            ImGuiUtils::Member(
                "Sprite group",
                m_spriteGroup ? m_spriteGroup->GetName() : "nullptr"
            );
            ImGuiUtils::Member("Sprite index", m_spriteID);
            if (ImGuiUtils::BeginSection("Borders"))
            {
                ImGuiUtils::Member("Top", m_borders.top);
                ImGuiUtils::Member("Bottom", m_borders.bottom);
                ImGuiUtils::Member("Left", m_borders.left);
                ImGuiUtils::Member("Right", m_borders.right);
                ImGuiUtils::Member("Scale", m_borders.scale);

                ImGuiUtils::EndSection();
            }

            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}

void UIImage::GetNativePixelSize(int &pixelWidth, int &pixelHeight) const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = static_cast<int>(src->w);
    pixelHeight = static_cast<int>(src->h);
}

void UIImage::GetNativePixelSize(float &pixelWidth, float &pixelHeight) const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = src->w;
    pixelHeight = src->h;
}

float UIImage::GetNativeAspectRatio() const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return src->w / src->h;
}

b2Vec2 UIImage::GetNativeUISize() const
{
    float pixelsPerUnit = m_scene->GetUIPixelsPerUnit();
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return 1.0f / pixelsPerUnit * b2Vec2(src->w, src->h);
}

b2Vec2 UIImage::GetUISize() const
{
    if (m_renderMode == RenderMode::ANCHOR)
    {
        return m_scale * GetNativeUISize();
    }
    else
    {
        return GetRectSize();
    }
}
