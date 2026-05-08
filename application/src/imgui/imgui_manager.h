/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_assets.h"

#include "imgui/imgui_components.h"

enum ImGuiManagerFlag : uint32_t
{
    IM_GUI_PHYSICS = 0x01,
    IM_GUI_QUERY = 0x2,
    IM_GUI_GRID = 0x4,
    IM_GUI_ENTITY_INSPECTOR = 0x8,
    IM_GUI_ENTITY_FILTER = 0x10,
    IM_GUI_DEBUG = 0x20,
    IM_GUI_UI_EXPLORER = 0x40,
};

class ImGuiManager : public ImGuiManagerBase
{
public:
    ImGuiManager(Scene *scene, uint32_t flags = 0);
    virtual void Update() override;
    virtual void Render() override;

    void SetFlags(uint32_t flags);

private:
    void RenderDebug();
    void RenderDrawShape();
    void RenderGameCommon();
    void UpdateDrawShape(Camera &camera, Transform &cameraTransform);

    uint32_t m_flags;

    GizmosShape m_gizmosShape;
    std::vector<b2Vec2> m_points;
    b2Vec2 m_shapeExtents;
    int m_selectedDrawMode;

    bool m_showDebug;
    bool m_showCommon;
    bool m_showDrawShape;

    bool m_makeStepClicked;
    bool m_makeStepActive;
    bool m_realtimePressed;

    float m_delayStep;
};
