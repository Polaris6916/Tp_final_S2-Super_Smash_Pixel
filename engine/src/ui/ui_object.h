/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "rendering/anim.h"
#include "utils/color.h"

class Scene;
class UIObject;

// Unités UIRect offset : 640 x 360

struct UIRect
{
    UIRect(
        b2Vec2 anchorMin = b2Vec2{ 0.f, 0.f }, b2Vec2 anchorMax = b2Vec2{ 1.f, 1.f },
        b2Vec2 offsetMin = b2Vec2{ 0.f, 0.f }, b2Vec2 offsetMax = b2Vec2{ 0.f, 0.f }
    );

    b2Vec2 anchorMin; // Position relative
    b2Vec2 anchorMax; // Position relative
    b2Vec2 offsetMin; // Position absolue
    b2Vec2 offsetMax; // Position absolue
};

struct UIBorders
{
    UIBorders(
        float left = 0.f, float right = 0.f,
        float top = 0.f, float bottom = 0.f,
        float scale = 1.f
    );

    float left, right, top, bottom;
    float scale;
};

struct UIFadeDef
{
    UIFadeDef();
    void Reset(float duration);

    b2Vec2 shift;
    float delay;
    float duration;
    EasingFct easing;
    bool fadeOpacity;
};

struct UIFadeAnim
{
    UIFadeAnim();

    AnimState state;
    LerpValues<float> alphaValues;
    LerpValues<b2Vec2> shiftValues;
};

struct UITarget
{
    UITarget();

    float alpha;
    Color color;
    UIRect rect;
};

class UIAnimListener
{
public:
    UIAnimListener() {};
    virtual ~UIAnimListener() {};

    virtual void OnFadeInEnd(UIObject *which) {};
    virtual void OnFadeOutEnd(UIObject *which) {};
    virtual void OnTransformEnd(UIObject *which) {};
};

class UIObject : public UIAnimListener
{
public:
    UIObject(Scene *scene);
    UIObject(UIObject const&) = delete;
    UIObject& operator=(UIObject const&) = delete;
    virtual ~UIObject();

    virtual void DrawGizmos();
    virtual void Render();
    virtual void Start();
    virtual void Update();
    virtual void OnDelete();
    virtual void DrawImGui();

    void SetLocalRect(const UIRect &rect);
    UIRect &GetLocalRect();
    SDL_FRect GetCanvasRect() const;
    bool Contains(const SDL_FPoint &point) const;

    void SetVisible(bool isVisible, bool applyToChildren = false);
    void Delete();

    void SetParent(UIObject *parent);
    UIObject *GetParent() const;

    void SetName(const std::string &name);
    const std::string &GetName();

    int GetDepth() const;
    int GetID() const;

    const std::set<UIObject *> &GetChildren();

    bool GetGizmosEnabled() const;
    void SetGizmosEnabled(bool gizmosEnabled);

    void SetFadeInAnimation(UIFadeDef fadeDef);
    void SetFadeOutAnimation(UIFadeDef fadeDef);
    UIFadeAnim &GetFadeInAnim();
    UIFadeAnim &GetFadeOutAnim();
    void PlayFadeIn();
    void PlayFadeOut();
    bool IsFadingIn() const;
    bool IsFadingOut() const;
    float GetFadeInProgression() const;
    float GetFadeOutProgression() const;

    void SetFadeChildren(bool fadeChildren);
    bool GetFadeChildren() const;

    void SetColor(Color color);
    Color GetColor() const;

    void ResetTarget();
    void SetTargetOpacity(float alpha);
    void SetTargetRect(const UIRect &rect);
    void SetTargetColor(Color color);
    void SetTransformEasing(EasingFct easing);
    void SetTransformDuration(float duration);
    void PlayTransformToTarget();
    void TransformToTarget();

    void SetEnabled(bool isUIEnabled);
    bool IsEnabled() const;

    void AddAnimListener(UIAnimListener *listener);
    void RemoveAnimListener(UIAnimListener *listener);

    void SetUseParentAnimation(bool useParentAnim);
    void SetUseParentAlpha(bool useParentAlpha);
    bool UsingParentAnimation() const;
    bool UsingParentAlpha() const;

    void SetOpacity(float alpha);
    void SetShift(b2Vec2 shift);

    SDL_FRect GetRenderRect() const;
    float GetAlpha() const;
    b2Vec2 GetShift() const;

    b2Vec2 GetRectSize() const;

    int GetLayer() const;
    void SetLayer(int layer);

protected:
    virtual void PlayFadeInAlone();
    virtual void PlayFadeOutAlone();
    virtual void OnPlayFadeIn();
    virtual void OnPlayFadeOut();
    virtual void OnPlayTransformToTarget();

    UIRect m_rect;
    Scene *m_scene;
    std::string m_name;
    int m_objectID;

private:
    friend class UIObjectManager;
    enum class Flag : uint32_t
    {
        NONE       = 0x00,
        TO_START   = 0x01,
        TO_DELETE  = 0x02,
        DEBUG_DELETED = 0x04
    };
    void AddChild(UIObject *child);
    void RemoveChild(UIObject *child);
    void UpdateDepth();
    void AddFlags(UIObject::Flag flags);
    void SubFlags(UIObject::Flag flags);
    bool TestFlag(UIObject::Flag flag) const;

    SDL_FRect GetCanvasRectRec(float pixelsPerUnit) const;
    float GetHierarchyBaseAlpha() const;
    float GetHierarchyAnimAlpha() const;

    void NotifyFadeInEnd();
    void NotifyFadeOutEnd();
    void NotifyTransformEnd();

    Flag m_flags;
    bool m_enabled;
    int m_layer;
    int m_depth;
    bool m_gizmosEnabled;

    UIObject *m_parent;
    std::set<UIObject *> m_children;

    std::set<UIAnimListener *> m_animListeners;
    bool m_useParentAnim;
    bool m_useParentAlpha;
    bool m_fadeChildren;

    b2Vec2 m_shift;
    float m_alpha;
    Color m_color;

    int m_targetMask;
    std::array<UITarget, 2> m_targets;

    float m_delayFadeI;
    float m_delayFadeO;
    UIFadeAnim m_fadeIAnim;
    UIFadeAnim m_fadeOAnim;
    AnimState m_transformAnimState;
    EasingFct m_transformEasing;
};

#include "scene/scene.h"

constexpr enum UIObject::Flag operator |(
    const enum UIObject::Flag selfValue,
    const enum UIObject::Flag inValue)
{
    return static_cast<enum UIObject::Flag>(
        static_cast<uint32_t>(selfValue) | static_cast<uint32_t>(inValue));
}

constexpr enum UIObject::Flag operator &(
    const enum UIObject::Flag selfValue,
    const enum UIObject::Flag inValue)
{
    return static_cast<enum UIObject::Flag>(
        static_cast<uint32_t>(selfValue) & static_cast<uint32_t>(inValue));
}

constexpr enum UIObject::Flag operator ~(
    const enum UIObject::Flag selfValue)
{
    return static_cast<enum UIObject::Flag>(
        ~static_cast<uint32_t>(selfValue));
}

inline void UIObject::SetVisible(bool isVisible, bool applyToChildren)
{
    m_scene->GetUIObjectManager()->SetVisible(this, isVisible);

    if (applyToChildren == false) return;
    for (auto child : m_children)
    {
        child->SetVisible(isVisible, true);
    }
}

inline void UIObject::Delete()
{
    m_scene->GetUIObjectManager()->DeleteObject(this);
}

inline UIObject *UIObject::GetParent() const
{
    return m_parent;
}

inline void UIObject::SetName(const std::string &name)
{
    m_name = name;
}

inline const std::string &UIObject::GetName()
{
    return m_name;
}

inline int UIObject::GetDepth() const
{
    return m_depth;
}

inline int UIObject::GetID() const
{
    return m_objectID;
}

inline const std::set<UIObject *> &UIObject::GetChildren()
{
    return m_children;
}

inline bool UIObject::GetGizmosEnabled() const
{
    return m_gizmosEnabled;
}

inline void UIObject::SetGizmosEnabled(bool gizmosEnabled)
{
    m_gizmosEnabled = gizmosEnabled;
}

inline void UIObject::AddFlags(UIObject::Flag flags)
{
    m_flags = m_flags | flags;
}

inline void UIObject::SubFlags(UIObject::Flag flags)
{
    m_flags = m_flags & (~flags);
}

inline bool UIObject::TestFlag(UIObject::Flag flag) const
{
    return (flag == (m_flags & flag));
}

inline void UIObject::SetLocalRect(const UIRect &rect)
{
    m_rect = rect;
}

inline UIRect &UIObject::GetLocalRect()
{
    return m_rect;
}

inline void UIObject::SetTransformEasing(EasingFct easing)
{
    m_transformEasing = easing;
}

inline void UIObject::SetTransformDuration(float duration)
{
    m_transformAnimState.cycleTime = duration;
}

inline void UIObject::SetUseParentAnimation(bool useParentAnim)
{
    m_useParentAnim = useParentAnim;
}

inline void UIObject::SetUseParentAlpha(bool useParentAlpha)
{
    m_useParentAlpha = useParentAlpha;
}

inline bool UIObject::UsingParentAnimation() const
{
    return m_useParentAnim;
}

inline bool UIObject::UsingParentAlpha() const
{
    return m_useParentAlpha;
}

inline void UIObject::SetOpacity(float alpha)
{
    m_alpha = alpha;
}

inline void UIObject::SetShift(b2Vec2 shift)
{
    m_shift = shift;
}

inline void UIObject::SetColor(Color color)
{
    m_color = color;
}

inline Color UIObject::GetColor() const
{
    return m_color;
}

inline void UIObject::SetFadeChildren(bool fadeChildren)
{
    m_fadeChildren = fadeChildren;
}

inline bool UIObject::GetFadeChildren() const
{
    return m_fadeChildren;
}

inline UIRect operator + (const UIRect& a, const UIRect& b)
{
    return UIRect(
        a.anchorMin + b.anchorMin,
        a.anchorMax + b.anchorMax,
        a.offsetMin + b.offsetMin,
        a.offsetMax + b.offsetMax
    );
}

inline UIRect operator - (const UIRect& a, const UIRect& b)
{
    return UIRect(
        a.anchorMin - b.anchorMin,
        a.anchorMax - b.anchorMax,
        a.offsetMin - b.offsetMin,
        a.offsetMax - b.offsetMax
    );
}

inline UIRect operator * (float s, const UIRect& a)
{
    return UIRect(
        s * a.anchorMin,
        s * a.anchorMax,
        s * a.offsetMin,
        s * a.offsetMax
    );
}
