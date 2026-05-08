/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "ecs/basic_components.h"

class GizmosShape
{
public:
    GizmosShape();
    GizmosShape(const b2ShapeId shapeId);
    GizmosShape(const b2ShapeId shapeId, const b2Transform &transform);
    GizmosShape(b2Vec2 point1, b2Vec2 point2);
    GizmosShape(const b2AABB &aabb);

    void Reset();
    void Set(const b2ShapeId shapeId, const b2Transform &transform = b2Transform_identity);
    void Set(const b2AABB &aabb, const b2Transform &transform = b2Transform_identity);
    void Set(b2Vec2 point, const b2Transform &transform = b2Transform_identity);
    void Set(b2Vec2 point1, b2Vec2 point2, const b2Transform &transform = b2Transform_identity);
    void Set(const std::vector<b2Vec2> &points, const b2Transform &transform = b2Transform_identity);

    void SetAsPolygon(const b2Polygon &polygon, const b2Transform &transform = b2Transform_identity);
    void SetAsCircle(const b2Circle &circle, const b2Transform &transform = b2Transform_identity);
    void SetAsSegment(const b2Segment &segment, const b2Transform &transform = b2Transform_identity);
    void SetAsCapsule(const b2Capsule &capsule,  const b2Transform &transform = b2Transform_identity);
    void SetAsChain(const b2ChainId chainId, const b2Transform &transform = b2Transform_identity);

    void Render(SDL_Renderer *renderer, const Camera &camera, const Transform &cameraTransform);

private:
    std::vector<b2Vec2> m_vertices;
};
