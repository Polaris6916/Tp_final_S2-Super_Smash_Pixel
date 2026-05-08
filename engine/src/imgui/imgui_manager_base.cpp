/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui/imgui_manager_base.h"
#include "scene/scene.h"
#include "ui/ui_object.h"
#include "ui/base/ui_canvas.h"

ImGuiManagerBase::ImGuiManagerBase(Scene *scene)
    : m_scene(scene)
    , m_registry(scene->GetRegistry())
    , m_selectedEntity{ entt::null }
    , m_showImGui(false)
    , m_showEntityInspector(true)
    , m_showEntityFilter(false)
    , m_showSystems(false)
    , m_selectedUI(nullptr)
    , m_showUIExplorer(false)
{
}

void ImGuiManagerBase::AddComponent(std::unique_ptr<ImGuiBaseComponent> component)
{
    assert(component != nullptr);
    assert(component->manager == nullptr);
    component->manager = this;
    m_imGuiComponents.emplace_back(std::move(component), false);
}

void ImGuiManagerBase::AddTag(std::unique_ptr<ImGuiBaseTag> tag)
{
    assert(tag != nullptr);
    m_imGuiTags.emplace_back(std::move(tag), false);
}

void ImGuiManagerBase::RenderEntityInspector()
{
    ImGui::Begin("Entity Inspector", &m_showEntityInspector, ImGuiWindowFlags_NoCollapse);
    if (m_registry.valid(m_selectedEntity))
    {
        ImGui::PushID(static_cast<int>(m_selectedEntity));
        ImGui::SeparatorText(GetEntityName(m_registry, m_selectedEntity).c_str());

        bool hasTag = false;
        for (auto &[tag, selected] : m_imGuiTags)
        {
            if (tag->Has(m_registry, m_selectedEntity))
            {
                hasTag = true;
                break;
            }
        }
        if (hasTag == true)
        {
            if (ImGui::CollapsingHeader("Tags"))
            {
                for (auto &[tag, selected] : m_imGuiTags)
                {
                    if (tag->Has(m_registry, m_selectedEntity) == false) continue;
                    ImGui::BulletText(tag->name.c_str());
                }
            }
        }
        for (auto &[component, selected] : m_imGuiComponents)
        {
            if (component->Has(m_registry, m_selectedEntity) == false) continue;

            if (ImGui::CollapsingHeader(component->name.c_str()))
            {
                component->DrawImGui(m_registry, m_selectedEntity);
            }
        }
        ImGui::PopID();
    }
    else
    {
        m_selectedEntity = entt::null;
        ImGui::TextUnformatted("No entity selected");
    }

    ImGui::End();
}

void ImGuiManagerBase::RenderUIExplorer()
{
    ImGui::Begin("UI Explorer", &m_showUIExplorer);
    UIObject *uiObject = dynamic_cast<UIObject *>(m_scene->GetCanvas());
    if (ImGui::BeginChild(
        "UI hierarchy", ImVec2(200, 0),
        ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
    {
        if (ImGui::BeginTable("DamageableComponent Members", 1, ImGuiTableFlags_RowBg))
        {
            RenderUIExplorerRec(uiObject);
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginGroup();
    if (m_scene->Contains(m_selectedUI))
    {
        m_selectedUI->DrawImGui();
    }
    else
    {
        m_selectedUI = nullptr;
        ImGui::TextUnformatted("No UIObject selected");
    }
    ImGui::EndGroup();
    ImGui::End();
}

void ImGuiManagerBase::RenderUIExplorerRec(UIObject *uiOject)
{
    if (uiOject == nullptr) return;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushID(uiOject);

    uiOject->SetGizmosEnabled(false);

    ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
    tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_flags |= ImGuiTreeNodeFlags_SpanFullWidth;

    if (uiOject == m_selectedUI)
    {
        tree_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (uiOject->GetChildren().size() == 0)
    {
        tree_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    }

    bool nodeOpen = ImGui::TreeNodeEx(GetUIName(uiOject).c_str(), tree_flags);
    if (ImGui::IsItemFocused())
    {
        m_selectedUI = uiOject;
    }
    if (ImGui::IsItemHovered()) uiOject->SetGizmosEnabled(true);
    if (nodeOpen)
    {
        for (UIObject *child : uiOject->GetChildren())
        {
            RenderUIExplorerRec(child);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ImGuiManagerBase::RenderFilter()
{
    ImGui::Begin("Entity filter", &m_showEntityFilter);

    ImGuiStyle &style = ImGui::GetStyle();
    float childWidth = (ImGui::GetContentRegionAvail().x - 1 * style.ItemSpacing.x) / 2;

    ImGui::BeginGroup();
    ImGui::TextUnformatted("Filters");
    ImGui::BeginChild("Filters", ImVec2(childWidth, 0), ImGuiChildFlags_Borders);
    {
        for (auto &[component, selected] : m_imGuiComponents)
        {
            ImGui::Checkbox(component->name.c_str(), &selected);
        }
    }
    ImGui::EndChild();
    ImGui::EndGroup();

    auto view = m_registry.view<NameComponent>();
    std::set<entt::entity> matchSet;
    for (entt::entity entity : view)
    {
        bool match = true;
        for (auto &[component, selected] : m_imGuiComponents)
        {
            if (selected)
            {
                if (component->Has(m_registry, entity) == false)
                {
                    match = false;
                    break;
                }
            }
        }
        if (match) matchSet.insert(entity);
    }

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Matching entities");
    ImGui::BeginChild("Results", ImVec2(childWidth, 0), ImGuiChildFlags_Borders);
    for (entt::entity entity : matchSet)
    {
        if (m_registry.valid(entity) == false) continue;

        ImGui::PushID(static_cast<int>(entt::to_integral(entity)));
        if (ImGui::Selectable(GetEntityName(m_registry, entity).c_str(), entity == m_selectedEntity))
        {
            SetSelected(entity);
        }
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::EndGroup();
    ImGui::End();
}

void ImGuiManagerBase::RenderSystems()
{
    ImGui::Begin("Systems", &m_showSystems);
    if (ImGui::BeginTabBar("SystemBar"))
    {
        if (ImGui::BeginTabItem("Simulation"))
        {
            for (auto &system : m_scene->GetSimulationSystems())
            {
                ImGui::Checkbox(system->name.c_str(), &(system->enabled));
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Presentation"))
        {
            for (auto &system : m_scene->GetPresentationSystems())
            {
                if (dynamic_pointer_cast<RenderImGuiSystem>(system) == nullptr)
                {
                    ImGui::Checkbox(system->name.c_str(), &(system->enabled));
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

std::string ImGuiManagerBase::GetEntityName(entt::registry &registry, entt::entity entity)
{
    if (entity == entt::null)
    {
        return std::string("entt::null");
    }
    if (registry.valid(entity) == false)
    {
        return std::string("Invalid entity");
    }
    if (registry.all_of<NameComponent>(entity))
    {
        NameComponent &nameComp = registry.get<NameComponent>(entity);
        return nameComp.name
            + " [" + std::to_string(entt::to_entity(entity))
            + ", v" + std::to_string(entt::to_version(entity)) + "]";
    }
    return std::string("Unamed [") + std::to_string(entt::to_integral(entity)) + "]";
}

std::string ImGuiManagerBase::GetUIName(UIObject *uiObject)
{
    return uiObject->GetName() + " [" + std::to_string(uiObject->GetID()) + "]";
}

void ImGuiManagerBase::SetSelected(entt::entity entity)
{
    if (m_registry.valid(entity))
    {
        m_showEntityInspector = true;
        m_selectedEntity = entity;
    }
}

void ImGuiManagerBase::SetSelected(UIObject *uiObject)
{
    if (m_scene->Contains(uiObject))
    {
        m_selectedUI = uiObject;
    }
}

void ImGuiManagerBase::Update()
{
    if (m_registry.valid(m_selectedEntity) == false)
    {
        m_selectedEntity = entt::null;
    }
    if (m_scene->Contains(m_selectedUI) == false)
    {
        m_selectedUI = dynamic_cast<UIObject *>(m_scene->GetCanvas());
    }
}

void ImGuiManagerBase::Render()
{
}
