/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "utils/gizmos_shape.h"
#include "utils/utils.h"

GizmosShape::GizmosShape() : m_vertices()
{
}

GizmosShape::GizmosShape(const b2ShapeId shapeId, const b2Transform &transform)
    : m_vertices()
{
    Set(shapeId, transform);
}

GizmosShape::GizmosShape(b2Vec2 point1, b2Vec2 point2)
    : m_vertices()
{
    Set(point1, point2);
}

GizmosShape::GizmosShape(const b2AABB &aabb)
    : m_vertices()
{
    Set(aabb);
}

GizmosShape::GizmosShape(const b2ShapeId shapeId)
    : m_vertices()
{
    Set(shapeId);
}

void GizmosShape::Set(const b2ShapeId shapeId, const b2Transform &transform)
{
    m_vertices.clear();

    switch (b2Shape_GetType(shapeId))
    {
    case b2_polygonShape:
    {
        b2Polygon polygon = b2Shape_GetPolygon(shapeId);
        SetAsPolygon(polygon, transform);
        break;
    }
    case b2_circleShape:
    {
        b2Circle circle = b2Shape_GetCircle(shapeId);
        SetAsCircle(circle, transform);
        break;
    }
    case b2_segmentShape:
    {
        b2Segment segment = b2Shape_GetSegment(shapeId);
        SetAsSegment(segment, transform);
        break;
    }
    case b2_capsuleShape:
    {
        b2Capsule capsule = b2Shape_GetCapsule(shapeId);
        SetAsCapsule(capsule, transform);
        break;
    }
    case b2_chainSegmentShape:
    {
        b2ChainId chainId = b2Shape_GetParentChain(shapeId);
        SetAsChain(chainId, transform);
        break;
    }
    default:
        break;
    }
}

void GizmosShape::Set(const b2AABB &aabb, const b2Transform &transform)
{
    m_vertices.clear();
    b2Vec2 lower = b2TransformPoint(transform, aabb.lowerBound);
    b2Vec2 upper = b2TransformPoint(transform, aabb.upperBound);
    m_vertices.push_back(lower);
    m_vertices.push_back(b2Vec2{ upper.x, lower.y });
    m_vertices.push_back(upper);
    m_vertices.push_back(b2Vec2{ lower.x, upper.y });
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::Set(b2Vec2 point, const b2Transform &transform)
{
    point = b2TransformPoint(transform, point);
    m_vertices.clear();
    m_vertices.push_back(point + b2Vec2{ +0.1f, +0.1f });
    m_vertices.push_back(point + b2Vec2{ -0.1f, -0.1f });
    m_vertices.push_back(point + b2Vec2{ +0.0f, +0.0f });
    m_vertices.push_back(point + b2Vec2{ +0.1f, -0.1f });
    m_vertices.push_back(point + b2Vec2{ -0.1f, +0.1f });
}

void GizmosShape::Set(b2Vec2 point1, b2Vec2 point2, const b2Transform &transform)
{
    m_vertices.clear();
    m_vertices.push_back(b2TransformPoint(transform, point1));
    m_vertices.push_back(b2TransformPoint(transform, point2));
}

void GizmosShape::Set(const std::vector<b2Vec2> &points, const b2Transform &transform)
{
    assert(points.size() > 1);

    m_vertices.clear();
    for (int i = 0; i < points.size(); i++)
    {
        m_vertices.push_back(b2TransformPoint(transform, points[i]));
    }
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::Reset()
{
    m_vertices.clear();
}

void GizmosShape::SetAsPolygon(const b2Polygon &polygon, const b2Transform &transform)
{
    assert(polygon.count > 1);

    m_vertices.clear();
    for (int i = 0; i < polygon.count; i++)
    {
        m_vertices.push_back(b2TransformPoint(transform, polygon.vertices[i]));
    }
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::SetAsCircle(const b2Circle &circle, const b2Transform &transform)
{
    constexpr int CIRCLE_SEG_COUNT = 16;
    const float inc = 2.f * B2_PI / (float)CIRCLE_SEG_COUNT;
    const b2Rot q = b2MakeRot(inc);

    m_vertices.clear();
    b2Vec2 c = b2TransformPoint(transform, circle.center);
    b2Vec2 r = b2MulSV(circle.radius, { 1.f, 0.f });
    for (int i = 0; i < CIRCLE_SEG_COUNT; i++)
    {
        m_vertices.push_back(b2Add(c, r));
        r = b2RotateVector(q, r);
    }
    m_vertices.push_back(m_vertices.front());
    m_vertices.push_back(
        b2TransformPoint(transform, circle.center)
    );
}

void GizmosShape::SetAsSegment(const b2Segment &segment, const b2Transform &transform)
{
    m_vertices.clear();
    m_vertices.push_back(b2TransformPoint(transform, segment.point1));
    m_vertices.push_back(b2TransformPoint(transform, segment.point2));
}

void GizmosShape::SetAsCapsule(const b2Capsule &capsule, const b2Transform &transform)
{
    m_vertices.clear();
    std::array<b2Vec2, 2> points = { capsule.center1, capsule.center2 };
    for (int i = 0; i < points.size(); i++)
    {
        points[i] = b2TransformPoint(transform, points[i]);
    }

    constexpr int ROUND_SEG_COUNT = 8;

    for (int i = 0; i < points.size(); i++)
    {
        const b2Vec2 center0 = points[(i + 0)];
        const b2Vec2 center1 = points[(i + 1) % points.size()];
        const b2Vec2 center2 = points[(i + 2) % points.size()];
        const b2Vec2 direction0 = b2Normalize(b2Sub(center1, center0));
        const b2Vec2 direction2 = b2Normalize(b2Sub(center2, center1));
        const b2Vec2 perp1 = math::PerpCW(direction0);
        const b2Vec2 perp2 = math::PerpCW(direction2);
        const b2Vec2 point0 = b2Add(center0, b2MulSV(capsule.radius, perp1));
        const b2Vec2 point1 = b2Add(center1, b2MulSV(capsule.radius, perp1));

        m_vertices.push_back(point0);
        m_vertices.push_back(point1);

        const float tau = static_cast<float>(TAU);
        const float angle = fmodf(math::SignedAngleRad(perp1, perp2) + tau, tau);
        const float inc = angle / (float)ROUND_SEG_COUNT;
        const b2Rot q = b2MakeRot(inc);

        b2Vec2 v = b2MulSV(capsule.radius, perp1);
        for (int j = 0; j < ROUND_SEG_COUNT - 1; j++)
        {
            v = b2RotateVector(q, v);
            m_vertices.push_back(b2Add(center1, v));
        }
    }
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::SetAsChain(const b2ChainId chainId, const b2Transform &transform)
{
    m_vertices.clear();
    int segmentCount = b2Chain_GetSegmentCount(chainId);
    if (segmentCount < 1) return;

    std::vector<b2ShapeId> segments(segmentCount, b2_nullShapeId);
    int count = b2Chain_GetSegments(chainId, segments.data(), segmentCount);
    assert(count == segmentCount);

    b2ChainSegment chainSegment = b2Shape_GetChainSegment(segments[0]);
    m_vertices.push_back(b2TransformPoint(transform, chainSegment.segment.point1));
    m_vertices.push_back(b2TransformPoint(transform, chainSegment.segment.point2));
    for (int i = 1; i < segmentCount; i++)
    {
        chainSegment = b2Shape_GetChainSegment(segments[i]);
        m_vertices.push_back(b2TransformPoint(transform, chainSegment.segment.point2));
    }
}

void GizmosShape::Render(
    SDL_Renderer *renderer, const Camera &camera, const Transform &cameraTransform)
{
    if (m_vertices.size() < 2) return;

    SDL_FPoint points[2] = { 0 };
    camera.WorldToView(cameraTransform.position, m_vertices[0], points[0].x, points[0].y);

    for (int i = 1; i < m_vertices.size(); ++i)
    {
        camera.WorldToView(cameraTransform.position, m_vertices[i], points[1].x, points[1].y);
        SDL_RenderLines(g_renderer, points, 2);
        points[0] = points[1];
    }
}
