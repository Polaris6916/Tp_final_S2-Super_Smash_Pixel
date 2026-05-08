/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/interaction/ui_button.h"
#include "scene/scene.h"
#include "ui/base/ui_canvas.h"

UIButton::UIButton(Scene *scene, UISelectableGroup *group)
    : UISelectable(scene, group)
    , m_stateObjectMaps()
{
    SetName("UIButton");
    SetFadeChildren(false);
}

UIButton::~UIButton()
{
}

void UIButton::AddLabelObject(UIObject *object)
{
    Label label;
    label.object = object;

    AddLabelObject(object, label);
}

void UIButton::AddLabelObject(UIObject *object, const UIStateModifiers &stateModifiers)
{
    Label label;
    label.object = object;
    label.stateModifiers = stateModifiers;

    AddLabelObject(object, label);
}

void UIButton::RemoveLabelObject(UIObject *object)
{
    auto it = m_labelMap.find(object);
    if (it == m_labelMap.end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_labelMap.erase(it);
}

void UIButton::AddStateObject(State state, UIObject *object, bool fade)
{
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetEnabled(state == GetState());

    int stateIdx = (int)state;

    auto it = m_stateObjectMaps[stateIdx].find(object);
    if (it != m_stateObjectMaps[stateIdx].end())
    {
        it->second = fade;
    }
    else
    {
        m_stateObjectMaps[stateIdx].insert(std::make_pair(object, fade));
    }

    if (fade)
    {
        UIFadeDef fadeDef;
        fadeDef.fadeOpacity = true;
        fadeDef.duration = 0.1f;
        object->SetFadeInAnimation(fadeDef);
        object->SetFadeOutAnimation(fadeDef);
    }
}

void UIButton::RemoveStateObject(State state, UIObject *object)
{
    int stateIdx = (int)state;
    auto it = m_stateObjectMaps[stateIdx].find(object);
    if (it == m_stateObjectMaps[stateIdx].end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_stateObjectMaps[stateIdx].erase(it);
}

void UIButton::Update()
{
    UISelectable::Update();
    if (IsEnabled() == false) return;

    SetVisible(true);
}

void UIButton::OnStateChanged(State state, State prevState, bool makeSound)
{
    UISelectable::OnStateChanged(state, prevState, makeSound);

    for (auto &it : m_labelMap)
    {
        UIObject *object = it.first;
        Label &label = it.second;

        label.stateModifiers.SetObjectTarget(object, state);
        object->PlayTransformToTarget();
    }

    std::map<UIObject *, bool> *m_stateMap = &m_stateObjectMaps[(int)state];
    for (auto it = m_stateMap->begin(); it != m_stateMap->end(); ++it)
    {
        UIObject *object = it->first;
        bool fade = it->second;

        if (fade)
        {
            object->PlayFadeIn();
        }
        else
        {
            object->SetEnabled(true);
        }
    }

    m_stateMap = &m_stateObjectMaps[(int)prevState];
    for (auto it = m_stateMap->begin(); it != m_stateMap->end(); ++it)
    {
        UIObject *object = it->first;
        bool fade = it->second;

        if (fade)
        {
            object->PlayFadeOut();
        }
        else
        {
            object->SetEnabled(false);
        }
    }
}

void UIButton::DrawImGui()
{
    UISelectable::DrawImGui();

    std::shared_ptr<ImGuiManagerBase> managerPtr = m_scene->GetImGuiManager();
    ImGuiManagerBase *imGuiManager = managerPtr.get();
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("UIButton"))
    {
        if (ImGui::BeginTable("UIButton Members", 2, ImGuiTableFlags_RowBg))
        {
            if (ImGuiUtils::BeginSection("Labels"))
            {
                for (auto it = m_labelMap.begin(); it != m_labelMap.end(); ++it)
                {
                    UIObject *uiObject = it->first;
                    ImGuiUtils::Member("Label", uiObject, imGuiManager);
                }
                ImGuiUtils::EndSection();
            }
            if (ImGuiUtils::BeginSection("State objects"))
            {
                for (int i = 0; i < 4; i++)
                {
                    std::string stateStr = std::string("State ") + ENUM_STRING(static_cast<State>(i));
                    auto &map = m_stateObjectMaps[i];
                    for (auto it = map.begin(); it != map.end(); ++it)
                    {
                        UIObject *uiObject = it->first;
                        ImGuiUtils::Member(stateStr.c_str(), uiObject, imGuiManager);
                    }
                }
                ImGuiUtils::EndSection();
            }
            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}

void UIButton::AddLabelObject(UIObject *object, Label &label)
{
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetTransformDuration(0.1f);

    auto it = m_labelMap.find(object);
    if (it != m_labelMap.end())
    {
        it->second = label;
    }
    else
    {
        m_labelMap.insert(std::make_pair(object, label));
    }

    label.stateModifiers.SetObjectTarget(object, GetState());
    object->TransformToTarget();
}
