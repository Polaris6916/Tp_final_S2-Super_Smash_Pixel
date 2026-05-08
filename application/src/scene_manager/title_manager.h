/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

#include "ui/title/ui_title_page.h"
#include "ui/title/ui_title_background.h"
#include "ui/title/ui_stage_page.h"

class TitleManager : public SceneManager
{
public:
    TitleManager(InputManager *inputManager);
    virtual ~TitleManager();

    enum class Page
    {
        TITLE = 0, STAGE
    };

    void OpenPage(Page page);
    void OpenPage(int pageID);
    void QuitPage(Page page);
    void QuitPage(int pageID);

    virtual void OnSceneUpdate() override;

    static TitleManager *GetFromScene(Scene *scene);

private:
    UITitlePage *m_titlePage;
    UIStagePage *m_stagePage;

    UITitleBackground *m_background;
};

inline void TitleManager::OpenPage(int pageID)
{
    OpenPage(static_cast<Page>(pageID));
}

inline void TitleManager::QuitPage(int pageID)
{
    QuitPage(static_cast<Page>(pageID));
}
