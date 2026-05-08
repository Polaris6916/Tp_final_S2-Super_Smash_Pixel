/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "imgui/imgui_component_base.h"
#include "utils/gizmos_shape.h"

class Scene;
class UIObject;

class ImGuiManagerBase
{
public:
    ImGuiManagerBase(Scene *scene);
    virtual void Update();
    virtual void Render();

    void AddComponent(std::unique_ptr<ImGuiBaseComponent> component);
    void AddTag(std::unique_ptr<ImGuiBaseTag> tag);

    void SetSelected(entt::entity entity);
    entt::entity GetSelectedEntity() const
    {
        return m_selectedEntity;
    }
    void SetSelected(UIObject *uiObject);
    UIObject *GetSelectedUI() const
    {
        return m_selectedUI;
    }

    static std::string GetEntityName(entt::registry &registry, entt::entity entity);
    static std::string GetUIName(UIObject *uiObject);

    bool ShouldBeRendered() const
    {
        return m_showImGui;
    }

protected:
    void RenderEntityInspector();
    void RenderUIExplorer();
    void RenderUIExplorerRec(UIObject *uiOject);
    void RenderFilter();
    void RenderSystems();

    Scene *m_scene;
    entt::registry &m_registry;
    std::vector<std::pair<std::unique_ptr<ImGuiBaseComponent>, bool>> m_imGuiComponents;
    std::vector<std::pair<std::unique_ptr<ImGuiBaseTag>, bool>> m_imGuiTags;
    entt::entity m_selectedEntity;
    UIObject *m_selectedUI;

    bool m_showImGui;
    bool m_showEntityFilter;
    bool m_showEntityInspector;
    bool m_showSystems;
    bool m_showUIExplorer;
};
