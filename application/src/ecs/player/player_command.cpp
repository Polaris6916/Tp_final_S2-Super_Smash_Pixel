/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_command.h"
#include "ecs/player/player_components.h"
#include "ecs/player/shield.h"
#include "ecs/player/player_ai.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"
#include "ecs/common/visual_id.h"
#include "ecs/player/leaf_ranger_arrow.h"
#include "ecs/player/metal_bladekeeper_proj.h"

void PlayerCommand::Execute(entt::registry &registry, entt::entity entity)
{
    Create(registry, entity, m_scene, m_playerID, m_position);
}

void PlayerCommand::Create(
    entt::registry &registry, entt::entity entity, Scene *scene, int playerID, b2Vec2 position)
{
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnimManager *animManager = scene->GetAnimManager();
    PlayerConfig *config = g_gameCommon.GetPlayerConfig(playerID);
    if (config == nullptr)
    {
        assert(false);
        return;
    }

    uint32_t animId = AnimID_Make(config->category, AnimType::IDLE);
    spriteGroup = animManager->GetSpriteGroup(animId);
    AssertNew(spriteGroup);

    // TODO 8 - Modifiez les dimensions du sprite pour 22 pixels par unité
    //      - Modifiez l'offset pour 0.0 en X et -0.1 en Y.

    Sprite spriteComponent;
    spriteComponent.anchor = Anchor::SOUTH;
    spriteComponent.texture = spriteGroup->GetTexture();
    spriteComponent.srcRect = *spriteGroup->GetSourceRect(0);
    spriteComponent.dimensions = {
        spriteComponent.srcRect.w / 22.f,
        spriteComponent.srcRect.h / 22.f
    };
    spriteComponent.offset = b2Vec2{ 0.f, -0.1f };

    RenderSortingLayer renderLayer;
    renderLayer.enabled = true;
    renderLayer.layer = LAYER_PLAYER;
    renderLayer.orderInLayer = 5 + config->playerID;

    b2BodyId bodyId = CreateBody(entity, scene, config, position);

    PlayerAffiliation affiliation{ playerID };

    std::string name = "Player";
    switch (config->type)
    {
    case PlayerType::FIRE_KNIGHT: name = "Fire knight"; break;
    case PlayerType::WATER_PRIESTESS: name = "Water priestess"; break;
    case PlayerType::LEAF_RANGER: name = "Leaf Ranger"; break;
	case PlayerType::METAL_BLADEKEEPER: name = "Metal Bladekeeper"; break;
    default: assert(false); break;
    }

    registry.emplace<NameComponent>(entity, name);
    registry.emplace<Transform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<FixedUpdateTransform>(entity, b2Body_GetTransform(bodyId));
    registry.emplace<Rigidbody>(entity, bodyId);
    registry.emplace<Sprite>(entity, spriteComponent);
    registry.emplace<RenderSortingLayer>(entity, renderLayer);

    registry.emplace<SpriteAnimState>(entity, animId, true);
    registry.emplace<Damageable>(entity);
    registry.emplace<GroundContact>(entity);

    registry.emplace<PlayerAffiliation>(entity, affiliation);
    registry.emplace<PlayerController>(entity, config->type);
    registry.emplace<PlayerControllerInput>(entity);
    registry.emplace<PlayerAnimInfo>(entity);
    registry.emplace<OneWayPass>(entity);

    if (config->isCPU)
    {
        registry.emplace<PlayerAI>(entity, config->level);
        registry.emplace<TrackedTarget>(entity);
    }

    switch (config->type)
    {
    case PlayerType::FIRE_KNIGHT: registry.emplace<FireKnightTag>(entity); break;
    case PlayerType::WATER_PRIESTESS: registry.emplace<WaterPriestessTag>(entity); break;
    case PlayerType::LEAF_RANGER: registry.emplace<LeafRangerTag>(entity); break;
	case PlayerType::METAL_BLADEKEEPER: registry.emplace<MetalBladekeeperTag>(entity); break;
    default: assert(false); break;
    }

    ShieldCommand::Create(registry, registry.create(), scene, entity);
    VisualIDCommand::Create(registry, registry.create(), scene, entity, affiliation, b2Vec2{ 0.f, 2.8f });
}

b2BodyId PlayerCommand::CreateBody(entt::entity entity, Scene *scene, const PlayerConfig *config, b2Vec2 position)
{
    b2WorldId worldId = scene->GetWorld();
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.fixedRotation = true;
    bodyDef.enableSleep = false;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    assert(b2Body_IsValid(bodyId));

    b2Capsule capsule = { 0 };
    if (config->type == PlayerType::FIRE_KNIGHT)
    {
        // TODO 2 - Donnez les valeurs suggérées dans le sujet.

        capsule.center1 = b2Vec2{ 0.f, 0.45f };
        capsule.center2 = b2Vec2{ 0.f, 1.15f };
    }
    else if (config->type == PlayerType::WATER_PRIESTESS)
    {
        // TODO 18 - A modifier lors de la création du second personnage à la fin du tuto.

        capsule.center1 = b2Vec2{ 0.f, 0.45f };
        capsule.center2 = b2Vec2{ 0.f, 1.15f };
    }
    else if (config->type == PlayerType::LEAF_RANGER)
    {
        // TODO 18 - A modifier lors de la création du second personnage à la fin du tuto.

        capsule.center1 = b2Vec2{ 0.f, 0.45f };
        capsule.center2 = b2Vec2{ 0.f, 1.15f };
    }
    else if (config->type == PlayerType::METAL_BLADEKEEPER)
    {
        // TODO 18 - A modifier lors de la création du second personnage à la fin du tuto.

        capsule.center1 = b2Vec2{ 0.f, 0.45f };
        capsule.center2 = b2Vec2{ 0.f, 1.15f };
    }
    // TODO 2 - Donnez les valeurs suggérées dans le sujet.

    capsule.radius = 0.45f;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.f;
    shapeDef.filter.categoryBits = config->teamMask;
    shapeDef.filter.maskBits = CATEGORY_PROJECTILE | CATEGORY_TERRAIN | CATEGORY_ATTACK;
    shapeDef.material.friction = 0.2f;
    shapeDef.material.restitution = 0.f;
    shapeDef.enableContactEvents = true;
    shapeDef.enableSensorEvents = true;

    b2ShapeId shapeId = b2_nullShapeId;
    shapeId = b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);

    return bodyId;
}
