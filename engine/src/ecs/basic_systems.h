/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "ecs/command_buffer.h"
#include "ecs/basic_components.h"
#include "imgui/imgui_manager_base.h"

class Scene;

class System
{
public:
    System(Scene *scene, const std::string &systemName);

    virtual ~System() = default;

    virtual void OnCreate() {};
    virtual void OnDestroy() {};

    virtual void OnUpdate(EntityCommandBuffer &ecb) {};
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) {};

    const std::string name;
    bool enabled;

protected:
    Scene *m_scene;
    entt::registry &m_registry;
};

class RenderGameSystem : public System
{
public:
    RenderGameSystem(Scene *scene) : System(scene, "Render game system") {}
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

private:
    void RenderSprite(
        const Camera &camera, const Transform &cameraTransform,
        const Sprite &sprite, const Transform &spriteTransform,
        float scale = 1.f, int tiledCountX = 1, int tiledCountY = 1);
    void RenderLayer(
        const Camera &camera, const Transform &cameraTransform,
        const BackgroundLayer &layer, const Transform &layerTransform);

    void FillLayerAbove(SDL_Texture *layer, SDL_FRect dstRect);
    void FillLayerBelow(SDL_Texture *layer, SDL_FRect dstRect);
};

class RenderUISystem : public System
{
public:
    RenderUISystem(Scene *scene) : System(scene, "Render UI system") {}
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};

class TransformSystem : public System
{
public:
    TransformSystem(Scene *scene) : System(scene, "Transform system") {}
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
};

class WorldBoundsSystem : public System
{
public:
    WorldBoundsSystem(
        Scene *scene,
        b2AABB worldBounds = b2AABB{ b2Vec2{ -100.f, -100.f }, b2Vec2{ +100.f, +100.f } }
    )
        : System(scene, "World bounds system")
        , bounds(worldBounds)
    {}
    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;

    b2AABB bounds;
};

class AnimatorSystem : public System
{
public:
    AnimatorSystem(Scene *scene) : System(scene, "Animator system") {}
    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};

class RenderQuerySystem : public System
{
public:
    RenderQuerySystem(Scene *scene) : System(scene, "Render Queries system")
    {
        enabled = false;
    }

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};

class RenderPhysicsSystem : public System
{
public:
    RenderPhysicsSystem(Scene *scene) : System(scene, "Render physics system")
    {
        enabled = false;
    }

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

    struct CallbackContext
    {
        entt::registry &registry;
        Camera &camera;
        Transform &cameraTransform;
    };

    static bool DrawQueryCallback(b2ShapeId shapeId, void *context);
};

class RenderGridSystem : public System
{
public:
    RenderGridSystem(Scene *scene) : System(scene, "Render grid system")
    {
        enabled = false;
    }

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};

class ImGuiManagerBase;
class RenderImGuiSystem : public System
{
public:
    RenderImGuiSystem(Scene *scene, std::shared_ptr<ImGuiManagerBase> manager)
        : System(scene, "Render ImGui system")
        , m_manager(manager)
    {}

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;

protected:
    std::shared_ptr<ImGuiManagerBase> m_manager;
};


//class HierarchySystem : public System
//{
//public:
//    HierarchySystem(Scene *scene) : System(scene, "Hierarchy system") {}
//    virtual void OnFixedUpdate(EntityCommandBuffer &ecb) override;
//    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
//};