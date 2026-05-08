/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/basic_components.h"
#include "utils/utils.h"

#ifndef HD_WIDTH
#  define HD_WIDTH 1280
#endif
#ifndef HD_HEIGHT
#  define HD_HEIGHT 720
#endif

// position et angle du sprite
void Transform::Set(b2Vec2 p, float a)
{
    position = p;
    angle = a;
}

// données reliant le corps physique et le composant
void Transform::Set(b2Transform xf)
{
    position = xf.p;
    angle = b2Rot_GetAngle(xf.q);
}

void FixedUpdateTransform::Set(b2Vec2 position, float angle)
{
    currXf = { position, b2MakeRot(angle * B2_PI  / 180.f) };
    prevXf = currXf;
}

void FixedUpdateTransform::Set(b2Transform xf)
{
    currXf = xf;
    prevXf = xf;
}

//  en cas d’élément rattaché à un parent (comme le bouclier)
LocalTransform::LocalTransform(entt::entity parentIn, b2Vec2 positionIn, float angleIn)
    : position(positionIn)
    , angle(angleIn)
    , parent(parentIn)
{
}

void LocalTransform::Set(b2Vec2 positionIn, float angleIn)
{
    position = positionIn;
    angle = angleIn;
}

void LocalTransform::Set(b2Transform xf)
{
    position = xf.p;
    angle = b2Rot_GetAngle(xf.q); 
}


void Sprite::Reset()
{
    texture = nullptr;
    flip = SDL_FLIP_NONE;
    anchor = Anchor::NORTH_WEST;
    srcRect = { 0.f, 0.f, 1.f, 1.f };
    dimensions = { 1.f, 1.f };
    offset = b2Vec2_zero;
}

void Sprite::SetSprite(SpriteGroup *spriteGroup, int spriteIdx, float pixelsPerUnit)
{
    const float epsilon = 1e-2f;
    texture = spriteGroup->GetTexture();
    srcRect = *(spriteGroup->GetSourceRect(spriteIdx));
    dimensions.x = srcRect.w / pixelsPerUnit + epsilon;
    dimensions.y = srcRect.h / pixelsPerUnit + epsilon;
}

void RigidbodyUtils::Destroy(entt::registry &reg, entt::entity entity)
{
    Rigidbody& rigidbody = reg.get<Rigidbody>(entity);
    b2BodyId bodyId = rigidbody.bodyId;
    if (B2_IS_NON_NULL(bodyId)) 
    {
        delete b2Body_GetUserData(bodyId);
        b2DestroyBody(bodyId);
        rigidbody.bodyId = b2_nullBodyId;
    }
}

void RigidbodyUtils::Construct(entt::registry &reg, entt::entity entity)
{
    Rigidbody& rigidbody = reg.get<Rigidbody>(entity);
    b2BodyId bodyId = rigidbody.bodyId;
    if (B2_IS_NON_NULL(bodyId))
    {
        b2Body_SetUserData(bodyId, new BodyUserData(entity));
    }
}

void RigidbodyUtils::AttachConstruct(entt::registry &reg)
{
    reg.on_construct<Rigidbody>().connect<&RigidbodyUtils::Construct>();
}

void RigidbodyUtils::AttachDestroy(entt::registry &reg)
{
    reg.on_destroy<Rigidbody>().connect<&RigidbodyUtils::Destroy>();
}

entt::entity RigidbodyUtils::GetEntity(const Rigidbody &rigidbody)
{
    return GetEntity(rigidbody.bodyId);
}

entt::entity RigidbodyUtils::GetEntity(b2BodyId bodyId)
{
    if (b2Body_IsValid(bodyId))
    {
        BodyUserData *data = static_cast<BodyUserData *>(b2Body_GetUserData(bodyId));
        return (data != nullptr) ? data->entity : entt::null;
    }
    return entt::null;
}

entt::entity RigidbodyUtils::GetEntity(b2ShapeId shapeId)
{
    if (b2Shape_IsValid(shapeId))
    {
        return GetEntity(b2Shape_GetBody(shapeId));
    }
    return entt::null;
}

b2Vec2 RigidbodyUtils::GetPosition(const Rigidbody &rigidbody)
{
    return GetPosition(rigidbody.bodyId);
}

b2Vec2 RigidbodyUtils::GetPosition(b2BodyId bodyId)
{
    if (b2Body_IsValid(bodyId))
    {
        return b2Body_GetPosition(bodyId);
    }
    return b2Vec2_zero;
}

b2Vec2 RigidbodyUtils::GetPosition(b2ShapeId shapeId)
{
    if (b2Shape_IsValid(shapeId))
    {
        return GetPosition(b2Shape_GetBody(shapeId));
    }
    return b2Vec2_zero;
}

b2Vec2 RigidbodyUtils::GetVelocity(const Rigidbody &rigidbody)
{
    return GetVelocity(rigidbody.bodyId);
}

b2Vec2 RigidbodyUtils::GetVelocity(b2BodyId bodyId)
{
    if (b2Body_IsValid(bodyId))
    {
        return b2Body_GetLinearVelocity(bodyId);
    }
    return b2Vec2_zero;
}

b2Vec2 RigidbodyUtils::GetVelocity(b2ShapeId shapeId)
{
    if (b2Shape_IsValid(shapeId))
    {
        return GetVelocity(b2Shape_GetBody(shapeId));
    }
    return b2Vec2_zero;
}

void BackgroundLayer::SetDimensions(float pixelsPerUnit)
{
    if (texture)
    {
        dimensions.x = static_cast<float>(texture->w) / pixelsPerUnit;
        dimensions.y = static_cast<float>(texture->h) / pixelsPerUnit;
    }
}

Camera::Camera(b2Vec2 viewExtentsIn, float viewScaleIn, bool isActiveIn)
    : viewExtents(viewExtentsIn)
    , viewScale(viewScaleIn)
    , isActive(isActiveIn)
    , viewport{ 0.f, 0.f, static_cast<float>(HD_WIDTH), static_cast<float>(HD_HEIGHT) }
{}

float Camera::ViewToWorldX(float camPositionX, float x) const
{
    const float ratio = x / viewport.w;
    const float worldW = 2.f * viewExtents.x * viewScale;
    return (camPositionX - viewExtents.x * viewScale) + ratio * worldW;
}

float Camera::ViewToWorldY(float camPositionY, float y) const
{
    const float ratio = (viewport.h - y) / viewport.h;
    const float worldH = 2.f * viewExtents.y * viewScale;
    return (camPositionY - viewExtents.y * viewScale) + ratio * worldH;
}

b2Vec2 Camera::ViewToWorld(b2Vec2 camPosition, float x, float y) const
{
    return b2Vec2{
        ViewToWorldX(camPosition.x, x),
        ViewToWorldY(camPosition.y, y)
    };
}

float Camera::WorldToViewX(float camPositionX, float positionX) const
{
    const float scale = GetWorldToViewScaleX();
    return (positionX - (camPositionX - viewExtents.x * viewScale)) * scale;
}

float Camera::WorldToViewY(float camPositionY, float positionY) const
{
    const float scale = GetWorldToViewScaleY();
    return ((camPositionY + viewExtents.y * viewScale) - positionY) * scale;
}

void Camera::WorldToView(b2Vec2 camPosition, b2Vec2 position, float &x, float &y) const
{
    x = WorldToViewX(camPosition.x, position.x);
    y = WorldToViewY(camPosition.y, position.y);
}

float Camera::GetWorldToViewScale() const
{
    return 0.5f * viewport.w / (viewExtents.x * viewScale);
}

float Camera::GetWorldToViewScaleX() const
{
    return 0.5f * viewport.w / (viewExtents.x * viewScale);
}

float Camera::GetWorldToViewScaleY() const
{
    return 0.5f * viewport.h / (viewExtents.y * viewScale);
}

void Camera::SetScaleToFitWidth(float width)
{
}

void Camera::UpdateViewport(SDL_Renderer *renderer)
{
    assert(renderer && "The SDL_Renderer must be created");

    int outW = 0, outH = 0;
    bool success = SDL_GetRenderOutputSize(renderer, &outW, &outH);
    assert(success);

    outW = math::Max(outW, HD_WIDTH);
    outH = math::Max(outH, HD_HEIGHT);

    const float worldAspectRatio = viewExtents.x / viewExtents.y;
    const float outputAspectRatio = (float)outW / (float)outH;

    viewport.x = 0.f;
    viewport.y = 0.f;
    if (outputAspectRatio >= worldAspectRatio)
    {
        // Output plus large
        viewport.w = worldAspectRatio * (float)outH;
        viewport.h = (float)outH;
        viewport.x = 0.5f * (outW - viewport.w);
    }
    else
    {
        // Output plus haut
        viewport.w = (float)outW;
        viewport.h = (float)outW / worldAspectRatio;
        viewport.y = 0.5f * (outH - viewport.h);
    }

    SDL_Rect rendererViewport = { 0 };
    rendererViewport.x = (int)roundf(viewport.x);
    rendererViewport.y = (int)roundf(viewport.y);
    rendererViewport.w = (int)roundf(viewport.w);
    rendererViewport.h = (int)roundf(viewport.h);

    SDL_SetRenderViewport(renderer, &rendererViewport);
}

// coordonnées du monde à afficher sur l’écran
b2AABB Camera::GetWorldView(b2Vec2 camPosition) const
{
    return b2AABB{
        camPosition - viewScale * viewExtents,
        camPosition + viewScale * viewExtents,
    };
}
