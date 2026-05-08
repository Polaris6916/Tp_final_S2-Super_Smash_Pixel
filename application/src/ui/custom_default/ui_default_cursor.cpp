/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/custom_default/ui_default_cursor.h"

UIDefaultCursor::UIDefaultCursor(Scene *scene)
    : UIImage(scene, nullptr, 0)
    , m_shiftAnim(1.5f, -1)
    , m_alphaAnim(1.f, -1)
    , m_shiftValues{ b2Vec2_zero, b2Vec2{ -5.f, 0.f }, EasingFct_Cos }
    , m_alphaValues{ 1.f, 0.5f, EasingFct_Cos }
{
    SetName("Cursor");
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Cursor");
    AssertNew(spriteGroup);

    SetSpriteGroup(spriteGroup);
    SetLayer(LAYER_UI_FOREGROUND);
    SetAnchor(Anchor::EAST);
    SetScale(4.f);
    SetTransformDuration(0.25f);
    SetTransformEasing(EasingFct_Cos);

    AnimUtils::AddFlags(m_shiftAnim, AnimFlag::ALTERNATE);
    AnimUtils::AddFlags(m_alphaAnim, AnimFlag::ALTERNATE);
}

void UIDefaultCursor::Update()
{
    UIImage::Update();

    const float dt = m_scene->GetTime().GetUnscaledDelta();
    AnimUtils::Update(m_shiftAnim, dt);
    AnimUtils::Update(m_alphaAnim, dt);

    SetShift(AnimUtils::GetValue(m_shiftAnim, m_shiftValues));
    SetOpacity(AnimUtils::GetValue(m_alphaAnim, m_alphaValues));
}
