/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "utils/timer.h"
#include "utils/gizmos_shape.h"
#include "input/input_manager.h"
#include "ui/ui_object_manager.h"
#include "scene/asset_manager.h"
#include "scene/particle_system.h"
#include "ecs/command_buffer.h"
#include "ecs/basic_components.h"
#include "ecs/basic_systems.h"
#include "rendering/sprite_anim.h"
#include "imgui/imgui_manager_base.h"

#define SCENE_SAVE_QUERIES

class SceneManager;
class UICanvas;
class ParticleSystem;
struct QueryGizmos;

class RayHit
{
public:
    RayHit();

    b2ShapeId shapeId;
    entt::entity entity;
    b2Vec2 normal;
    b2Vec2 hitPoint;
    float fraction;
};

struct QueryFilter
{
    QueryFilter(
        uint64_t maskBits = (uint64_t)(-1),
        uint64_t categoryBits = (uint64_t)(-1),
        bool solidOnly = true);

    bool solidOnly;

    /// The collision category bits of this query. Normally you would just set one bit.
    uint64_t categoryBits;

    /// The collision mask bits. This states the shape categories that this
    /// query would accept for collision.
    uint64_t maskBits;
};

struct OverlapResult
{
    OverlapResult();

    b2ShapeId shapeId;
    entt::entity entity;
};

class Scene
{
public:
    Scene(SceneManager *manager, InputManager *inputManager);
    Scene(Scene const&) = delete;
    Scene& operator=(Scene const&) = delete;
    virtual ~Scene();

    void Render();
    void Update();

    SceneManager *GetSceneManager();
    InputManager *GetInputManager();
    AssetManager *GetAssetManager();
    SpriteAnimManager *GetAnimManager();
    b2WorldId GetWorld() const;
    entt::registry &GetRegistry();
    ParticleSystem *GetParticleSystem();
    UIObjectManager *GetUIObjectManager();
    std::shared_ptr<ImGuiManagerBase> GetImGuiManager();
    void SetImGuiManager(std::shared_ptr<ImGuiManagerBase> imGuiManager);

    bool Contains(UIObject *gameObject) const;

    float GetAlpha();

    void AddSimulationSystem(std::shared_ptr<System> system);
    void AddPresentationSystem(std::shared_ptr<System> system);

    std::vector<std::shared_ptr<System>> &GetSimulationSystems()
    {
        return m_simulationSystems;
    }
    std::vector<std::shared_ptr<System>> &GetPresentationSystems()
    {
        return m_presentationSystems;
    }

    UICanvas *GetCanvas();
    float GetUIPixelsPerUnit() const;

    const Timer &GetTime() const;
    Timer &GetTime();
    const float GetElapsed() const;
    const Uint64 GetElapsedMS() const;
    const float GetDelta() const;
    const Uint64 GetDeltaMS() const;
    const float GetTimeStep() const;
    const Uint64 GetTimeStepMS() const;
    const float GetUnscaledDelta() const;
    const Uint64 GetUnscaledDeltaMS() const;

    std::set<QueryGizmos *> &GetQueryGizmos()
    {
        return m_queryGizmos;
    }

    RayHit RayCastFirst(
        b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter
    );
    RayHit RayCastFirstGizmos(
        b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter,
        Color defaultColor = Color(0, 255, 0),
        Color hitColor = Color(0, 255, 255)
    );

    void RayCast(
        b2Vec2 point1, b2Vec2 point2,
        const QueryFilter &filter, std::vector<RayHit> &result
    );
    void RayCastGizmos(
        b2Vec2 point1, b2Vec2 point2,
        const QueryFilter &filter, std::vector<RayHit> &result,
        Color defaultColor = Color(0, 255, 0),
        Color hitColor = Color(0, 255, 255)
    );

    void OverlapAABB(
        const b2AABB &aabb,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapAABBGizmos(
        const b2AABB &aabb,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    void OverlapCircle(
        b2Vec2 center, float radius,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapCircleGizmos(
        b2Vec2 center, float radius,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    void OverlapPolygon(
        const b2Vec2 *vertices, int vertexCount,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapPolygonGizmos(
        const b2Vec2 *vertices, int vertexCount,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    enum class UpdateMode
    {
        REALTIME,
        STEP_BY_STEP
    };
    UpdateMode GetUpdateMode() const;
    void SetUpdateMode(UpdateMode mode);

    void MakeStep();
    Uint64 GetUpdateID() const;

protected:
    entt::registry m_registry;

    SceneManager *m_sceneManager;

    /// @brief Mode de mise à jour du moteur physique de la scène (temps réel ou pas-à-pas).
    UpdateMode m_mode;

    Timer m_time;

    /// @brief Moteur physique.
    b2WorldId m_worldId;

    /// @brief Gestionnaire des instances de UIObject.
    UIObjectManager m_uiObjectManager;

    /// @brief Gestionnaire des entrées utilisateur.
    InputManager *m_inputManager;

    /// @brief Gestionnaire des ressources de la scène.
    AssetManager m_assetManager;

    SpriteAnimManager m_animManager;

    ParticleSystem m_particleSystem;

    UICanvas *m_canvas;

    std::shared_ptr<ImGuiManagerBase> m_imGuiManager;

    /// @brief Pas de temps fixe.
    Uint64 m_timeStepMS;

    /// @brief Accumulateur pour la mise à jour à pas de temps fixe.
    Uint64 m_stepAccuMS;

    Uint64 m_updateID;

    /// @brief Paramètre d'interpolation pour les positions des corps physiques.
    float m_alpha;

    /// @brief Booléen indiquant s'il faut effectuer un pas fixe en mode pas-à-pas.
    bool m_makeStep;

    /// @brief Booléen indiquant s'il faut quitter la scène.
    bool m_quit;

    void UpdateGameObjects();
    void MakeFixedStep();

    bool m_inFixedUpdate;

    std::set<QueryGizmos *> m_queryGizmos;
    std::vector<std::shared_ptr<System>> m_simulationSystems;
    std::vector<std::shared_ptr<System>> m_presentationSystems;
    EntityCommandBuffer m_entityCommandBuffer;
};

inline SceneManager *Scene::GetSceneManager()
{
    return m_sceneManager;
}

inline InputManager *Scene::GetInputManager()
{
    return m_inputManager;
}

inline AssetManager *Scene::GetAssetManager()
{
    return &m_assetManager;
}

inline SpriteAnimManager *Scene::GetAnimManager()
{
    return &m_animManager;
}

inline b2WorldId Scene::GetWorld() const
{
    return m_worldId;
}

inline entt::registry &Scene::GetRegistry()
{
    return m_registry;
}

inline ParticleSystem *Scene::GetParticleSystem()
{
    return &m_particleSystem;
}

inline UIObjectManager *Scene::GetUIObjectManager()
{
    return &m_uiObjectManager;
}

inline std::shared_ptr<ImGuiManagerBase> Scene::GetImGuiManager()
{
    return m_imGuiManager;
}

inline void Scene::SetImGuiManager(std::shared_ptr<ImGuiManagerBase> imGuiManager)
{
    m_imGuiManager = imGuiManager;
}

inline float Scene::GetAlpha()
{
    return m_alpha;
}

inline void Scene::AddSimulationSystem(std::shared_ptr<System> system)
{
    m_simulationSystems.emplace_back(std::move(system));
}

inline void Scene::AddPresentationSystem(std::shared_ptr<System> system)
{
    m_presentationSystems.emplace_back(std::move(system));
}

inline UICanvas *Scene::GetCanvas()
{
    return m_canvas;
}

inline const Timer &Scene::GetTime() const
{
    return m_time;
}

inline Timer &Scene::GetTime()
{
    return m_time;
}

inline const float Scene::GetElapsed() const
{
    return m_time.GetElapsed();
}

inline const Uint64 Scene::GetElapsedMS() const
{
    return m_time.GetElapsedMS();
}

inline const float Scene::GetDelta() const
{
    return m_inFixedUpdate ? (static_cast<float>(m_timeStepMS) / 1000.f) : m_time.GetDelta();
}

inline const Uint64 Scene::GetDeltaMS() const
{
    return m_inFixedUpdate ? m_timeStepMS : m_time.GetDeltaMS();
}

inline const float Scene::GetUnscaledDelta() const
{
    return m_inFixedUpdate ? (static_cast<float>(m_timeStepMS) / 1000.f) : m_time.GetUnscaledDelta();
}

inline const Uint64 Scene::GetUnscaledDeltaMS() const
{
    return m_inFixedUpdate ? m_timeStepMS : m_time.GetUnscaledDeltaMS();
}

inline const float Scene::GetTimeStep() const
{
    return static_cast<float>(m_timeStepMS) / 1000.f;
}

inline const Uint64 Scene::GetTimeStepMS() const
{
    return m_timeStepMS;
}

inline Scene::UpdateMode Scene::GetUpdateMode() const
{
    return m_mode;
}

inline void Scene::SetUpdateMode(UpdateMode mode)
{
    m_mode = mode;
}

inline void Scene::MakeStep()
{
    m_makeStep = true;
}

inline bool Scene::Contains(UIObject *gameObject) const
{
    return m_uiObjectManager.Contains(gameObject);
}

inline Uint64 Scene::GetUpdateID() const
{
    return m_updateID;
}

struct QueryGizmos
{
    QueryGizmos(Color color, GizmosShape shape) :
        m_color(color), m_shape(shape) { }

    Color m_color;
    GizmosShape m_shape;
};

