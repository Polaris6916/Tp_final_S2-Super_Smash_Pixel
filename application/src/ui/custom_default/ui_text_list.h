/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

class UITextList : public UIItemList
{
public:
    UITextList(
        Scene *scene,
        const std::string &labelString,
        const std::vector<std::string> &textStrings,
        Color color,
        float textWidth,
        UISelectableGroup *group = nullptr
    );

private:
    std::vector<std::string> m_texts;
};
