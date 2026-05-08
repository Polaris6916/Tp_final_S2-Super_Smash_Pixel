/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "scene/scene.h"
#include "ui/ui_object_manager.h"
#include "ui/ui_object.h"
#include "ui/base/ui_canvas.h"
#include "scene/scene_manager.h"

#define TIME_STEP_MS 16

RayHit::RayHit()
    : shapeId(b2_nullShapeId)
    , fraction(0.f)
    , normal(b2Vec2_zero)
    , hitPoint(b2Vec2_zero)
    , entity(entt::null)
{}

QueryFilter::QueryFilter(uint64_t maskBits, uint64_t categoryBits, bool solidOnly)
    : categoryBits(categoryBits)
    , maskBits(maskBits)
    , solidOnly(solidOnly)
{}

OverlapResult::OverlapResult()
    : shapeId(b2_nullShapeId)
    , entity(entt::null)
{}

Scene::Scene(SceneManager *manager, InputManager *inputManager)
    : m_sceneManager(manager)
    , m_inputManager(inputManager)
    , m_stepAccuMS(0)
    , m_alpha(0.f)
    , m_makeStep(false)
    , m_mode(UpdateMode::REALTIME)
    , m_uiObjectManager()
    , m_quit(false)
    , m_timeStepMS(TIME_STEP_MS)
    , m_inFixedUpdate(false)
    , m_time()
    , m_assetManager()
    , m_particleSystem(this)
    , m_worldId(b2_nullWorldId)
    , m_queryGizmos()
    , m_updateID(0)
    , m_entityCommandBuffer()
    , m_registry()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0.0f, -40.0f };
    worldDef.enableSleep = false;
    worldDef.enableContinuous = true;
    m_worldId = b2CreateWorld(&worldDef);

    RigidbodyUtils::AttachConstruct(m_registry);
    RigidbodyUtils::AttachDestroy(m_registry);

    m_canvas = nullptr;
    m_canvas = new UICanvas(this);

    if (inputManager == nullptr)
    {
        assert(false);
        abort();
    }
}

Scene::~Scene()
{
    m_registry.clear();
    m_uiObjectManager.DeleteObjects();
    b2DestroyWorld(m_worldId);
}

void Scene::Render()
{
    for (auto &system : m_presentationSystems)
    {
        if (system->enabled == false) continue;
        system->OnUpdate(m_entityCommandBuffer);
        m_entityCommandBuffer.Flush(m_registry);
    }

    if (m_sceneManager) m_sceneManager->OnSceneRender();
}

void Scene::Update()
{
    m_updateID++;

    // Appelle les méthodes asynchrones
    // Messages : Start(), Delete()
    m_uiObjectManager.ProcessObjects();

    // Met à jour les entrées de l'utilisateur
    m_inputManager->ProcessEvents();

    m_particleSystem.Update();

    // Met à jour les entités et les UIObjects
    UpdateGameObjects();
}

void Scene::MakeFixedStep()
{
    const float timeStep = (float)m_timeStepMS / 1000.f;
    m_inFixedUpdate = true;

    for (QueryGizmos *autoGizmos : m_queryGizmos) delete autoGizmos;
    m_queryGizmos.clear();

    // World
    int32_t subSteps = 4;
    b2World_Step(m_worldId, timeStep, subSteps);

    // ECS
    for (auto &system : m_simulationSystems)
    {
        if (system->enabled == false) continue;
        system->OnFixedUpdate(m_entityCommandBuffer);
        m_entityCommandBuffer.Flush(m_registry);
    }

    // Scene manager
    if (m_sceneManager) m_sceneManager->OnSceneFixedUpdate();

    m_inFixedUpdate = false;
}

void Scene::UpdateGameObjects()
{
    //--------------------------------------------------------------------------
    // FIXED UPDATE

    if (m_mode == UpdateMode::REALTIME)
    {
        // Mode temps réel
        m_time.Update();

        m_stepAccuMS += m_time.GetDeltaMS();
        while (m_stepAccuMS >= m_timeStepMS)
        {
            MakeFixedStep();
            m_stepAccuMS -= m_timeStepMS;
        }
        m_alpha = (float)m_stepAccuMS / (float)m_timeStepMS;
    }
    else
    {
        // Mode pas à pas
        if (m_makeStep)
        {
            m_time.Update(m_timeStepMS);
            MakeFixedStep();
        }
        else
        {
            m_time.Update(0.f);
        }
        m_makeStep = false;
        m_alpha = 1.f;
    }

    //--------------------------------------------------------------------------
    // UPDATE

    // ECS
    for (auto &system : m_simulationSystems)
    {
        if (system->enabled == false) continue;
        system->OnUpdate(m_entityCommandBuffer);
        m_entityCommandBuffer.Flush(m_registry);
    }

    // UIObjects
    for (auto it = m_uiObjectManager.begin();
         it != m_uiObjectManager.end(); ++it)
    {
        UIObject *object = *it;
        object->Update();
    }

    // Scene manager
    if (m_sceneManager) m_sceneManager->OnSceneUpdate();
}

class RayContext
{
public:
    RayContext(const QueryFilter &filter)
        : rayHits{}
        , filter(filter)
        , count(0)
    {}

    int count;
    std::array<RayHit, 16> rayHits;
    const QueryFilter &filter;

    static float ClosestCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context)
    {
        RayContext *rayContext = static_cast<RayContext *>(context);
        const QueryFilter &filter = rayContext->filter;

        if (filter.solidOnly && b2Shape_IsSensor(shapeId))
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }
        const b2BodyId bodyId = b2Shape_GetBody(shapeId);
        BodyUserData *userData = static_cast<BodyUserData *>(b2Body_GetUserData(bodyId));

        RayHit *rayHit = &(rayContext->rayHits[0]);
        rayHit->shapeId = shapeId;
        rayHit->hitPoint = point;
        rayHit->normal = normal;
        rayHit->entity = userData->entity;
        rayHit->fraction = fraction;
        rayContext->count = 1;

        // By returning the current fraction, we instruct the calling code to clip the ray and
        // continue the ray-cast to the next shape. WARNING: do not assume that shapes
        // are reported in order. However, by clipping, we can always get the closest shape.
        return fraction;
    }

    static float MultipleCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context)
    {
        RayContext *rayContext = static_cast<RayContext *>(context);
        const QueryFilter &filter = rayContext->filter;

        if (rayContext->count >= rayContext->rayHits.size())
        {
            // At this point the buffer is full.
            // By returning 0, we instruct the calling code to terminate the ray-cast.
            return 0.0f;
        }

        if (filter.solidOnly && b2Shape_IsSensor(shapeId))
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        const b2BodyId bodyId = b2Shape_GetBody(shapeId);
        BodyUserData *userData = static_cast<BodyUserData *>(b2Body_GetUserData(bodyId));

        RayHit *rayHit = &(rayContext->rayHits[rayContext->count]);
        rayHit->shapeId = shapeId;
        rayHit->hitPoint = point;
        rayHit->normal = normal;
        rayHit->entity = userData->entity;
        rayHit->fraction = fraction;
        rayContext->count++;

        // By returning 1, we instruct the caller to continue without clipping the ray.
        return 1.0f;
    }
};

RayHit Scene::RayCastFirst(b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter)
{
    RayContext rayContext(filter);
    b2QueryFilter queryFilter = b2DefaultQueryFilter();
    queryFilter.categoryBits = filter.categoryBits;
    queryFilter.maskBits = filter.maskBits;

    b2World_CastRay(m_worldId, point1, point2 - point1, queryFilter, RayContext::ClosestCallback, &rayContext);

    return rayContext.rayHits[0];
}

RayHit Scene::RayCastFirstGizmos(
    b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter,
    Color defaultColor, Color hitColor)
{
    RayHit firstHit = RayCastFirst(point1, point2, filter);

#ifdef SCENE_SAVE_QUERIES
    b2ShapeId shapeId = firstHit.shapeId;
    if (b2Shape_IsValid(shapeId))
    {
        m_queryGizmos.insert(new QueryGizmos(hitColor, GizmosShape(shapeId)));
        m_queryGizmos.insert(new QueryGizmos(hitColor, GizmosShape(point1, point2)));
    }
    else
    {
        m_queryGizmos.insert(new QueryGizmos(defaultColor, GizmosShape(point1, point2)));

    }
#endif

    return firstHit;
}

void Scene::RayCast(
    b2Vec2 point1, b2Vec2 point2,
    const QueryFilter &filter, std::vector<RayHit> &result)
{
    result.clear();

    RayContext rayContext(filter);
    b2QueryFilter queryFilter = b2DefaultQueryFilter();
    queryFilter.categoryBits = filter.categoryBits;
    queryFilter.maskBits = filter.maskBits;

    b2World_CastRay(m_worldId, point1, point2 - point1, queryFilter, RayContext::MultipleCallback, &rayContext);

    for (int i = 0; i < rayContext.count; i++)
    {
        result.push_back(rayContext.rayHits[i]);
    }
}

void Scene::RayCastGizmos(
    b2Vec2 point1, b2Vec2 point2,
    const QueryFilter &filter, std::vector<RayHit> &result,
    Color defaultColor, Color hitColor)
{
    RayCast(point1, point2, filter, result);

#ifdef SCENE_SAVE_QUERIES
    Color color = result.empty() ? defaultColor : hitColor;
    m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(point1, point2)));
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        const RayHit &rayHit = *it;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(rayHit.shapeId)));
    }
#endif
}

class OverlapContext
{
public:
    OverlapContext(const QueryFilter &filter)
        : overlaps{}
        , filter(filter)
        , count(0)
    {}

    int count;
    std::array<OverlapResult, 16> overlaps;
    const QueryFilter &filter;

    static bool MultipleCallback(b2ShapeId shapeId, void *context)
    {
        OverlapContext *overlapContext = static_cast<OverlapContext *>(context);
        const QueryFilter &filter = overlapContext->filter;

        if (overlapContext->count >= overlapContext->overlaps.size())
        {
            return false;
        }

        if (filter.solidOnly && b2Shape_IsSensor(shapeId))
        {
            return true;
        }

        const b2BodyId bodyId = b2Shape_GetBody(shapeId);
        BodyUserData *userData = static_cast<BodyUserData *>(b2Body_GetUserData(bodyId));

        OverlapResult *overlapRes = &(overlapContext->overlaps[overlapContext->count]);
        overlapRes->shapeId = shapeId;
        overlapRes->entity = userData->entity;
        overlapContext->count++;

        return true;
    }
};

void Scene::OverlapAABB(
    const b2AABB &aabb,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    result.clear();

    OverlapContext overlapContext(filter);
    b2QueryFilter queryFilter = b2DefaultQueryFilter();
    queryFilter.categoryBits = filter.categoryBits;
    queryFilter.maskBits = filter.maskBits;

    b2World_OverlapAABB(m_worldId, aabb, queryFilter, OverlapContext::MultipleCallback, &overlapContext);

    for (int i = 0; i < overlapContext.count; i++)
    {
        result.push_back(overlapContext.overlaps[i]);
    }
}

void Scene::OverlapAABBGizmos(
    const b2AABB &aabb,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapAABB(aabb, filter, result);

#ifdef SCENE_SAVE_QUERIES
    Color color = result.empty() ? defaultColor : hitColor;
    m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(aabb)));
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        const OverlapResult &overlap = *it;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.shapeId)));
    }
#endif
}

void Scene::OverlapCircle(
    b2Vec2 center, float radius,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    result.clear();

    OverlapContext overlapContext(filter);
    b2QueryFilter queryFilter = b2DefaultQueryFilter();
    queryFilter.categoryBits = filter.categoryBits;
    queryFilter.maskBits = filter.maskBits;

    b2ShapeProxy shapeProxy = b2MakeProxy(&center, 1, radius);
    b2World_OverlapShape(m_worldId, &shapeProxy, queryFilter, OverlapContext::MultipleCallback, &overlapContext);

    for (int i = 0; i < overlapContext.count; i++)
    {
        result.push_back(overlapContext.overlaps[i]);
    }
}

void Scene::OverlapCircleGizmos(
    b2Vec2 center, float radius,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapCircle(center, radius, filter, result);

#ifdef SCENE_SAVE_QUERIES
    GizmosShape shape;
    b2Transform xf = b2Transform_identity;
    b2Circle circle = { 0 };
    circle.center = center;
    circle.radius = radius;
    shape.SetAsCircle(circle, xf);

    Color color = result.empty() ? defaultColor : hitColor;
    m_queryGizmos.insert(new QueryGizmos(color, shape));
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        const OverlapResult &overlap = *it;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.shapeId)));
    }
#endif
}

void Scene::OverlapPolygon(
    const b2Vec2 *vertices, int vertexCount,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    assert(vertices && (2 < vertexCount) && (vertexCount < B2_MAX_POLYGON_VERTICES));
    result.clear();

    OverlapContext overlapContext(filter);
    b2QueryFilter queryFilter = b2DefaultQueryFilter();
    queryFilter.categoryBits = filter.categoryBits;
    queryFilter.maskBits = filter.maskBits;

    b2Hull hull = b2ComputeHull(vertices, vertexCount);
    b2ShapeProxy shapeProxy = b2MakeProxy(hull.points, hull.count, 0.f);
    b2World_OverlapShape(m_worldId, &shapeProxy, queryFilter, OverlapContext::MultipleCallback, &overlapContext);

    for (int i = 0; i < overlapContext.count; i++)
    {
        result.push_back(overlapContext.overlaps[i]);
    }
}

void Scene::OverlapPolygonGizmos(
    const b2Vec2 *vertices, int vertexCount,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapPolygon(vertices, vertexCount, filter, result);

#ifdef SCENE_SAVE_QUERIES
    GizmosShape shape;
    b2Transform xf = b2Transform_identity;
    b2Hull hull = b2ComputeHull(vertices, vertexCount);
    b2Polygon polygonShape = b2MakePolygon(&hull, 0.f);
    shape.SetAsPolygon(polygonShape, xf);

    Color color = result.empty() ? defaultColor : hitColor;
    m_queryGizmos.insert(new QueryGizmos(color, shape));
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        const OverlapResult &overlap = *it;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.shapeId)));
    }
#endif
}

float Scene::GetUIPixelsPerUnit() const
{
    if (m_canvas) return m_canvas->GetPixelsPerUnit();
    return 1.0f;
}
