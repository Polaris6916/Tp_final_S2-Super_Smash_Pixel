/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"

#include "imgui/imgui_component_base.h"
#include "ecs/basic_components.h"

class Scene;
class ImGuiTransform : public ImGuiBaseComponent
{
public:
    ImGuiTransform()
        : ImGuiBaseComponent("Transform")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<Transform>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiFixedUpdateTransform : public ImGuiBaseComponent
{
public:
    ImGuiFixedUpdateTransform()
        : ImGuiBaseComponent("Fixed update transform")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<FixedUpdateTransform>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiLocalTransform : public ImGuiBaseComponent
{
public:
    ImGuiLocalTransform()
        : ImGuiBaseComponent("Local transform")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<LocalTransform>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiRigidbody : public ImGuiBaseComponent
{
public:
    ImGuiRigidbody()
        : ImGuiBaseComponent("Rigid body")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<Rigidbody>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;

private:
    void DrawShapes(b2BodyId bodyId);
    void DrawPolygonShape(b2ShapeId shapeId);
    void DrawCircleShape(b2ShapeId shapeId);
    void DrawCapsuleShape(b2ShapeId shapeId);
    void DrawSegmentShape(b2ShapeId shapeId);
    void DrawChainSegmentShape(b2ShapeId shapeId);
};

class ImGuiRenderSortingLayer : public ImGuiBaseComponent
{
public:
    ImGuiRenderSortingLayer()
        : ImGuiBaseComponent("Render sorting layer")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<RenderSortingLayer>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiRenderBlendMod : public ImGuiBaseComponent
{
public:
    ImGuiRenderBlendMod()
        : ImGuiBaseComponent("Render blend modulation")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<RenderBlendMod>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiRenderColorMod : public ImGuiBaseComponent
{
public:
    ImGuiRenderColorMod()
        : ImGuiBaseComponent("Render color modulation")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<RenderColorMod>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiSprite : public ImGuiBaseComponent
{
public:
    ImGuiSprite()
        : ImGuiBaseComponent("Sprite")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<Sprite>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiTiledSprite : public ImGuiBaseComponent
{
public:
    ImGuiTiledSprite()
        : ImGuiBaseComponent("Tiled sprite")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<TiledSprite>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiTilemapRenderer : public ImGuiBaseComponent
{
public:
    ImGuiTilemapRenderer()
        : ImGuiBaseComponent("Tilemap Renderer")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<TilemapRenderer>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiBackgroundLayer : public ImGuiBaseComponent
{
public:
    ImGuiBackgroundLayer()
        : ImGuiBaseComponent("Background layer")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<BackgroundLayer>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

class ImGuiSpriteAnimState : public ImGuiBaseComponent
{
public:
    ImGuiSpriteAnimState(Scene *scene)
        : ImGuiBaseComponent("Sprite animamtion state")
        , m_scene(scene)
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<SpriteAnimState>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;

protected:
    Scene *m_scene;
};

class ImGuiCamera : public ImGuiBaseComponent
{
public:
    ImGuiCamera()
        : ImGuiBaseComponent("Camera")
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<Camera>(entity);
    }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
};

//class ImGuiParentComponent : public ImGuiBaseComponent
//{
//public:
//    ImGuiParentComponent(ImGuiManager *imGuiManager)
//        : ImGuiBaseComponent("Parent")
//        , m_imGuiManager(imGuiManager)
//    {}
//
//    virtual bool Has(entt::registry &registry, entt::entity entity) override
//    {
//        return registry.all_of<ParentComponent>(entity);
//    }
//    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
//
//private:
//    ImGuiManager *m_imGuiManager;
//};
//
//class ImGuiChildrenComponent : public ImGuiBaseComponent
//{
//public:
//    ImGuiChildrenComponent(ImGuiManager *imGuiManager)
//        : ImGuiBaseComponent("Children")
//        , m_imGuiManager(imGuiManager)
//    {}
//
//    virtual bool Has(entt::registry &registry, entt::entity entity) override
//    {
//        return registry.all_of<ChildrenComponent>(entity);
//    }
//    virtual void DrawImGui(entt::registry &registry, entt::entity entity) override;
//
//private:
//    ImGuiManager *m_imGuiManager;
//};
