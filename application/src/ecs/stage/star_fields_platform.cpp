/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/stage/star_fields_platform.h"
#include "ecs/common/kinematic_utils.h"

void StarFieldsPlatformCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position, m_type);
}

void StarFieldsPlatformCommand::Create(
    entt::registry &registry, entt::entity entity,
    Scene *scene, b2Vec2 position, int type)
{
    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_TERRAIN;
    renderLayer.orderInLayer = 1;

    registry.emplace<RenderSortingLayer>(entity, renderLayer);

    const float scale = 16.f / 20.f;
    if (type == 0)
    {
        CreateTilesA(registry, entity, scene, scale);
    }

    b2BodyId bodyId = CreateBody(entity, scene, position, scale, type);
    b2Transform xf = b2Body_GetTransform(bodyId);

    FloatingPlatform platform;
    platform.freqX = (type == 0) ? 10.f : 0.0f;
    platform.freqY = 1.0f;
    platform.phaseX = 0.0f;
    platform.phaseY = 0.0f;
    platform.amplitudeX = 1.0f;
    platform.amplitudeY = 0.0f;

    registry.emplace<NameComponent>(entity, "Star fields platform");
    registry.emplace<Transform>(entity, xf);
    registry.emplace<FixedUpdateTransform>(entity, xf);
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<KinematicTargetPosition>(entity, position);
    registry.emplace<KinematicTargetRotation>(entity);
    registry.emplace<FloatingPlatform>(entity, platform);
    registry.emplace<ReferencePosition>(entity, position);
}

b2BodyId StarFieldsPlatformCommand::CreateBody(
    entt::entity entity, Scene *scene, b2Vec2 position, float scale, int type)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_kinematicBody;
    bodyDef.position = position;
    bodyDef.rotation = b2Rot_identity;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    
    // PreSolve pour le OneWay
    shapeDef.enablePreSolveEvents = true;
    shapeDef.filter.categoryBits = CATEGORY_TERRAIN;
    shapeDef.material.friction = 0.5f;
    shapeDef.material.restitution = 0.0f;
    shapeDef.density = 1.f;

    if (type == 0)
    {
        b2Polygon box = b2MakeOffsetBox(scale * 4.5f, 0.5f, b2Vec2{ scale * 0.5f, -0.5f }, b2Rot_identity);
        b2CreatePolygonShape(bodyId, &shapeDef, &box);
    }
    return bodyId;
}

void StarFieldsPlatformCommand::CreateTilesA(entt::registry &registry, entt::entity entity, Scene *scene, float scale)
{
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_TILESET_STAR_FIELDS);
    AssertNew(spriteSheet);

    SpriteGroup *platform = spriteSheet->GetGroup("Platform");
    AssertNew(platform);
    SpriteGroup *ground2 = spriteSheet->GetGroup("Ground2");
    AssertNew(ground2);

    Sprite tile;
    tile.flipOffset = false;
    const float pixelsPerUnit = 16.f;

    registry.emplace<TilemapRenderer>(entity);
    TilemapRenderer &tilemap = registry.get<TilemapRenderer>(entity);
    tilemap.scale = scale;

    // Sol
    tile.Reset();
    tile.SetSprite(ground2, 4, pixelsPerUnit);
    tile.offset = { -2.f, 0.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(ground2, 3, pixelsPerUnit);
    tile.offset = { 0.f, 0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    // Platforme à gauche
    tile.Reset();
    tile.SetSprite(platform, 0, pixelsPerUnit);
    tile.offset = { -4.f, 0.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 2, pixelsPerUnit);
    tile.offset = { -3.f, 0.f };
    tilemap.tiles.push_back(tile);

    // Platforme à droite
    tile.Reset();
    tile.SetSprite(platform, 2, pixelsPerUnit);
    tile.offset = { 2.f, 0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 1, pixelsPerUnit);
    tile.offset = { 3.f, 0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 0, pixelsPerUnit);
    tile.offset = { 4.f, 0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);
}

void StarFieldsPlatformCommand::CreateTilesB(
    entt::registry &registry, entt::entity entity, Scene *scene, float scale)
{
    // Pour faire une seconde platforme comme dans la démo
}

FloatingPlatform::FloatingPlatform()
    : freqX(1.0f)
    , freqY(1.0f)
    , phaseX(0.0f)
    , phaseY(0.0f)
    , amplitudeX(1.0f)
    , amplitudeY(1.0f)
{
}

void FloatingPlatformSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    float elapsed = m_scene->GetElapsed();

    auto view = m_registry.view<const FloatingPlatform, KinematicTargetPosition, const ReferencePosition, const Rigidbody>();
    for (auto [entity, platform, targetPos, refPos, rigidbody] : view.each())
    {
        b2Vec2 target = refPos.position;
        target.x += platform.amplitudeX * cosf(2.f * B2_PI * (elapsed / platform.freqX + platform.phaseX));
        target.y += platform.amplitudeY * cosf(2.f * B2_PI * (elapsed / platform.freqY + platform.phaseY));
        targetPos.target = target;
    }

    // Ajouter éventuellement une rotation
}
