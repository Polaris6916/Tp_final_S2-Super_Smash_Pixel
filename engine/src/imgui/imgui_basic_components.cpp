/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui/imgui_basic_components.h"
#include "imgui/imgui_manager_base.h"
#include "utils/utils.h"
#include "scene/scene.h"

constexpr ImGuiTableFlags membersTableFlags = ImGuiTableFlags_RowBg;

void ImGuiTransform::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<Transform>(entity) == false) return;
    auto &transform = registry.get<Transform>(entity);

    if (ImGui::BeginTable("TransformComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Position", transform.position);
        ImGuiUtils::Member("Angle", transform.angle);

        ImGui::EndTable();
    }
}

void ImGuiFixedUpdateTransform::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<FixedUpdateTransform>(entity) == false) return;
    auto &fixed = registry.get<FixedUpdateTransform>(entity);

    if (ImGui::BeginTable("FixedUpdateTransformComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Curr position", fixed.currXf.p);
        ImGuiUtils::Member("Prev position", fixed.prevXf.p);
        ImGuiUtils::Member("Curr angle", b2Rot_GetAngle(fixed.currXf.q) / B2_PI * 180.f);
        ImGuiUtils::Member("Prev angle", b2Rot_GetAngle(fixed.prevXf.q) / B2_PI * 180.f);

        ImGui::EndTable();
    }
}

void ImGuiLocalTransform::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<LocalTransform>(entity) == false) return;
    auto &local = registry.get<LocalTransform>(entity);

    if (ImGui::BeginTable("LocalTransformComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Parent", local.parent, registry, manager);
        ImGuiUtils::Member("Position", local.position);
        ImGuiUtils::Member("Angle", local.angle);

        ImGui::EndTable();
    }
}

void ImGuiRenderSortingLayer::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<RenderSortingLayer>(entity) == false) return;
    auto &layer = registry.get<RenderSortingLayer>(entity);

    if (ImGui::BeginTable("RenderLayerComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Enabled", layer.enabled);
        ImGuiUtils::Member("Layer", layer.layer);
        ImGuiUtils::Member("Order in layer", layer.orderInLayer);

        ImGui::EndTable();
    }
}

void ImGuiRigidbody::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<Rigidbody>(entity) == false) return;
    auto &rigidbody = registry.get<Rigidbody>(entity);

    if (ImGui::BeginTable("RigidbodyComponent Members", 2, membersTableFlags))
    {
        b2BodyId bodyId = rigidbody.bodyId;
        if (B2_IS_NULL(bodyId) || b2Body_IsValid(bodyId) == false)
        {
            ImGuiUtils::Member("ID", "INVALID");
        }
        else
        {
            ImGuiUtils::Member("Is awake", b2Body_IsAwake(bodyId));
            ImGuiUtils::Member("Is bullet", b2Body_IsBullet(bodyId));
            ImGuiUtils::Member("Is enabled", b2Body_IsEnabled(bodyId));
            ImGuiUtils::Member("Body type", ENUM_STRING(b2Body_GetType(bodyId)));
            ImGuiUtils::Member("Position", b2Body_GetPosition(bodyId));
            ImGuiUtils::Member("Velocity", b2Body_GetLinearVelocity(bodyId));
            ImGuiUtils::Member("Angular velocity", b2Body_GetAngularVelocity(bodyId));
            ImGuiUtils::Member("Gravity scale", b2Body_GetGravityScale(bodyId));
            ImGuiUtils::Member("Mass", b2Body_GetMass(bodyId));

            ImGui::PushID(static_cast<int>(entt::to_integral(entity)));
            DrawShapes(bodyId);
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void ImGuiRigidbody::DrawShapes(b2BodyId bodyId)
{
    b2ShapeId shapeIds[32] = { 0 };
    int shapeCount = b2Body_GetShapes(bodyId, shapeIds, 32);

    ImGui::TableNextColumn();
    const std::string shapeCountStr = std::to_string(shapeCount);
    bool shapesOpen = ImGui::TreeNodeEx("Shape count", ImGuiTreeNodeFlags_SpanAllColumns);
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(shapeCountStr.c_str());
    if (shapesOpen)
    {
        for (int i = 0; i < shapeCount; i++)
        {
            b2ShapeId shapeId = shapeIds[i];
            b2ShapeType type = b2Shape_GetType(shapeId);

            ImGui::PushID((void *)static_cast<intptr_t>(b2StoreShapeId(shapeId)));
            ImGui::TableNextColumn();
            const std::string shapeName = std::string("Shape ") + std::to_string(i);
            bool open = ImGui::TreeNodeEx(shapeName.c_str(), ImGuiTreeNodeFlags_SpanAllColumns);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(ENUM_STRING(type).c_str());

            b2Filter filter = b2Shape_GetFilter(shapeId);

            if (open)
            {
                ImGuiUtils::Member("Is sensor", b2Shape_IsSensor(shapeId));
                ImGuiUtils::Member("Contact events", b2Shape_AreContactEventsEnabled(shapeId));
                ImGuiUtils::Member("Sensor events", b2Shape_AreSensorEventsEnabled(shapeId));
                ImGuiUtils::MemberBits("Category bits", filter.categoryBits);
                ImGuiUtils::MemberBits("Mask bits", filter.maskBits);
                switch (type)
                {
                case b2_circleShape: DrawCircleShape(shapeId); break;
                case b2_capsuleShape: DrawCapsuleShape(shapeId); break;
                case b2_segmentShape: DrawSegmentShape(shapeId); break;
                case b2_polygonShape: DrawPolygonShape(shapeId); break;
                case b2_chainSegmentShape: DrawChainSegmentShape(shapeId); break;
                default: break;
                }
                ImGui::TreePop();
            }
            ImGui::PopID();

        }
        ImGui::TreePop();
    }
}

void ImGuiRigidbody::DrawPolygonShape(b2ShapeId shapeId)
{
    b2Polygon polygon = b2Shape_GetPolygon(shapeId);
    ImGuiUtils::Member("Count", polygon.count);
    for (int i = 0; i < polygon.count; i++)
    {
        std::string vertexName = std::string("Vertex ") + std::to_string(i);
        ImGuiUtils::Member(vertexName.c_str(), polygon.vertices[i]);
    }
}

void ImGuiRigidbody::DrawCircleShape(b2ShapeId shapeId)
{
    b2Circle circle = b2Shape_GetCircle(shapeId);
    ImGuiUtils::Member("Center", circle.center);
    ImGuiUtils::Member("Radius", circle.radius);

}

void ImGuiRigidbody::DrawCapsuleShape(b2ShapeId shapeId)
{
    b2Capsule capsule = b2Shape_GetCapsule(shapeId);
    ImGuiUtils::Member("Center 1", capsule.center1);
    ImGuiUtils::Member("Center 2", capsule.center2);
    ImGuiUtils::Member("Radius", capsule.radius);
}

void ImGuiRigidbody::DrawSegmentShape(b2ShapeId shapeId)
{
    b2Segment segment = b2Shape_GetSegment(shapeId);
    ImGuiUtils::Member("Point 1", segment.point1);
    ImGuiUtils::Member("Point 2", segment.point2);
}

void ImGuiRigidbody::DrawChainSegmentShape(b2ShapeId shapeId)
{
    b2ChainSegment chain = b2Shape_GetChainSegment(shapeId);
    ImGuiUtils::Member("Point 1", chain.segment.point1);
    ImGuiUtils::Member("Point 2", chain.segment.point2);
}

void ImGuiSprite::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<Sprite>(entity) == false) return;
    auto &sprite = registry.get<Sprite>(entity);

    if (ImGui::BeginTable("SpriteComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Dimensions", sprite.dimensions);
        ImGuiUtils::Member("Offset", sprite.offset);
        ImGuiUtils::Member("Anchor", ENUM_STRING(sprite.anchor));
        ImGuiUtils::Member("Flip", static_cast<int>(sprite.flip));
        ImGuiUtils::Member("Flip offset", sprite.flipOffset);
        ImGuiUtils::Member("Source x", sprite.srcRect.x);
        ImGuiUtils::Member("Source y", sprite.srcRect.y);
        ImGuiUtils::Member("Source w", sprite.srcRect.w);
        ImGuiUtils::Member("Source h", sprite.srcRect.h);

        ImGui::EndTable();
    }
}

void ImGuiTiledSprite::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<TiledSprite>(entity) == false) return;
    auto &tiled = registry.get<TiledSprite>(entity);

    if (ImGui::BeginTable("TiledSprite Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Tiled count x", tiled.tiledCountX);
        ImGuiUtils::Member("Tiled count y", tiled.tiledCountY);

        ImGui::EndTable();
    }
}

void ImGuiTilemapRenderer::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<TilemapRenderer>(entity) == false) return;
    auto &tilemap = registry.get<TilemapRenderer>(entity);

    if (ImGui::BeginTable("TilemapRendererComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Sprite count", tilemap.tiles.size());
        ImGuiUtils::Member("Scale", tilemap.scale);

        ImGui::EndTable();
    }
}

void ImGuiSpriteAnimState::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<SpriteAnimState>(entity) == false) return;
    auto &anim = registry.get<SpriteAnimState>(entity);

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    if (animManager == nullptr) return;

    if (ImGui::BeginTable("SpriteAnimComponent Members", 2, membersTableFlags))
    {
        const std::string idString = animManager->GetAnimName(anim.id);

        ImGuiUtils::Member("Animation ID", idString.c_str());
        ImGuiUtils::Member("Cycle index", anim.cycleIdx);
        ImGuiUtils::Member("Frame index", anim.frameIdx);
        ImGuiUtils::Member("Speed", anim.speed);
        ImGuiUtils::Member("Is playing", SpriteAnimUtils::IsPlaying(anim));
        ImGuiUtils::Member("Is stopped", SpriteAnimUtils::IsStopped(anim));
        ImGuiUtils::Member("Is delayed", SpriteAnimUtils::IsDelayed(anim));

        ImGui::EndTable();
    }
}

void ImGuiRenderBlendMod::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<RenderBlendMod>(entity) == false) return;
    auto &blendMod = registry.get<RenderBlendMod>(entity);

    if (ImGui::BeginTable("RenderBlendModComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("SDL BlendMode", static_cast<int>(blendMod.blendMode));
        ImGuiUtils::Member("alpha", blendMod.alpha);

        ImGui::EndTable();
    }
}

void ImGuiRenderColorMod::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<RenderColorMod>(entity) == false) return;
    auto &colorMod = registry.get<RenderColorMod>(entity);

    if (ImGui::BeginTable("RenderColorModComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Color mod R", colorMod.r);
        ImGuiUtils::Member("Color mod G", colorMod.g);
        ImGuiUtils::Member("Color mod B", colorMod.b);

        ImGui::EndTable();
    }
}

void ImGuiBackgroundLayer::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<BackgroundLayer>(entity) == false) return;
    auto &layer = registry.get<BackgroundLayer>(entity);

    if (ImGui::BeginTable("BackgroundLayerComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Dimensions", layer.dimensions);
        ImGuiUtils::Member("Shift factor", layer.shiftFactor);
        ImGuiUtils::Member("Mode", ENUM_STRING(layer.mode));

        ImGui::EndTable();
    }
}

void ImGuiCamera::DrawImGui(entt::registry &registry, entt::entity entity)
{
    if (registry.all_of<Camera>(entity) == false) return;
    auto &camera = registry.get<Camera>(entity);

    if (ImGui::BeginTable("CameraComponent Members", 2, membersTableFlags))
    {
        ImGuiUtils::Member("Is Active", camera.isActive);
        ImGuiUtils::Member("View extents", camera.viewExtents);
        ImGuiUtils::Member("View scale", camera.viewScale);
        ImGuiUtils::Member("Viewport x", camera.viewport.x);
        ImGuiUtils::Member("Viewport y", camera.viewport.y);
        ImGuiUtils::Member("Viewport w", camera.viewport.w);
        ImGuiUtils::Member("Viewport h", camera.viewport.h);
        ImGui::EndTable();
    }
}

//void ImGuiChildrenComponent::DrawImGui(entt::registry &registry, entt::entity entity)
//{
//    if (registry.all_of<ChildrenComponent>(entity) == false) return;
//    auto &children = registry.get<ChildrenComponent>(entity);
//
//    for (entt::entity child : children.children)
//    {
//        if (registry.valid(child))
//        {
//            ImGui::Bullet();
//            ImGui::PushID(static_cast<int>(entt::to_integral(child)));
//            if (ImGui::Selectable(ImGuiManager::GetEntityName(registry, child).c_str(), false))
//            {
//                m_imGuiManager->SetSelected(child);
//            }
//            ImGui::PopID();
//        }
//        else
//        {
//            ImGui::TextUnformatted("Invalid entity");
//        }
//    }
//}
//
//void ImGuiParentComponent::DrawImGui(entt::registry &registry, entt::entity entity)
//{
//    if (registry.all_of<ParentComponent>(entity) == false) return;
//    entt::entity parent = registry.get<ParentComponent>(entity).parent;
//    if (registry.valid(parent))
//    {
//        ImGui::PushID(static_cast<int>(entt::to_integral(entity)));
//        if (ImGui::Selectable(ImGuiManager::GetEntityName(registry, parent).c_str(), false))
//        {
//            m_imGuiManager->SetSelected(parent);
//        }
//        ImGui::PopID();
//    }
//    else
//    {
//        ImGui::TextUnformatted("Invalid entity");
//    }
//}
