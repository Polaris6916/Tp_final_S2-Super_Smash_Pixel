/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui/imgui_component_base.h"
#include "imgui/imgui_manager_base.h"
#include "ui/ui_object.h"

void ImGuiUtils::Member(const char *name, entt::entity entity, entt::registry &registry, ImGuiManagerBase *manager)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn();
    const std::string entityName = ImGuiManagerBase::GetEntityName(registry, entity);
    if (manager == nullptr || registry.valid(entity) == false)
    {
        ImGui::TextUnformatted(entityName.c_str());
    }
    else
    {
        ImGui::PushID(static_cast<int>(entt::to_integral(entity)));
        if (ImGui::Selectable(entityName.c_str(), false))
        {
            manager->SetSelected(entity);
        }
        ImGui::PopID();
    }
}

void ImGuiUtils::MemberBits(const char *name, uint64_t value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("%08X", value);
}

void ImGuiUtils::Member(const char *name, UIObject *uiObject, ImGuiManagerBase *manager)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn();
    if (manager == nullptr || uiObject == nullptr)
    {
        ImGui::TextUnformatted("nullptr");
    }
    else
    {
        ImGui::PushID(uiObject);
        if (ImGui::Selectable(uiObject->GetName().c_str(), false))
        {
            manager->SetSelected(uiObject);
        }
        ImGui::PopID();
    }
}

void ImGuiUtils::Member(const char *name, int value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("%d", value);
}

void ImGuiUtils::Member(const char *name, size_t value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("%d", static_cast<int>(value));
}

void ImGuiUtils::Member(const char *name, float value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("%+.2f", value);
}

void ImGuiUtils::Member(const char *name, bool value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("%s", value ? "true" : "false");
}

void ImGuiUtils::Member(const char *name, b2Vec2 value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::Text("(%+.2f,%+.2f)", value.x, value.y);
}

void ImGuiUtils::Member(const char *name, const std::string &value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::TextUnformatted(value.c_str());
}

void ImGuiUtils::Member(const char *name, const char *value)
{
    ImGui::TableNextColumn(); ImGui::Text(name);
    ImGui::TableNextColumn(); ImGui::TextUnformatted(value);
}


bool ImGuiUtils::BeginSection(const char *name)
{
    ImGui::TableNextColumn();
    bool isOpened = ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_SpanAllColumns);
    ImGui::TableNextColumn();
    return isOpened;
}

void ImGuiUtils::EndSection()
{
    ImGui::TreePop();
}
