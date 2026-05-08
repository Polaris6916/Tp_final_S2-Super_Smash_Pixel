/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/common/camera.h"
#include "scene_manager/stage_manager.h"
#include "ecs/player/player_components.h"

CameraFollow::CameraFollow(b2Vec2 targetIn)
    : target(targetIn)
    , velocity(b2Vec2_zero)
{
}

void CameraCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position);
}

void CameraCommand::Create(entt::registry &registry, entt::entity entity, Scene *scene, b2Vec2 position)
{
    registry.emplace<NameComponent>(entity, "Camera");
    registry.emplace<Transform>(entity, position);
    registry.emplace<Camera>(entity, b2Vec2{ 24.f, 24.f * 9.f / 16.f });
    registry.emplace<CameraFollow>(entity, position);
}

void CameraSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto cameraView = m_registry.view<Camera, CameraFollow, Transform>();
    for (auto [cameraEntity, camera, follow, cameraTransform] : cameraView.each())
    {
        if (camera.isActive == false) continue;

        if (useDebugCamera == false)
        {
            UpdateMainCamera(camera, follow, cameraTransform);
        }
        else
        {
            UpdateDebugCamera(camera, cameraTransform);
        }
    }
}

b2AABB CameraSystem::ComputePlayersBox()
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    AssertNew(stageManager);

    b2AABB playersBox = { 0 };
    playersBox.lowerBound = b2Vec2{ +INFINITY, +INFINITY };
    playersBox.upperBound = b2Vec2{ -INFINITY, -INFINITY };

    auto view = m_registry.view<
        const PlayerController,
        const Transform
    >();
    for (auto [entity, controller, transform] : view.each())
    {
        playersBox.lowerBound = b2Min(transform.position, playersBox.lowerBound);
        playersBox.upperBound = b2Max(transform.position, playersBox.upperBound);
    }

    playersBox.upperBound.y += 1.5f;

    return playersBox;
}

void CameraSystem::UpdateMainCamera(
    Camera &camera, CameraFollow &follow, Transform &transform)
{
    b2AABB playersBox = ComputePlayersBox();

    camera.viewScale = 0.5f;
    follow.target = b2AABB_Center(playersBox) + b2Vec2{ 0.f, 1.5f };

    const float smoothTimeX = 0.2f;
    const float smoothTimeY = 0.4f;

    transform.position.x = math::SmoothDamp(
        transform.position.x, follow.target.x, follow.velocity.x,
        smoothTimeX, 100.f, m_scene->GetDelta()
    );
    transform.position.y = math::SmoothDamp(
        transform.position.y, follow.target.y, follow.velocity.y,
        smoothTimeX, 100.f, m_scene->GetDelta()
    );
}

void CameraSystem::UpdateDebugCamera(Camera &camera, Transform &transform)
{
    InputManager *inputManager = m_scene->GetInputManager();
    MouseInput *mouse = MouseInput::GetFromManager(inputManager);

    b2Vec2 currMousePos = camera.ViewToWorld(transform.position, mouse->viewPos.x, mouse->viewPos.y);
    b2Vec2 prevMousePos = camera.ViewToWorld(transform.position, mouse->lastViewPos.x, mouse->lastViewPos.y);

    if (mouse->middleClick)
    {
        camera.viewScale = 1.f;
    }
    else if (mouse->wheel > 0)
    {
        camera.viewScale *= 0.8f;
        camera.viewScale = math::Clamp(camera.viewScale, 0.05f, 3.f);
    }
    else if (mouse->wheel < 0)
    {
        camera.viewScale /= 0.8f;
        camera.viewScale = math::Clamp(camera.viewScale, 0.05f, 3.f);
    }

    if (mouse->leftDown)
    {
        transform.position += prevMousePos - currMousePos;
    }
}
