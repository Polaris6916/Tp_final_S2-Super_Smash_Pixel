/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui/imgui_manager.h"
#include "common/game_common.h"
#include "ecs/common/camera.h"

ImGuiManager::ImGuiManager(Scene *scene, uint32_t flags)
    : ImGuiManagerBase(scene)
    , m_delayStep(0.5f)
    , m_showDebug(false)
    , m_showDrawShape(false)
    , m_showCommon(false)
    , m_makeStepClicked(false)
    , m_makeStepActive(false)
    , m_realtimePressed(false)
    , m_flags(flags)
    , m_gizmosShape()
    , m_points()
    , m_selectedDrawMode(0)
    , m_shapeExtents{ 1.f, 1.f }
{
}

void ImGuiManager::RenderDebug()
{
    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    ImGui::Begin("Debug", &m_showDebug);

    ImGui::SeparatorText("Camera");

    for (auto &system : m_scene->GetPresentationSystems())
    {
        if (auto cameraSystem = dynamic_pointer_cast<CameraSystem>(system))
        {
            ImGui::Checkbox("Debug camera", &(cameraSystem->useDebugCamera));
        }
    }

    ImGui::SeparatorText("Scene update");
    ImGui::Button("Make step [Tab]");
    m_makeStepClicked = ImGui::IsItemClicked();
    m_makeStepActive = ImGui::IsItemActive();

    if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP)
    {
        ImGui::SameLine();
        if (ImGui::Button("Realtime [LCtrl]"))
        {
            m_realtimePressed = true;
        }
    }

    ImGui::SeparatorText("Debug draws");

    for (auto &system : m_scene->GetPresentationSystems())
    {
        if (dynamic_pointer_cast<RenderPhysicsSystem>(system))
        {
            ImGui::Checkbox("Render physics", &(system->enabled));
        }
        else if (dynamic_pointer_cast<RenderQuerySystem>(system))
        {
            ImGui::Checkbox("Render queries", &(system->enabled));
        }
        else if (dynamic_pointer_cast<RenderGridSystem>(system))
        {
            ImGui::Checkbox("Render grid", &(system->enabled));
        }
    }

    ImGui::End();
}

void ImGuiManager::RenderDrawShape()
{
    ImGui::Begin("Draw shape", &m_showDrawShape);

    ImGui::SeparatorText("Reference");
    if (ImGui::BeginTable("RenderDrawShape", 2, 0))
    {
        ImGuiUtils::Member("Selected entity", m_selectedEntity, m_registry, this);
        if (m_registry.valid(m_selectedEntity) &&
            m_registry.all_of<Rigidbody>(m_selectedEntity))
        {
            const auto &rigidbody = m_registry.get<Rigidbody>(m_selectedEntity);
            b2Vec2 position = RigidbodyUtils::GetPosition(rigidbody);
            ImGuiUtils::Member("Position", position);
        }
        ImGui::EndTable();
    }

    ImGui::SeparatorText("Mode");
    const char* drawModes[] = { "Circle", "Box", "Polygon" };
    if (ImGui::BeginTable("RenderDrawShapeMode", 3, ImGuiTableFlags_Borders))
    {
        for (int i = 0; i < IM_ARRAYSIZE(drawModes); i++)
        {
            const bool isSelected = (m_selectedDrawMode == i);
            ImGui::TableNextColumn();
            if (ImGui::Selectable(drawModes[i], isSelected))
            {
                m_selectedDrawMode = i;
                m_points.clear();
                m_gizmosShape.Reset();
            }
        }
        ImGui::EndTable();
    }
    ImGui::SeparatorText("Information");

    if (m_points.size() == 0)
    {
        ImGui::TextUnformatted("Left click to add a point");
        ImGui::TextUnformatted("Right click to reset the shape");
    }
    else if (m_selectedDrawMode == 0)
    {
        if (ImGui::BeginTable("GizmosShape", 2, 0))
        {
            ImGuiUtils::Member("Center", m_points[0]);
            ImGuiUtils::Member("Radius", m_shapeExtents.x);
            ImGui::EndTable();
        }
    }
    else if (m_selectedDrawMode == 1)
    {
        if (ImGui::BeginTable("GizmosShape", 2, 0))
        {
            ImGuiUtils::Member("Center", m_points[0]);
            ImGuiUtils::Member("Extents", m_shapeExtents);
            ImGui::EndTable();
        }
    }
    else if (m_selectedDrawMode == 2)
    {
        if (ImGui::BeginTable("GizmosShape", 2, 0))
        {
            for (int i = 0; i < m_points.size(); i++)
            {
                std::string name = "Point " + std::to_string(i);
                ImGuiUtils::Member(name.c_str(), m_points[i]);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void ImGuiManager::RenderGameCommon()
{
    ImGui::Begin("Game common", &m_showCommon);

    ImGui::SeparatorText("Players");
    if (ImGui::BeginTabBar("PlayerTabBar"))
    {
        for (int i = 0; i < g_gameCommon.playerCount; i++)
        {
            std::string label = std::string("Player ") + std::to_string(i);
            if (ImGui::BeginTabItem(label.c_str()))
            {
                PlayerConfig *config = g_gameCommon.GetPlayerConfig(i);
                ImGui::PushID(config);

                if (ImGui::CollapsingHeader("Config"))
                {
                    if (ImGui::BeginTable("PlayerConfig", 2, ImGuiTableFlags_RowBg))
                    {
                        ImGuiUtils::Member("Player type", ENUM_STRING(config->type));
                        ImGuiUtils::Member("Anim category", ENUM_STRING(config->category));
                        ImGuiUtils::Member("Player ID", config->playerID);
                        ImGuiUtils::Member("Team ID", config->teamID);
                        ImGuiUtils::Member("Skin ID", config->skinID);
                        ImGuiUtils::Member("Is KO", config->isKO);
                        ImGuiUtils::Member("Is enabled", config->enabled);
                        ImGuiUtils::Member("Is CPU", config->isCPU);
                        ImGuiUtils::Member("CPU level", config->level);
                        ImGuiUtils::MemberBits("Team mask", config->teamMask);
                        ImGuiUtils::MemberBits("Other team mask", config->otherTeamMask);

                        ImGui::EndTable();
                    }
                }
                ImGui::PopID();

                PlayerStats *stats = g_gameCommon.GetPlayerStats(i);
                ImGui::PushID(stats);
                if (ImGui::CollapsingHeader("Stats"))
                {
                    if (ImGui::BeginTable("PlayerConfig", 2, ImGuiTableFlags_RowBg))
                    {
                        ImGuiUtils::Member("Ejection score", stats->ejectionScore);
                        ImGuiUtils::Member("Fall count", stats->fallCount);
                        ImGuiUtils::Member("KO count", stats->koCount);
                        ImGuiUtils::Member("Damage given", stats->damageGiven);
                        ImGuiUtils::Member("Damage taken", stats->damageTaken);
                        ImGuiUtils::Member("Max speed", stats->maxSpeed);
                        ImGuiUtils::Member("KO time", stats->koTime);

                        ImGui::EndTable();
                    }
                }
                ImGui::PopID();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::SeparatorText("Stage");

    StageConfig *stage = &g_gameCommon.stageConfig;
    if (ImGui::CollapsingHeader("Stage congfig"))
    {
        if (ImGui::BeginTable("StageConfig", 2, ImGuiTableFlags_RowBg))
        {
            ImGuiUtils::Member("Type", ENUM_STRING(stage->type));
            ImGuiUtils::Member("Mode", ENUM_STRING(stage->mode));
            ImGuiUtils::Member("Duration", stage->duration);
            ImGuiUtils::Member("Life count", stage->lifeCount);
            ImGuiUtils::Member("Potion frequency", ENUM_STRING(stage->potionFrequency));
            ImGuiUtils::Member("Bomb frequency", ENUM_STRING(stage->bombsFrequency));

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void ImGuiManager::UpdateDrawShape(Camera &camera, Transform &cameraTransform)
{
    InputManager *inputManager = m_scene->GetInputManager();
    MouseInput *mouse = MouseInput::GetFromManager(inputManager);
    ApplicationInput *application = ApplicationInput::GetFromManager(inputManager);

    b2Vec2 currMousePos = camera.ViewToWorld(
        cameraTransform.position, mouse->viewPos.x, mouse->viewPos.y
    );

    b2Transform xf = b2Transform_identity;
    if (m_registry.valid(m_selectedEntity) &&
        m_registry.all_of<Rigidbody>(m_selectedEntity))
    {
        const auto &rigidbody = m_registry.get<Rigidbody>(m_selectedEntity);
        xf = b2Body_GetTransform(rigidbody.bodyId);
    }
    currMousePos = b2InvTransformPoint(xf, currMousePos);
    currMousePos.x = roundf(10.f * currMousePos.x) / 10.f;
    currMousePos.y = roundf(10.f * currMousePos.y) / 10.f;

    if (mouse->rightClickCount > 0)
    {
        m_points.clear();
        m_gizmosShape.Reset();
    }

    if (m_selectedDrawMode == 0)
    {
        // Circle

        if (application->uiInputs[0].right) m_shapeExtents.x += 0.1f;
        if (application->uiInputs[0].left)  m_shapeExtents.x -= 0.1f;
        if (application->uiInputs[0].up)    m_shapeExtents.x += 0.1f;
        if (application->uiInputs[0].down)  m_shapeExtents.x -= 0.1f;
        m_shapeExtents.x = math::Clamp(m_shapeExtents.x, 0.1f, 3.f);

        b2Circle circle{};
        circle.center = currMousePos;
        circle.radius = m_shapeExtents.x;
        m_gizmosShape.SetAsCircle(circle, xf);

        if (mouse->leftClickCount > 0)
        {
            m_points.clear();
            m_points.emplace_back(currMousePos);
        }
    }
    else if (m_selectedDrawMode == 1)
    {
        // Box

        if (application->uiInputs[0].right) m_shapeExtents.x += 0.1f;
        if (application->uiInputs[0].left)  m_shapeExtents.x -= 0.1f;
        if (application->uiInputs[0].up)    m_shapeExtents.y += 0.1f;
        if (application->uiInputs[0].down)  m_shapeExtents.y -= 0.1f;
        m_shapeExtents.x = math::Clamp(m_shapeExtents.x, 0.1f, 3.f);
        m_shapeExtents.y = math::Clamp(m_shapeExtents.y, 0.1f, 3.f);

        b2Polygon box = b2MakeOffsetBox(m_shapeExtents.x, m_shapeExtents.y, currMousePos, b2Rot_identity);
        m_gizmosShape.SetAsPolygon(box, xf);

        if (mouse->leftClickCount > 0)
        {
            m_points.clear();
            m_points.emplace_back(currMousePos);
        }
    }
    else if (m_selectedDrawMode == 2)
    {
        // Polygon

        if (mouse->leftClickCount > 0)
        {
            m_points.emplace_back(currMousePos);
        }

        const int pointCount = static_cast<int>(m_points.size());
        if (pointCount == 1)
        {
            m_gizmosShape.Set(m_points[0], xf);
        }
        else if (pointCount == 2)
        {
            m_gizmosShape.Set(m_points[0], m_points[1], xf);
        }
        if (2 < pointCount && pointCount < B2_MAX_POLYGON_VERTICES)
        {
            b2Hull hull = b2ComputeHull(m_points.data(), pointCount);
            b2Polygon polygonShape = b2MakePolygon(&hull, 0.f);
            m_gizmosShape.SetAsPolygon(polygonShape, xf);
        }
    }
}

void ImGuiManager::Update()
{
    ImGuiManagerBase::Update();

    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);
    MouseInput *mouse = MouseInput::GetFromManager(inputManager);

    if (applicationInput->debugPressed)
    {
        m_showImGui = !m_showImGui;

        for (auto &system : m_scene->GetPresentationSystems())
        {
            if ((m_flags & IM_GUI_PHYSICS) != 0 &&
                dynamic_pointer_cast<RenderPhysicsSystem>(system))
            {
                system->enabled = m_showImGui;
            }
            if ((m_flags & IM_GUI_QUERY) != 0 &&
                dynamic_pointer_cast<RenderQuerySystem>(system))
            {
                system->enabled = m_showImGui;
            }
            if ((m_flags & IM_GUI_GRID) != 0 &&
                dynamic_pointer_cast<RenderGridSystem>(system))
            {
                system->enabled = m_showImGui;
            }
            if (auto cameraSystem = dynamic_pointer_cast<CameraSystem>(system))
            {
                cameraSystem->useDebugCamera = m_showImGui;
            }
        }

        m_showEntityFilter = (m_flags & IM_GUI_ENTITY_FILTER);
        m_showEntityInspector = (m_flags & IM_GUI_ENTITY_INSPECTOR);
        m_showUIExplorer = (m_flags & IM_GUI_UI_EXPLORER);
        m_showDebug = (m_flags & IM_GUI_DEBUG);
    }

    if (m_showImGui && (mouse->leftClickCount > 1))
    {
        auto cameraView = m_registry.view<Camera, Transform>();
        for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
        {
            if (camera.isActive == false) continue;

            QueryFilter filter;
            filter.solidOnly = false;

            std::vector<OverlapResult> result;
            b2Vec2 position = camera.ViewToWorld(
                cameraTransform.position, mouse->viewPos.x, mouse->viewPos.y);

            m_scene->OverlapCircle(position, 0.2f, filter, result);
            if (result.empty() == false)
            {
                SetSelected(result[0].entity);
                break;
            }
        }
    }

    if (m_showImGui)
    {
        if (m_makeStepClicked || applicationInput->makeStepPressed)
        {
            m_makeStepClicked = false;
            m_scene->SetUpdateMode(Scene::UpdateMode::STEP_BY_STEP);
            m_scene->MakeStep();
            m_delayStep = 0.5f;
        }
        if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP)
        {
            if (m_makeStepActive || applicationInput->makeStepDown)
            {
                m_delayStep -= g_time->GetUnscaledDelta();
                if (m_delayStep < 0.f)
                {
                    m_scene->MakeStep();
                    m_delayStep += m_scene->GetTimeStep();
                }
            }

            if (m_realtimePressed || applicationInput->realtimePressed)
            {
                m_realtimePressed = false;
                m_scene->SetUpdateMode(Scene::UpdateMode::REALTIME);
            }
        }
    }
    else
    {
        m_scene->SetUpdateMode(Scene::UpdateMode::REALTIME);
    }

    if (m_showDrawShape)
    {
        auto cameraView = m_registry.view<Camera, Transform>();
        for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
        {
            if (camera.isActive == false) continue;

            UpdateDrawShape(camera, cameraTransform);
            break;
        }
    }
}

void ImGuiManager::Render()
{
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::BeginMainMenuBar();
    ImGui::Checkbox("Debug", &m_showDebug);
    ImGui::Checkbox("Entity filter", &m_showEntityFilter);
    ImGui::Checkbox("Entity inspector", &m_showEntityInspector);
    ImGui::Checkbox("Systems", &m_showSystems);
    ImGui::Checkbox("Common", &m_showCommon);
    ImGui::Checkbox("UI explorer", &m_showUIExplorer);
    ImGui::Checkbox("Draw shape", &m_showDrawShape);
    ImGui::EndMainMenuBar();

    if (m_showDebug) RenderDebug();
    if (m_showEntityFilter) RenderFilter();
    if (m_showEntityInspector) RenderEntityInspector();
    if (m_showSystems) RenderSystems();
    if (m_showCommon) RenderGameCommon();
    if (m_showUIExplorer) RenderUIExplorer();
    if (m_showDrawShape) RenderDrawShape();

    ImGui::EndFrame();

    if (m_showDrawShape)
    {
        auto cameraView = m_registry.view<Camera, Transform>();
        for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
        {
            if (camera.isActive == false) continue;

            SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
            m_gizmosShape.Render(g_renderer, camera, cameraTransform);
            break;
        }
    }
}

void ImGuiManager::SetFlags(uint32_t flags)
{
    m_flags = flags;
}
