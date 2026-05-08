/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "scene_manager/title_manager.h"
#include "input/application_input.h"

#include "imgui/imgui_manager.h"
#include "imgui/imgui_components.h"

TitleManager::TitleManager(InputManager *inputManager)
    : SceneManager(inputManager)
    , m_titlePage(nullptr)
    , m_stagePage(nullptr)
{
    Scene *scene = GetScene();
    entt::registry &registry = scene->GetRegistry();
    SpriteAnimManager *animManager = scene->GetAnimManager();
    animManager->SetAnimIDToString(assets::AnimIDToString);

    AssetManager *assets = scene->GetAssetManager();
    assets::InitAssets(assets);
    assets::InitTextures(assets);
    assets::InitFonts(assets);
    assets::InitMusic(assets);
    assets::InitSFX(assets);
    assets::InitSpriteAnimations(scene);

    std::shared_ptr<ImGuiManager> imGuiManager = std::make_shared<ImGuiManager>(scene);
    scene->SetImGuiManager(imGuiManager);
    imGuiManager->SetFlags(IM_GUI_UI_EXPLORER);

    //--------------------------------------------------------------------------
    // Presentation Systems

    scene->AddPresentationSystem(std::make_shared<RenderGameSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderUISystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderQuerySystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderPhysicsSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderGridSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderImGuiSystem>(scene, imGuiManager));

    //--------------------------------------------------------------------------
    // Création du menu

    // Fond
    m_background = new UITitleBackground(scene);

    // Caméra
    CameraCommand::Create(registry, registry.create(), scene, b2Vec2_zero);

    // Première page
    OpenPage(Page::TITLE);

    // Musique du menu
    scene->GetAssetManager()->FadeInMusic(MUSIC_MENU);
}

TitleManager::~TitleManager()
{
}

void TitleManager::OnSceneUpdate()
{
    Scene *scene = GetScene();
    game::UpdateFontSize(scene);
}

void TitleManager::OpenPage(Page page)
{
    Scene *scene = GetScene();
    UIObject *uiPage = nullptr;
    switch (page)
    {
    case Page::TITLE:
        if (m_titlePage) m_titlePage->Delete();
        m_titlePage = new UITitlePage(scene);
        uiPage = m_titlePage;
        break;

    case Page::STAGE:
        if (m_stagePage) m_stagePage->Delete();
        m_stagePage = new UIStagePage(scene);
        uiPage = m_stagePage;
        break;

    default:
        assert(false);
        return;
    }

    uiPage->SetParent(scene->GetCanvas());
    uiPage->PlayFadeIn();
}

void TitleManager::QuitPage(Page page)
{
    UIObject *uiPage = nullptr;
    switch (page)
    {
    case Page::TITLE:
        uiPage = m_titlePage;
        m_titlePage = nullptr;
        break;

    case Page::STAGE:
        uiPage = m_stagePage;
        m_stagePage = nullptr;
        break;

    default:
        assert(false);
        return;
    }
    if (uiPage) uiPage->Delete();
}

TitleManager *TitleManager::GetFromScene(Scene *scene)
{
    if (scene == nullptr)
    {
        assert(false);
        return nullptr;
    }
    return dynamic_cast<TitleManager *>(scene->GetSceneManager());
}
