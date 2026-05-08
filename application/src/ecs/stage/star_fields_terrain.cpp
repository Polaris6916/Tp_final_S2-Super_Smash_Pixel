/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/stage/star_fields_terrain.h"
#include "ecs/common/kinematic_utils.h"

void StarFieldsTerrainCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_position);
}

void StarFieldsTerrainCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, b2Vec2 position)
{
    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_TERRAIN;
    renderLayer.orderInLayer = 0;

    registry.emplace<RenderSortingLayer>(entity, renderLayer);

    const float scale = 16.f / 20.f;
    CreateTiles(registry, entity, scene, scale);

    b2BodyId bodyId = CreateBody(entity, scene, position, scale);

    registry.emplace<NameComponent>(entity, "Star fields terrain");
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
}

b2BodyId StarFieldsTerrainCommand::CreateBody(
    entt::entity entity, Scene *scene, b2Vec2 position, float scale)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;
    bodyDef.rotation = b2Rot_identity;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    std::array<b2Vec2, 12> vertices = {
        b2Vec2{ +14.0f, +0.0f },
        b2Vec2{ +14.0f, +0.0f },
        b2Vec2{ + 6.5f, +0.0f },
        b2Vec2{ + 4.0f, +0.4f },
        b2Vec2{ + 1.0f, +0.0f },
        b2Vec2{ - 0.3f, +0.2f },
        b2Vec2{ - 2.0f, +0.0f },
        b2Vec2{ - 3.5f, +0.0f },
        b2Vec2{ - 5.5f, -0.3f },
        b2Vec2{ - 7.5f, +0.0f },
        b2Vec2{ -14.0f, +0.0f },
        b2Vec2{ -14.0f, +0.0f }
    };
    for (int i = 0; i < vertices.size(); i++)
        vertices[i] *= scale;

    b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
    material.friction = 0.5f;
    material.restitution = 0.0f;

    b2ChainDef chainDef = b2DefaultChainDef();
    chainDef.points = vertices.data();
    chainDef.count = static_cast<int>(vertices.size());
    chainDef.isLoop = false;
    chainDef.materials = &material;
    chainDef.materialCount = 1;
    chainDef.filter.categoryBits = CATEGORY_TERRAIN;

    b2ChainId chainId = b2CreateChain(bodyId, &chainDef);

    return bodyId;
}

void StarFieldsTerrainCommand::CreateTiles(
    entt::registry &registry, entt::entity entity, Scene *scene, float scale)
{
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_TILESET_STAR_FIELDS);
    AssertNew(spriteSheet);

    SpriteGroup *tree = spriteSheet->GetGroup("Tree");
    AssertNew(tree);
    SpriteGroup *bush = spriteSheet->GetGroup("Bush");
    AssertNew(bush);
    SpriteGroup *rock = spriteSheet->GetGroup("Rock");
    AssertNew(rock);
    SpriteGroup *plant = spriteSheet->GetGroup("Plant");
    AssertNew(plant);
    SpriteGroup *fallingPlant = spriteSheet->GetGroup("FallingPlant");
    AssertNew(fallingPlant);
    SpriteGroup *plantCorner = spriteSheet->GetGroup("PlantCorner");
    AssertNew(plantCorner);
    SpriteGroup *platform = spriteSheet->GetGroup("Platform");
    AssertNew(platform);
    SpriteGroup *grass = spriteSheet->GetGroup("Grass");
    AssertNew(grass);
    SpriteGroup *ground1 = spriteSheet->GetGroup("Ground1");
    AssertNew(ground1);
    SpriteGroup *ground2 = spriteSheet->GetGroup("Ground2");
    AssertNew(ground2);
    SpriteGroup *ground3 = spriteSheet->GetGroup("Ground3");
    AssertNew(ground3);
    SpriteGroup *ground6 = spriteSheet->GetGroup("Ground6");
    AssertNew(ground6);
    SpriteGroup *groundCorner = spriteSheet->GetGroup("GroundCorner");
    AssertNew(groundCorner);

    Sprite tile;
    tile.flipOffset = false;
    const float pixelsPerUnit = 16.f;

    registry.emplace<TilemapRenderer>(entity);
    TilemapRenderer &tilemap = registry.get<TilemapRenderer>(entity);
    tilemap.scale = scale;

    // Sol
    int w = 22;
    for (int i = 0; i < w; i++)
    {
        float x = (float)(i - w / 2);
        tile.Reset();
        tile.SetSprite(ground1, 0, pixelsPerUnit);
        tile.offset = { x, +0.f };
        tilemap.tiles.push_back(tile);
        tile.offset = { x, -1.f };
        tilemap.tiles.push_back(tile);
    }
    for (int i = 0; i < w; i++)
    {
        float x = (float)(i - w / 2);

        // Herbe renversée
        tile.Reset();
        tile.SetSprite(grass, (i + i / 2) % 4, pixelsPerUnit);
        tile.offset = { x, -1.f };
        tilemap.tiles.push_back(tile);

        tile.Reset();
        tile.SetSprite(grass, i % 4, pixelsPerUnit);
        tile.offset = { x, -2.f };
        tile.flip = SDL_FLIP_VERTICAL;
        tilemap.tiles.push_back(tile);
    }

    // Déco dans le sol
    tile.Reset();
    tile.SetSprite(ground6, 0, pixelsPerUnit);
    tile.offset = { -9.f, 0.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(ground2, 2, pixelsPerUnit);
    tile.offset = { -3.5f, -0.3f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(ground3, 0, pixelsPerUnit);
    tile.offset = { +5.5f, 0.f };
    tilemap.tiles.push_back(tile);

    // Angles dans le sol (gauche)
    tile.Reset();
    tile.SetSprite(groundCorner, 2, pixelsPerUnit);
    tile.offset = { -11.f, -1.f };
    tilemap.tiles.push_back(tile);

    // Angles dans le sol (droite)
    tile.Reset();
    tile.SetSprite(groundCorner, 0, pixelsPerUnit);
    tile.offset = { +10.f, 0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(groundCorner, 3, pixelsPerUnit);
    tile.offset = { 10.f, -1.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    // Buisson
    tile.Reset();
    tile.SetSprite(bush, 0, pixelsPerUnit);
    tile.offset = { -3.f, 2.f };
    tilemap.tiles.push_back(tile);

    // Arbre
    tile.Reset();
    tile.SetSprite(tree, 0, pixelsPerUnit);
    tile.offset = { +0.f, +9.f };
    tilemap.tiles.push_back(tile);

    // Rocher
    tile.Reset();
    tile.SetSprite(rock, 1, pixelsPerUnit);
    tile.offset = { +8.f, +2.f };
    tilemap.tiles.push_back(tile);

    // Plantes sur le sol
    int plantIDs[] = {
        2, 0, 1, 0
    };
    float plantPositions[] = {
        -11.f, -9.3f, -3.6f, 10.f
    };
    for (int i = 0; i < 4; i++)
    {
        tile.Reset();
        tile.SetSprite(plant, plantIDs[i], pixelsPerUnit);
        tile.offset = { plantPositions[i], 1.f };
        tilemap.tiles.push_back(tile);
    }
    tile.Reset();
    tile.SetSprite(grass, 2, pixelsPerUnit);
    tile.offset = { -10.f, 1.f };
    tilemap.tiles.push_back(tile);

    // Plantes tombantes
    tile.Reset();
    tile.SetSprite(fallingPlant, 1, pixelsPerUnit);
    tile.offset = { -10.f, -2.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(fallingPlant, 3, pixelsPerUnit);
    tile.offset = { +9.f, -2.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    // Platforme gauche
    tile.Reset();
    tile.SetSprite(platform, 0, pixelsPerUnit);
    tile.offset = { -14.f, +0.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 1, pixelsPerUnit);
    tile.offset = { -13.f, +0.f };
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 2, pixelsPerUnit);
    tile.offset = { -12.f, +0.f };
    tilemap.tiles.push_back(tile);

    // Platforme Droite
    tile.Reset();
    tile.SetSprite(platform, 2, pixelsPerUnit);
    tile.offset = { +11.f, +0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 1, pixelsPerUnit);
    tile.offset = { +12.f, +0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);

    tile.Reset();
    tile.SetSprite(platform, 0, pixelsPerUnit);
    tile.offset = { +13.f, +0.f };
    tile.flip = SDL_FLIP_HORIZONTAL;
    tilemap.tiles.push_back(tile);
}
