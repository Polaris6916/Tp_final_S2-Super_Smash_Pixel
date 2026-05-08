/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/visual/ui_animator.h"

UIAnimator::UIAnimator(Scene *scene, uint32_t animID)
    : UIObject(scene)
    , m_state(animID, false)
    , m_stretch(false)
    , m_scale(1.f)
    , m_anchor(Anchor::CENTER)
    , m_scaleWithViewport(true)
{
    SetName("UIAnimator");
}

UIAnimator::~UIAnimator()
{
}

void UIAnimator::Render()
{
    if (IsEnabled() == false) return;

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    SpriteAnim *spriteAnim = animManager->GetSpriteAnim(m_state);
    if (spriteAnim == nullptr) return;
    SpriteGroup *spriteGroup = spriteAnim->GetSpriteGroup();
    if (spriteGroup == nullptr) return;
    SDL_Texture *texture = spriteGroup->GetTexture();
    if (texture == nullptr) return;

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(texture, GetAlpha());

    const float viewportScale = m_scaleWithViewport ? game::GetViewportScale() : 1.f;

    const SDL_FRect *srcRect = spriteGroup->GetSourceRect(m_state.frameIdx);
    SDL_FRect dstRect = GetRenderRect();

    if (m_stretch)
    {
        SDL_RenderTexture(g_renderer, texture, srcRect, &dstRect);
    }
    else
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

        dstRect.h = (float)srcRect->h * m_scale * viewportScale;
        dstRect.w = (float)srcRect->w * m_scale * viewportScale;

        RenderTexture(g_renderer, texture, srcRect, &dstRect, m_anchor);
    }
}

void UIAnimator::Update()
{
    UIObject::Update();
    SetVisible(true);

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    SpriteAnim *spriteAnim = animManager->GetSpriteAnim(m_state);
    if (spriteAnim == nullptr) return;

    const float dt = m_scene->GetUnscaledDelta();
    spriteAnim->Update(m_state, dt);
}

void UIAnimator::DrawImGui()
{
    UIObject::DrawImGui();

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIAnimator"))
    {
        if (ImGui::BeginTable("UIAnimator Members", 2, ImGuiTableFlags_RowBg))
        {
            const std::string idString = animManager->GetAnimName(m_state.id);
            ImGuiUtils::Member("Anchor", ENUM_STRING(m_anchor));
            ImGuiUtils::Member("Stretch", m_stretch);
            ImGuiUtils::Member("Scale", m_scale);
            ImGuiUtils::Member("Scale with viewport", m_scaleWithViewport);

            if (ImGuiUtils::BeginSection("Animation state"))
            {
                ImGuiUtils::Member("Animation ID", idString.c_str());
                ImGuiUtils::Member("Cycle index", m_state.cycleIdx);
                ImGuiUtils::Member("Frame index", m_state.frameIdx);
                ImGuiUtils::Member("Speed", m_state.speed);
                ImGuiUtils::Member("Is playing", SpriteAnimUtils::IsPlaying(m_state));
                ImGuiUtils::Member("Is stopped", SpriteAnimUtils::IsStopped(m_state));
                ImGuiUtils::Member("Is delayed", SpriteAnimUtils::IsDelayed(m_state));

                ImGuiUtils::EndSection();
            }

            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}
