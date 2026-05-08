/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_object.h"
#include "rendering/anim.h"

class UIAnimator : public UIObject
{
public:
    UIAnimator(Scene *scene, uint32_t animID);
    virtual ~UIAnimator();

    void SetAnchor(Anchor anchor);
    void SetScale(float scale);
    void Stretch();
    void SetScaleWithViewport(bool scaleWithViewport);
    SpriteAnimState &GetSpriteAnimState();

    virtual void Render() override;
    virtual void Update() override;
    virtual void DrawImGui() override;

private:
    SpriteAnimState m_state;
    Anchor m_anchor;
    float m_scale;
    bool m_stretch;
    bool m_scaleWithViewport;
};

inline SpriteAnimState &UIAnimator::GetSpriteAnimState()
{
    return m_state;
}

inline void UIAnimator::SetAnchor(Anchor anchor)
{
    m_anchor = anchor;
    m_stretch = false;
}

inline void UIAnimator::SetScale(float scale)
{
    m_scale = scale;
}

inline void UIAnimator::Stretch()
{
    m_stretch = true;
}

inline void UIAnimator::SetScaleWithViewport(bool scaleWithViewport)
{
    m_scaleWithViewport = scaleWithViewport;
}
