/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

class UIDefaultButton : public UIButton
{
public:
    UIDefaultButton(
        Scene *scene,
        const std::string &labelString,
        UISelectableGroup *group = nullptr
    );

private:
    std::vector<UIImage *> m_backgrounds;
};
