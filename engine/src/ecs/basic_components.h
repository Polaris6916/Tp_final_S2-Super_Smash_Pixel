/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "rendering/anim.h"
#include "rendering/sprite_sheet.h"
#include "rendering/sprite_anim.h"

struct NameComponent
{
    std::string name;
};

struct Transform
{
    Transform(b2Vec2 position = b2Vec2_zero, float angle = 0.f)
        : position(position)
        , angle(angle)
    {}
    Transform(b2Transform xf) { Set(xf); }

    b2Vec2 position;
    float angle;

    void Set(b2Vec2 position, float angle);
    void Set(b2Transform xf);
};

struct FixedUpdateTransform
{
    FixedUpdateTransform(b2Transform xf = b2Transform_identity)
        : currXf(xf), prevXf(xf) {}

    b2Transform currXf;
    b2Transform prevXf;

    void Set(b2Vec2 position, float angle);
    void Set(b2Transform xf);
};

struct LocalTransform
{
    LocalTransform(
        entt::entity parent = entt::null,
        b2Vec2 position = b2Vec2_zero,
        float angle = 0.f
    );

    entt::entity parent;
    b2Vec2 position;
    float angle;

    void Set(b2Vec2 position, float angle);
    void Set(b2Transform xf);
};

struct Rigidbody
{
    b2BodyId bodyId;
};

class RigidbodyUtils
{
public:
    static void Destroy(entt::registry &reg, entt::entity entity);
    static void Construct(entt::registry &reg, entt::entity entity);
    static void AttachConstruct(entt::registry &reg);
    static void AttachDestroy(entt::registry &reg);

    static entt::entity GetEntity(const Rigidbody &rigidbody);
    static entt::entity GetEntity(b2BodyId bodyId);
    static entt::entity GetEntity(b2ShapeId shapeId);

    static b2Vec2 GetPosition(const Rigidbody &rigidbody);
    static b2Vec2 GetPosition(b2BodyId bodyId);
    static b2Vec2 GetPosition(b2ShapeId shapeId);
    static b2Vec2 GetVelocity(const Rigidbody &rigidbody);
    static b2Vec2 GetVelocity(b2BodyId bodyId);
    static b2Vec2 GetVelocity(b2ShapeId shapeId);

private:
    RigidbodyUtils() = delete;
};

// profondeur d’affichage (avant plan ou arrière plan)
struct RenderSortingLayer
{
    RenderSortingLayer(int layer = 0, int orderInLayer = 0, bool enabled = true)
        : layer(layer), orderInLayer(orderInLayer), enabled(enabled)
    {}

    bool enabled;
    int layer;
    int orderInLayer;
};

// transparence de sprite
struct RenderBlendMod
{
    RenderBlendMod(float alpha = 1.f, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND)
        : alpha(alpha), blendMode(blendMode)
    {}

    SDL_BlendMode blendMode;
    float alpha;
};

//  modification de couleurs du sprite
struct RenderColorMod
{
    RenderColorMod(float r = 1.f, float g = 1.f, float b = 1.f)
        : r(r), g(g), b(b)
    {}

    float r, g, b;
};

// éléments à afficher
struct Sprite
{
    Sprite()
        : texture(nullptr)
        , srcRect{ 0.f, 0.f, 1.f, 1.f }
        , anchor(Anchor::CENTER)
        , dimensions({ 1.f, 1.f })
        , offset(b2Vec2_zero)
        , flip(SDL_FLIP_NONE)
        , flipOffset(true)
    {}

    SDL_Texture *texture;
    SDL_FRect srcRect;
    Anchor anchor;
    b2Vec2 dimensions;
    SDL_FlipMode flip;
    bool flipOffset;
    b2Vec2 offset;

    void Reset();
    void SetSprite(SpriteGroup *spriteGroup, int spriteIdx, float pixelsPerUnit = 24.f);
};

// groupe de plusieurs sprites identiques (utilisé pour le laser)
struct TiledSprite
{
    int tiledCountX;
    int tiledCountY;
};

// groupe des sprites formant le sol
struct TilemapRenderer
{
    TilemapRenderer()
        : tiles()
        , scale(1.f)
    {}

    std::vector<Sprite> tiles;
    float scale;
};

enum class LayerMode : uint32_t
{
    NO_FILL, FILL_ABOVE, FILL_BELOW, FILL_VERTICAL
};

//  images de fond (bougent en fonction de la caméra)
struct BackgroundLayer
{
    BackgroundLayer(SDL_Texture *texture = nullptr)
        : dimensions{ 16.f, 9.f }
        , shiftFactor{ 0.5f, 0.5f }
        , mode(LayerMode::FILL_BELOW)
        , texture(texture)
    {}

    b2Vec2 dimensions;
    SDL_Texture *texture;
    b2Vec2 shiftFactor;
    LayerMode mode;

    void SetDimensions(float pixelsPerUnit);
};

struct Camera
{
    Camera(b2Vec2 viewExtents = { 16.0f, 9.0f }, float viewScale = 1.f, bool isActive = true);

    b2Vec2 viewExtents;
    float viewScale;
    bool isActive;
    SDL_FRect viewport;

    float ViewToWorldX(float camPositionX, float x) const;
    float ViewToWorldY(float camPositionY, float y) const;
    b2Vec2 ViewToWorld(b2Vec2 camPosition, float x, float y) const;
    float WorldToViewX(float camPositionX, float positionX) const;
    float WorldToViewY(float camPositionY, float positionY) const;
    void WorldToView(b2Vec2 camPosition, b2Vec2 position, float &x, float &y) const;

    float GetWorldToViewScale() const;
    float GetWorldToViewScaleX() const;
    float GetWorldToViewScaleY() const;
    void SetScaleToFitWidth(float width);

    void UpdateViewport(SDL_Renderer *renderer);
    b2AABB GetWorldView(b2Vec2 camPosition) const;
};
