/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_object.h"
#include "ui/ui_object_manager.h"
#include "ui/base/ui_canvas.h"
#include "scene/scene.h"
#include "utils/utils.h"

constexpr uint32_t TARGET_RECT = 0x1;
constexpr uint32_t TARGET_ALPHA = 0x2;
constexpr uint32_t TARGET_COLOR = 0x4;

UIBorders::UIBorders(float left, float right, float top, float bottom, float scale)
    : left(left)
    , right(right)
    , top(top)
    , bottom(bottom)
    , scale(scale)
{}

UIFadeAnim::UIFadeAnim()
    : state(1.f, 0, true)
    , alphaValues{ 1.f, 1.f, EasingFct_Linear }
    , shiftValues{ b2Vec2_zero, b2Vec2_zero, EasingFct_Linear }
{
    AnimUtils::Stop(state);
}

UIFadeDef::UIFadeDef()
    : shift(b2Vec2_zero)
    , delay(0.f)
    , duration(0.25f)
    , easing(EasingFct_InOut)
    , fadeOpacity(true)
{}

UIRect::UIRect(b2Vec2 anchorMin, b2Vec2 anchorMax, b2Vec2 offsetMin, b2Vec2 offsetMax)
    : anchorMin(anchorMin)
    , anchorMax(anchorMax)
    , offsetMin(offsetMin)
    , offsetMax(offsetMax)
{}

UITarget::UITarget()
    : alpha(1.f)
    , color()
    , rect()
{}

UIObject::UIObject(Scene *scene)
    : m_scene(scene)
    , m_children()
    , m_parent(nullptr)
    , m_depth(0)
    , m_layer(DEFAULT_UI_LAYER)
    , m_name()
    , m_flags(Flag::NONE)
    , m_objectID(-1)
    , m_gizmosEnabled(false)
    , m_rect()
    , m_useParentAnim(true)
    , m_useParentAlpha(true)
    , m_alpha(1.f)
    , m_fadeChildren(true)
    , m_animListeners()
    , m_targets()
    , m_targetMask(0)
    , m_enabled(true)
    , m_fadeIAnim()
    , m_fadeOAnim()
    , m_transformAnimState(0.25f, 1, true)
    , m_transformEasing(EasingFct_InOut)
    , m_shift(b2Vec2_zero)
    , m_delayFadeI(0.f)
    , m_delayFadeO(0.f)
{
    SetName("UIObject");
    scene->GetUIObjectManager()->AddObject(this);

    UIObject *canvas = scene->GetCanvas();
    if (canvas && this != canvas) SetParent(canvas);

    AddAnimListener(this);
}

void UIObject::AddChild(UIObject *child)
{
    assert(child);

    m_children.insert(child);
    child->m_parent = this;
    child->UpdateDepth();
}

void UIObject::RemoveChild(UIObject *child)
{
    assert(child);

    auto it = m_children.find(child);
    if (it == m_children.end())
    {
        assert(false);
        return;
    }

    m_children.erase(child);
    child->m_parent = nullptr;
    child->UpdateDepth();
}

void UIObject::UpdateDepth()
{
    m_depth = (m_parent == nullptr) ? 0 : m_parent->m_depth + 1;
    for (UIObject *child : m_children)
    {
        child->UpdateDepth();
    }
}

void UIObject::SetParent(UIObject *parent)
{
    if (parent == this || m_parent == parent) return;

    if (m_parent) m_parent->RemoveChild(this);
    if (parent) parent->AddChild(this);
    
    m_parent = parent;
}

UIObject::~UIObject()
{
    // C'est la scene qui s'occupe de supprimer les objets fils
}

void UIObject::Render()
{
}

void UIObject::Start()
{
}

void UIObject::Update()
{
    if (IsEnabled() == false) return;

    const float dt = m_scene->GetTime().GetUnscaledDelta();
    if (AnimUtils::IsPlaying(m_fadeIAnim.state))
    {
        AnimUtils::Update(m_fadeIAnim.state, dt);
        for (int i = 0; i < m_fadeIAnim.state.eventCount; i++)
        {
            AnimEvent &animEvent = m_fadeIAnim.state.events[i];
            if (animEvent.type == AnimEvent::Type::ANIMATION_END)
            {
                NotifyFadeInEnd();
                break;
            }
        }
    }
    if (AnimUtils::IsPlaying(m_fadeOAnim.state))
    {
        AnimUtils::Update(m_fadeOAnim.state, dt);
        for (int i = 0; i < m_fadeOAnim.state.eventCount; i++)
        {
            AnimEvent &animEvent = m_fadeOAnim.state.events[i];
            if (animEvent.type == AnimEvent::Type::ANIMATION_END)
            {
                SetEnabled(false);
                for (UIAnimListener *listener : m_animListeners)
                {
                    NotifyFadeOutEnd();
                    break;
                }
            }
        }
    }
    if (AnimUtils::IsPlaying(m_transformAnimState))
    {
        AnimUtils::Update(m_transformAnimState, dt);
        for (int i = 0; i < m_transformAnimState.eventCount; i++)
        {
            AnimEvent &animEvent = m_transformAnimState.events[i];
            if (animEvent.type == AnimEvent::Type::ANIMATION_END)
            {
                NotifyTransformEnd();
                break;
            }
        }
        const float t = AnimUtils::GetProgression(m_transformAnimState);

        if ((m_targetMask & TARGET_RECT) != 0)
        {
            m_rect = math::Lerp(m_targets[0].rect, m_targets[1].rect, t);
        }
        if ((m_targetMask & TARGET_ALPHA) != 0)
        {
            m_alpha = math::Lerp(m_targets[0].alpha, m_targets[1].alpha, t);
        }
        if ((m_targetMask & TARGET_COLOR) != 0)
        {
            m_color = math::Lerp(m_targets[0].color, m_targets[1].color, t);
        }
    }
}

void UIObject::OnDelete()
{
}

void UIObject::DrawImGui()
{
    std::shared_ptr<ImGuiManagerBase> managerPtr = m_scene->GetImGuiManager();
    ImGui::SeparatorText(ImGuiManagerBase::GetUIName(this).c_str());
    ImGuiManagerBase *imGuiManager = managerPtr.get();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIObject"))
    {
        if (ImGui::BeginTable("UIObject Members", 2, ImGuiTableFlags_RowBg))
        {
            ImGuiUtils::Member("Enabled", m_enabled);
            ImGuiUtils::Member("Layer", m_layer);
            ImGuiUtils::Member("Local rect anchorMin", m_rect.anchorMin);
            ImGuiUtils::Member("Local rect anchorMax", m_rect.anchorMax);
            ImGuiUtils::Member("Local rect offsetMin", m_rect.offsetMin);
            ImGuiUtils::Member("Local rect offsetMax", m_rect.offsetMax);
            ImGuiUtils::Member("Fade children", m_fadeChildren);
            ImGuiUtils::Member("Alpha", m_alpha);
            ImGuiUtils::Member("Shift", m_shift);
            ImGuiUtils::Member("Use parent anim", m_useParentAnim);
            ImGuiUtils::Member("Use parent alpha", m_useParentAlpha);
            ImGuiUtils::Member("Hierarchy base alpha", GetHierarchyBaseAlpha());
            ImGuiUtils::Member("Hierarchy anim alpha", GetHierarchyAnimAlpha());
            ImGuiUtils::Member("Hierarchy shift", GetShift());
            ImGuiUtils::Member("Is fading In", IsFadingIn());
            ImGuiUtils::Member("Is fading Out", IsFadingOut());

            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}

void UIObject::SetTargetRect(const UIRect &rect)
{
    m_targetMask |= TARGET_RECT;
    m_targets[1].rect = rect;
}

void UIObject::SetTargetOpacity(float alpha)
{
    m_targetMask |= TARGET_ALPHA;
    m_targets[1].alpha = alpha;
}

void UIObject::SetTargetColor(Color color)
{
    m_targetMask |= TARGET_COLOR;
    m_targets[1].color = color;
}

b2Vec2 UIObject::GetRectSize() const
{
    SDL_FRect rect = GetCanvasRectRec(1.0f);
    return b2Vec2(rect.w, rect.h);
}

float UIObject::GetHierarchyBaseAlpha() const
{
    float alpha = 1.f;

    const UIObject *uiObject = this;
    while (uiObject != nullptr)
    {
        alpha *= uiObject->m_alpha;
        if (uiObject->m_useParentAlpha == false) break;

        uiObject = uiObject->GetParent();
    }

    return alpha;
}

float UIObject::GetHierarchyAnimAlpha() const
{
    float alpha = 1.f;

    const UIObject *uiObject = this;
    while (uiObject != nullptr)
    {
        const UIFadeAnim *anims[] = {
            &(uiObject->m_fadeIAnim),
            &(uiObject->m_fadeOAnim),
        };
        for (const UIFadeAnim *anim : anims)
        {
            if (AnimUtils::IsPlaying(anim->state) == false) continue;
            alpha *= AnimUtils::GetValue(anim->state, anim->alphaValues);
        }

        if (uiObject->m_useParentAnim == false) break;
        uiObject = uiObject->GetParent();
    }

    return alpha;
}

void UIObject::NotifyFadeInEnd()
{
    for (UIAnimListener *listener : m_animListeners)
    {
        listener->OnFadeInEnd(this);
    }
}

void UIObject::NotifyFadeOutEnd()
{
    for (UIAnimListener *listener : m_animListeners)
    {
        listener->OnFadeOutEnd(this);
    }
}

void UIObject::NotifyTransformEnd()
{
    for (UIAnimListener *listener : m_animListeners)
    {
        listener->OnTransformEnd(this);
    }
}

SDL_FRect UIObject::GetCanvasRect() const
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);

    float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();
    SDL_FRect rect = GetCanvasRectRec(pixelsPerUnit);
    rect.y = viewport.h - rect.y - rect.h;
    return rect;
}

SDL_FRect UIObject::GetCanvasRectRec(float pixelsPerUnit) const
{
    UIObject *uiParent = dynamic_cast<UIObject *>(GetParent());

    if (uiParent)
    {
        SDL_FRect pRect = uiParent->GetCanvasRectRec(pixelsPerUnit);
        float lowerX = pRect.x + pRect.w * m_rect.anchorMin.x + pixelsPerUnit * m_rect.offsetMin.x;
        float upperX = pRect.x + pRect.w * m_rect.anchorMax.x + pixelsPerUnit * m_rect.offsetMax.x;
        float lowerY = pRect.y + pRect.h * m_rect.anchorMin.y + pixelsPerUnit * m_rect.offsetMin.y;
        float upperY = pRect.y + pRect.h * m_rect.anchorMax.y + pixelsPerUnit * m_rect.offsetMax.y;

        return SDL_FRect{
            lowerX,
            lowerY,
            fabsf(upperX - lowerX),
            fabsf(upperY - lowerY)
        };
    }
    else
    {
        return SDL_FRect{
            pixelsPerUnit * m_rect.offsetMin.x,
            pixelsPerUnit * m_rect.offsetMin.y,
            pixelsPerUnit * fabsf(m_rect.offsetMax.x - m_rect.offsetMin.x),
            pixelsPerUnit * fabsf(m_rect.offsetMax.y - m_rect.offsetMin.y)
        };
    }
}

bool UIObject::Contains(const SDL_FPoint &point) const
{
    SDL_FRect rect = GetCanvasRect();
    return (point.x >= rect.x) && (point.x <= rect.x + rect.w)
        && (point.y >= rect.y) && (point.y <= rect.y + rect.h);
}

void UIObject::DrawGizmos()
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);
    SDL_FRect canvasRect = GetCanvasRect();
    if ((fabsf(viewport.w - canvasRect.w) < 1.f) &&
        (fabsf(viewport.h - canvasRect.h) < 1.f))
    {
        SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
        SDL_RenderRect(g_renderer, &canvasRect);
    }
    else
    {
        SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 31);
        SDL_RenderFillRect(g_renderer, &canvasRect);
    }
}

void UIObject::SetFadeInAnimation(UIFadeDef fadeDef)
{
    m_delayFadeI = fadeDef.delay;

    m_fadeIAnim.state.cycleCount = 1;
    m_fadeIAnim.state.cycleTime = fadeDef.duration;
    m_fadeIAnim.state.cycleIdx = 0;
    m_fadeIAnim.state.flags = AnimFlag::STOPPED;

    m_fadeIAnim.alphaValues.value0 = fadeDef.fadeOpacity ? 0.f : 1.f;
    m_fadeIAnim.alphaValues.value1 = 1.f;
    m_fadeIAnim.alphaValues.easing = fadeDef.easing;

    m_fadeIAnim.shiftValues.value0 = fadeDef.shift;
    m_fadeIAnim.shiftValues.value1 = b2Vec2_zero;
    m_fadeIAnim.shiftValues.easing = fadeDef.easing;
}

void UIObject::SetFadeOutAnimation(UIFadeDef fadeDef)
{
    m_delayFadeO = fadeDef.delay;

    m_fadeOAnim.state.cycleCount = 1;
    m_fadeOAnim.state.cycleTime = fadeDef.duration;
    m_fadeOAnim.state.cycleIdx = 0;
    m_fadeOAnim.state.flags = AnimFlag::STOPPED;
    m_fadeOAnim.state.accu = -1.f * fadeDef.delay;

    m_fadeOAnim.alphaValues.value0 = 1.f;
    m_fadeOAnim.alphaValues.value1 = fadeDef.fadeOpacity ? 0.f : 1.f;
    m_fadeOAnim.alphaValues.easing = fadeDef.easing;

    m_fadeOAnim.shiftValues.value0 = b2Vec2_zero;
    m_fadeOAnim.shiftValues.value1 = fadeDef.shift;
    m_fadeOAnim.shiftValues.easing = fadeDef.easing;
}

void UIObject::PlayFadeIn()
{
    if (AnimUtils::IsPlaying(m_fadeIAnim.state)) return;

    SetEnabled(true);
    OnPlayFadeIn();

    if (m_fadeChildren)
    {
        for (UIObject *child : GetChildren())
        {
            if (child) child->PlayFadeIn();
        }
    }

    if (AnimUtils::IsPlaying(m_fadeOAnim.state))
    {
        AnimUtils::Stop(m_fadeOAnim.state);
        NotifyFadeOutEnd();
    }
}

void UIObject::PlayFadeOut()
{
    if (AnimUtils::IsPlaying(m_fadeOAnim.state)) return;

    OnPlayFadeOut();

    if (m_fadeChildren)
    {
        for (UIObject *child : GetChildren())
        {
            if (child) child->PlayFadeIn();
        }
    }

    if (AnimUtils::IsPlaying(m_fadeIAnim.state))
    {
        AnimUtils::Stop(m_fadeIAnim.state);
        NotifyFadeInEnd();
    }
}

bool UIObject::IsFadingIn() const
{
    return AnimUtils::IsPlaying(m_fadeIAnim.state);
}

bool UIObject::IsFadingOut() const
{
    return AnimUtils::IsPlaying(m_fadeOAnim.state);
}

float UIObject::GetFadeInProgression() const
{
    return AnimUtils::GetProgression(m_fadeIAnim.state);
}

float UIObject::GetFadeOutProgression() const
{
    return AnimUtils::GetProgression(m_fadeOAnim.state);
}

void UIObject::AddAnimListener(UIAnimListener *listener)
{
    m_animListeners.insert(listener);
}

void UIObject::RemoveAnimListener(UIAnimListener *listener)
{
    m_animListeners.erase(listener);
}

SDL_FRect UIObject::GetRenderRect() const
{
    SDL_FRect rect = GetCanvasRect();
    const b2Vec2 shift = GetShift();
    const float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();
    rect.x += pixelsPerUnit * shift.x;
    rect.y -= pixelsPerUnit * shift.y;

    // Pixel perfect
    rect.x = roundf(rect.x);
    rect.y = roundf(rect.y);
    rect.w = roundf(rect.w);
    rect.h = roundf(rect.h);
    return rect;
}

float UIObject::GetAlpha() const
{
    return GetHierarchyBaseAlpha() * GetHierarchyAnimAlpha();
}

b2Vec2 UIObject::GetShift() const
{
    b2Vec2 shift = b2Vec2_zero;

    const UIObject *uiObject = this;
    while (uiObject != nullptr)
    {
        shift += uiObject->m_shift;
        const UIFadeAnim *anims[] = {
            &(uiObject->m_fadeIAnim),
            &(uiObject->m_fadeOAnim),
        };
        for (const UIFadeAnim *anim : anims)
        {
            if (AnimUtils::IsPlaying(anim->state) == false) continue;
            shift += AnimUtils::GetValue(anim->state, anim->shiftValues);
        }

        if (uiObject->m_useParentAnim == false) break;
        uiObject = uiObject->GetParent();
    }

    return shift;
}

void UIObject::PlayFadeInAlone()
{
    SetEnabled(true);

    if (m_fadeIAnim.state.cycleTime <= 0.f)
    {
        NotifyFadeInEnd();
    }
    else
    {
        AnimUtils::Play(m_fadeIAnim.state, m_delayFadeI);
    }
}

void UIObject::PlayFadeOutAlone()
{
    if (m_fadeOAnim.state.cycleTime <= 0.f)
    {
        SetEnabled(false);
        NotifyFadeOutEnd();
    }
    else
    {
        AnimUtils::Play(m_fadeOAnim.state, m_delayFadeO);
    }
}

void UIObject::OnPlayFadeIn()
{
    PlayFadeInAlone();
}

void UIObject::OnPlayFadeOut()
{
    PlayFadeOutAlone();
}

void UIObject::OnPlayTransformToTarget()
{
}

void UIObject::ResetTarget()
{
    m_targetMask = 0;
    AnimUtils::Stop(m_transformAnimState);
}

void UIObject::PlayTransformToTarget()
{
    m_targets[0].rect = m_rect;
    m_targets[0].alpha = m_alpha;
    m_targets[0].color = m_color;
    AnimUtils::Play(m_transformAnimState);
    OnPlayTransformToTarget();
}

void UIObject::TransformToTarget()
{
    if ((m_targetMask & TARGET_RECT) != 0)
    {
        m_rect = m_targets[1].rect;
    }
    if ((m_targetMask & TARGET_ALPHA) != 0)
    {
        m_alpha = m_targets[1].alpha;
    }
    if ((m_targetMask & TARGET_COLOR) != 0)
    {
        m_color = m_targets[1].color;
    }
    m_targetMask = 0;
}

void UIObject::SetEnabled(bool isEnabled)
{
    m_enabled = isEnabled;
}

bool UIObject::IsEnabled() const
{
    if (m_enabled == false) return false;

    UIObject *parent = GetParent();
    if (parent == nullptr) return m_enabled;

    return parent->IsEnabled();
}

UIFadeAnim &UIObject::GetFadeInAnim()
{
    return m_fadeIAnim;
}

UIFadeAnim &UIObject::GetFadeOutAnim()
{
    return m_fadeOAnim;
}

void UIFadeDef::Reset(float durationIn)
{
    shift = b2Vec2_zero;
    delay = 0.f;
    duration = durationIn;
    easing = EasingFct_InOut;
    fadeOpacity = true;
}

int UIObject::GetLayer() const
{
    return m_layer;
}

void UIObject::SetLayer(int layer)
{
    m_layer = layer;
}
