/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/custom_default/ui_default_button.h"

UIDefaultButton::UIDefaultButton(
    Scene *scene, const std::string &labelString, UISelectableGroup *group
)
    : UIButton(scene, group)
    , m_backgrounds{}
{
    SetName("Button");

    AssetManager *assets = m_scene->GetAssetManager();
    TTF_Font *font = assets->GetFont(FONT_NORMAL);
    UIText *text = nullptr;

    UIStateModifiers stateModifier;
    stateModifier.alphaMap[UISelectable::State::NORMAL] = 0.7f;
    stateModifier.alphaMap[UISelectable::State::SELECTED] = 1.0f;
    stateModifier.alphaMap[UISelectable::State::PRESSED] = 0.5f;

    text = new UIText(m_scene, labelString, font, Colors::White);
    AddLabelObject(text, stateModifier);

    GetLocalCursorRect().anchorMin = { 0.f, 0.5f };
    GetLocalCursorRect().anchorMax = { 0.f, 0.5f };
    GetLocalCursorRect().offsetMin = { -15.f, -5.f };
    GetLocalCursorRect().offsetMax = { -5.f, +5.f };

    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Button");
    AssertNew(spriteGroup);
    
    UISelectable::State states[3] = {
        UISelectable::State::NORMAL,
        UISelectable::State::SELECTED,
        UISelectable::State::PRESSED
    };
    for (UISelectable::State state : states)
    {
        UIImage *buttonBack = new UIImage(scene, spriteGroup, (int)state);
        buttonBack->SetLayer(LAYER_UI_BACKGROUND);
        buttonBack->SetBorders(6, 6, 6, 6, 4.f);
        buttonBack->SetOpacity(0.5f);
        AddStateObject(state, buttonBack);
        m_backgrounds.push_back(buttonBack);
    }

    SetSoundOnClick(SFX_UI_CLICK);
    SetSoundOnSelected(SFX_UI_SELECTED);
}
