
#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ecs/common/damage_utils.h"
#include "ecs/player/player_components.h"


struct ArrowTag {};

struct ArrowCommand : public EntityCommand
{
public:
    ArrowCommand(EntityCommandBuffer& ecb, Scene* scene,
        const PlayerAffiliation &playerID, const PlayerController &controller, b2Vec2 position, b2Vec2 velocity, float s, AnimType anim)
        : EntityCommand(ecb, ecb.CreateEntity())
        , m_scene(scene)
        , m_position(position)
        , m_velocity(velocity)
        , m_s(s)
        , m_affiliation(playerID)
        , m_anim(anim)
        ,m_controller(controller)
    {
    }

    virtual ~ArrowCommand() = default;

    virtual void Execute(entt::registry& registry, entt::entity entity) override;

    static void Create(
        entt::registry& registry, entt::entity entity,
        Scene* scene,
        const PlayerAffiliation &affiliation,
        const PlayerController& controller,
        b2Vec2 position, b2Vec2 velocity, float s, AnimType anim
    );

protected:
    static b2BodyId CreateBody(entt::entity entity, Scene* scene,
        const PlayerAffiliation &affiliation,
        b2Vec2 position, b2Vec2 velocity, float s, AnimType anim);

    Scene* m_scene;
    b2Vec2 m_position;
    b2Vec2 m_velocity;
    PlayerAffiliation m_affiliation;
    AnimType m_anim;
    float m_s;
    PlayerController m_controller;
};

class ArrowSystem : public System
{
public:
    ArrowSystem(Scene* scene) : System(scene, "Arrow system") {}
    virtual void OnFixedUpdate(EntityCommandBuffer& ecb) override;
    virtual void OnUpdate(EntityCommandBuffer& ecb) override;

protected:
    
};

struct ArrowComponent
{
    AnimType animType;
    PlayerController controller;
    float s;

};
