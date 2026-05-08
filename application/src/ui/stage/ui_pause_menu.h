/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ui/custom_default/ui_default_button.h"

class UIPauseMenu : public UIObject, public UISelectableListener
{
public:
    UIPauseMenu(Scene *scene);

    virtual void OnClick(UISelectable *which) override;

protected:
    virtual void OnPlayFadeIn() override;
    virtual void OnPlayFadeOut() override;
    virtual void OnFadeOutEnd(UIObject *which) override;

private:
    UIText *m_titleText;
    UIFillRect *m_pauseFill;
    UISelectableGroup *m_group;

    UIDefaultButton *m_continueButton;
    UIDefaultButton *m_mainMenuButton;

    void InitFadeAnim();
};