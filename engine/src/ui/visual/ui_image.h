/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_object.h"

class UIImage : public UIObject
{
public:
    UIImage(Scene *scene, SpriteGroup *spriteGroup, int spriteID);

    enum class RenderMode
    {
        STRETCH, ANCHOR, BORDERS
    };
    void SetRenderMode(RenderMode renderMode);
    RenderMode GetRenderMode() const;

    void SetBorders(const UIBorders &borders);
    void SetBorders(float left, float right, float top, float bottom, float scale = 1.f);

    void SetAnchor(Anchor anchor);
    void SetScale(float scale);

    void SetSpriteGroup(SpriteGroup *spriteGroup);
    void SetSpriteID(int spriteID);
    void SetScaleWithViewport(bool scaleWithViewport);

    void GetNativePixelSize(int &pixelWidth, int &pixelHeight) const;
    void GetNativePixelSize(float &pixelWidth, float &pixelHeight) const;
    float GetNativeAspectRatio() const;
    b2Vec2 GetNativeUISize() const;
    b2Vec2 GetUISize() const;

    void GetNaturalSize(int &pixelWidth, int &pixelHeight) const;
    void GetNaturalSize(float &pixelWidth, float &pixelHeight) const;
    float GetNaturalRatio() const;

    virtual void Render() override;
    virtual void Update() override;
    virtual void DrawImGui() override;

private:
    SpriteGroup *m_spriteGroup;
    int m_spriteID;
    bool m_scaleWithViewport;

    RenderMode m_renderMode;
    Anchor m_anchor;
    UIBorders m_borders;
    float m_scale;
};

inline void UIImage::SetBorders(const UIBorders &borders)
{
    m_borders = borders;
    m_scale = 1.f;
    m_renderMode = RenderMode::BORDERS;
}

inline void UIImage::SetBorders(float left, float right, float top, float bottom, float scale)
{
    SetBorders(UIBorders(left, right, top, bottom, scale));
}

inline void UIImage::GetNaturalSize(int &pixelWidth, int &pixelHeight) const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = static_cast<int>(src->w);
    pixelHeight = static_cast<int>(src->h);
}

inline void UIImage::GetNaturalSize(float &pixelWidth, float &pixelHeight) const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = src->w;
    pixelHeight = src->h;
}

inline float UIImage::GetNaturalRatio() const
{
    const SDL_FRect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return (float)src->w / (float)src->h;
}

inline void UIImage::SetAnchor(Anchor anchor)
{
    m_renderMode = RenderMode::ANCHOR;
    m_anchor = anchor;
}

inline void UIImage::SetScale(float scale)
{
    m_renderMode = RenderMode::ANCHOR;
    m_scale = scale;
}

inline void UIImage::SetSpriteGroup(SpriteGroup *spriteGroup)
{
    m_spriteGroup = spriteGroup;
}

inline void UIImage::SetSpriteID(int spriteID)
{
    m_spriteID = spriteID;
}

inline void UIImage::SetScaleWithViewport(bool scaleWithViewport)
{
    m_scaleWithViewport = scaleWithViewport;
}

inline void UIImage::SetRenderMode(RenderMode renderMode)
{
    m_renderMode = renderMode;
    if (renderMode != RenderMode::ANCHOR)
    {
        m_scale = 1.f;
    }
}

inline UIImage::RenderMode UIImage::GetRenderMode() const
{
    return m_renderMode;
}
