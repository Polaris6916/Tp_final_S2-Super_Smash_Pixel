/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/basic_systems.h"
#include "scene/scene.h"
#include "utils/utils.h"
#include "ui/ui_object.h"
#include "imgui/imgui_manager_base.h"

System::System(Scene *scene, const std::string &systemName)
    : name(systemName)
    , m_scene(scene)
    , m_registry(scene->GetRegistry())
    , enabled(true)
{
}

void RenderGameSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    ParticleSystem *particleSystem = m_scene->GetParticleSystem();

    m_registry.sort<RenderSortingLayer>([](const RenderSortingLayer& a, const RenderSortingLayer& b) {
        if (a.layer != b.layer) return (a.layer < b.layer);
        return (a.orderInLayer < b.orderInLayer);
    });

    auto renderableView = m_registry.view<RenderSortingLayer, Transform>();
    renderableView.use<RenderSortingLayer>();

    auto cameraView = m_registry.view<Camera, Transform>();

    for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
    {
        if (camera.isActive == false) continue;

        camera.UpdateViewport(g_renderer);

        int prevLayer = 0;
        for (auto [entity, renderLayer, transform] : renderableView.each())
        {
            if (renderLayer.enabled == false) continue;

            if (renderLayer.layer > prevLayer)
            {
                particleSystem->renderParticleLayers(camera, cameraTransform, prevLayer, renderLayer.layer);
                prevLayer = renderLayer.layer;
            }

            if (m_registry.all_of<Sprite>(entity))
            {
                auto &sprite = m_registry.get<Sprite>(entity);
                if (m_registry.all_of<RenderBlendMod>(entity))
                {
                    auto &blendMode = m_registry.get<RenderBlendMod>(entity);
                    SDL_SetTextureBlendMode(sprite.texture, blendMode.blendMode);
                    SDL_SetTextureAlphaModFloat(sprite.texture, blendMode.alpha);
                }
                if (m_registry.all_of<RenderColorMod>(entity))
                {
                    auto &colorMod = m_registry.get<RenderColorMod>(entity);
                    SDL_SetTextureColorModFloat(sprite.texture, colorMod.r, colorMod.g, colorMod.b);
                }

                if (m_registry.all_of<TiledSprite>(entity))
                {
                    auto &tiledSprite = m_registry.get<TiledSprite>(entity);
                    RenderSprite(
                        camera, cameraTransform, sprite, transform,
                        1.f, tiledSprite.tiledCountX, tiledSprite.tiledCountY
                    );
                }
                else
                {
                    RenderSprite(camera, cameraTransform, sprite, transform);
                }
            }

            if (m_registry.all_of<TilemapRenderer>(entity))
            {
                auto &tilemap = m_registry.get<TilemapRenderer>(entity);
                for (Sprite &sprite : tilemap.tiles)
                {
                    RenderSprite(camera, cameraTransform, sprite, transform, tilemap.scale);
                }
            }
            if (m_registry.all_of<BackgroundLayer>(entity))
            {
                auto &layer = m_registry.get<BackgroundLayer>(entity);
                RenderLayer(camera, cameraTransform, layer, transform);
            }
        }

        particleSystem->renderParticleLayers(camera, cameraTransform, prevLayer, INT_MAX);
    }
}

//  dessine ce qui est lien avec les sprites et les positions 
void RenderGameSystem::RenderSprite(
    const Camera &camera, const Transform &cameraTransform,
    const Sprite &sprite, const Transform &spriteTransform,
    float scale, int tiledCountX, int tiledCountY)
{
    if (sprite.texture == nullptr) return;

    const float scaleX = camera.GetWorldToViewScaleX();
    const float scaleY = camera.GetWorldToViewScaleY();
    const SDL_FRect *srcRect = &(sprite.srcRect);
    SDL_FRect dstRect = { 0 };
    b2Vec2 sign{ 1.f, 1.f };
    if (sprite.flipOffset)
    {
        if (sprite.flip & SDL_FLIP_HORIZONTAL) sign.x = -1.f;
        if (sprite.flip & SDL_FLIP_VERTICAL) sign.y = -1.f;
    }

    b2Vec2 spriteDim{
        scale * sprite.dimensions.x,
        scale * sprite.dimensions.y
    };
    b2Vec2 tileDim{
        scale * sprite.dimensions.x / tiledCountX,
        scale * sprite.dimensions.y / tiledCountY
    };
    const float tileDstW = scaleX * tileDim.x;
    const float tileDstH = scaleY * tileDim.y;

    for (int x = 0; x < tiledCountX; x++)
    {
        for (int y = 0; y < tiledCountY; y++)
        {
            b2Vec2 offset = b2Mul(b2Vec2{ (float)x, (float)-y }, tileDim)
                + b2Mul(scale * sprite.offset, sign);

            switch (sprite.anchor)
            {
            case Anchor::NORTH_WEST:
                break;
            case Anchor::WEST:
                offset.y += 0.5f * spriteDim.y;
                break;
            case Anchor::SOUTH_WEST:
                offset.y += spriteDim.y;
                break;
            case Anchor::NORTH:
                offset.x -= 0.5f * spriteDim.x;
                break;
            case Anchor::CENTER:
                offset.x -= 0.5f * spriteDim.x;
                offset.y += 0.5f * spriteDim.y;
                break;
            case Anchor::SOUTH:
                offset.x -= 0.5f * spriteDim.x;
                offset.y += spriteDim.y;
                break;
            case Anchor::NORTH_EAST:
                offset.x -= spriteDim.x;
                break;
            case Anchor::EAST:
                offset.x -= spriteDim.x;
                offset.y += 0.5f * spriteDim.y;
                break;
            case Anchor::SOUTH_EAST:
                offset.x -= spriteDim.x;
                offset.y += spriteDim.y;
                break;
            default:
                break;
            }
            b2Transform xf{ spriteTransform.position, b2MakeRot(spriteTransform.angle) };
            b2Vec2 position = b2TransformPoint(xf, offset);
            dstRect.x = camera.WorldToViewX(cameraTransform.position.x, position.x);
            dstRect.y = camera.WorldToViewY(cameraTransform.position.y, position.y);
            dstRect.w = tileDstW;
            dstRect.h = tileDstH;

            SDL_FPoint sdlCenter{ 0.f, 0.f };
            SDL_RenderTextureRotated(
                g_renderer, sprite.texture, srcRect, &dstRect,
                -spriteTransform.angle * RAD_TO_DEG, &sdlCenter, sprite.flip
            );
        }
    }
}

void RenderGameSystem::RenderLayer(
    const Camera &camera, const Transform &cameraTransform,
    const BackgroundLayer &layer, const Transform &layerTransform)
{
    if (layer.texture == nullptr) return;

    // Dimension du fond dans le référentiel monde
    const float layerW = camera.GetWorldToViewScaleX() * layer.dimensions.x;
    const float layerH = camera.GetWorldToViewScaleY() * layer.dimensions.y;

    b2Vec2 layerWest = cameraTransform.position - layerTransform.position;
    layerWest.x *= (1.f - layer.shiftFactor.x);
    layerWest.y *= (1.f - layer.shiftFactor.y);
    layerWest += layerTransform.position;

    float x = camera.WorldToViewX(cameraTransform.position.x, layerWest.x);
    float y = camera.WorldToViewY(cameraTransform.position.y, layerWest.y);
    x = fmodf(x, layerW);

    for (x = x - layerW; x < camera.viewport.w; x += layerW)
    {
        SDL_FRect dstRect = { 0 };
        dstRect.x = x;
        dstRect.y = y;
        dstRect.w = layerW;
        dstRect.h = layerH;

        switch (layer.mode)
        {
        default:
        case LayerMode::NO_FILL:
            break;
        case LayerMode::FILL_ABOVE:
            FillLayerAbove(layer.texture, dstRect);
            break;
        case LayerMode::FILL_BELOW:
            FillLayerBelow(layer.texture, dstRect);
            break;
        case LayerMode::FILL_VERTICAL:
            FillLayerAbove(layer.texture, dstRect);
            FillLayerBelow(layer.texture, dstRect);
            break;
        }
        RenderTexture(g_renderer, layer.texture, NULL, &dstRect, Anchor::WEST);
    }
}

inline void RenderGameSystem::FillLayerAbove(SDL_Texture *layer, SDL_FRect dstRect)
{
    SDL_FRect srcRect = { 0 };
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = static_cast<float>(layer->w);
    srcRect.h = 1;

    dstRect.h = dstRect.y - 0.5f * dstRect.h;
    dstRect.y = 0.f;

    if (dstRect.h > 0.f)
    {
        SDL_RenderTexture(g_renderer, layer, &srcRect, &dstRect);
    }
}

inline void RenderGameSystem::FillLayerBelow(SDL_Texture *layer, SDL_FRect dstRect)
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);

    SDL_FRect srcRect = { 0 };
    srcRect.x = 0;
    srcRect.y = static_cast<float>(layer->h) - 1.f;
    srcRect.w = static_cast<float>(layer->w);
    srcRect.h = 1;

    dstRect.y += 0.5f * dstRect.h;
    dstRect.h = viewport.h - dstRect.y;

    if (dstRect.h > 0.f)
    {
        SDL_RenderTexture(g_renderer, layer, &srcRect, &dstRect);
    }
}

//  met à jour les Tr et les FUTr qui sont en lien avec un RB ou un LTr
void TransformSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    const float alpha = m_scene->GetAlpha();

    auto view = m_registry.view<Transform, FixedUpdateTransform>(
        entt::exclude<LocalTransform>);
    for (auto [entity, transform, fixed] : view.each())
    {
        transform.position = math::Lerp(fixed.prevXf.p, fixed.currXf.p, alpha);
        transform.angle = math::LerpAngle(
            b2Rot_GetAngle(fixed.prevXf.q), b2Rot_GetAngle(fixed.currXf.q), alpha
        );
    }

    auto viewLocal = m_registry.view<LocalTransform, Transform>();
    for (auto [entity, local, transform] : viewLocal.each())
    {
        if (m_registry.valid(local.parent) && m_registry.all_of<Transform>(local.parent))
        {
            Transform &parentTransform = m_registry.get<Transform>(local.parent);
            transform.position = parentTransform.position + local.position;
            transform.angle = fmodf(parentTransform.angle + local.angle, (float)TAU);
        }
    }
}

void TransformSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<Rigidbody, FixedUpdateTransform>();
    for (auto [entity, rigidBody, fixedTransform] : view.each())
    {
        b2BodyId bodyId = rigidBody.bodyId;
        if (b2Body_IsValid(bodyId) == false) continue;

        fixedTransform.prevXf = fixedTransform.currXf;
        fixedTransform.currXf = b2Body_GetTransform(bodyId);
    }
}

// destruction des corps trop loin des bornes du jeu
void WorldBoundsSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<Rigidbody, FixedUpdateTransform>();
    for (auto [entity, rigidBody, fixedTransform] : view.each())
    {
        b2BodyId bodyId = rigidBody.bodyId;
        if (b2Body_IsValid(bodyId) == false)
        {
            ecb.DestroyEntity(entity);
            continue;
        }
        const b2Vec2 position = b2Body_GetPosition(bodyId);
        if (math::PointInAABB(position, bounds) == false)
        {
            ecb.DestroyEntity(entity);
            continue;
        }
    }
}

// met à jour les sprites d’une animation
void AnimatorSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    const float delta = m_scene->GetDelta();

    SpriteAnimManager *animManager = m_scene->GetAnimManager();
    auto view = m_registry.view<Sprite, SpriteAnimState>();
    for (auto [entity, sprite, state] : view.each())
    {
        SpriteAnim *spriteAnim = animManager->GetSpriteAnim(state);
        if (spriteAnim == nullptr) continue;
        SpriteGroup *spriteGroup = spriteAnim->GetSpriteGroup();
        if (spriteGroup == nullptr) continue;

        spriteAnim->Update(state, delta);

        sprite.texture = spriteGroup->GetTexture();
        sprite.srcRect = *(spriteGroup->GetSourceRect(state.frameIdx));
    }
}

void RenderQuerySystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto cameraView = m_registry.view<Camera, Transform>();
    for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
    {
        if (camera.isActive == false) continue;

        auto &queryGizmos = m_scene->GetQueryGizmos();
        for (auto &gizmos : queryGizmos)
        {
            SDL_SetRenderDrawColor(
                g_renderer,
                gizmos->m_color.r,
                gizmos->m_color.g,
                gizmos->m_color.b,
                gizmos->m_color.a
            );
            gizmos->m_shape.Render(g_renderer, camera, cameraTransform);
        }
    }
}

bool RenderPhysicsSystem::DrawQueryCallback(b2ShapeId shapeId, void *context)
{
    CallbackContext *ctx = static_cast<CallbackContext *>(context);
    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    b2Transform xf = b2Body_GetTransform(bodyId);

    GizmosShape gizmos(shapeId, xf);
    gizmos.Render(g_renderer, ctx->camera, ctx->cameraTransform);

    const b2Vec2 position = b2Body_GetPosition(bodyId);
    const float x = ctx->camera.WorldToViewX(ctx->cameraTransform.position.x, position.x);
    const float y = ctx->camera.WorldToViewY(ctx->cameraTransform.position.y, position.y);
    const float w = 3.f;
    SDL_RenderLine(g_renderer, x - w, y - w, x + w, y + w);
    SDL_RenderLine(g_renderer, x + w, y - w, x - w, y + w);

    // On continue la recherche
    return true;
}

void RenderPhysicsSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto cameraView = m_registry.view<Camera, Transform>();
    for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
    {
        if (camera.isActive == false) continue;

        SDL_SetRenderDrawColor(g_renderer, 128, 255, 255, 255);

        b2AABB worldView = camera.GetWorldView(cameraTransform.position);

        CallbackContext context{ m_registry, camera, cameraTransform };
        b2QueryFilter filter = b2DefaultQueryFilter();
        filter.categoryBits = (uint64_t)(-1);
        filter.maskBits = (uint64_t)(-1);
        b2World_OverlapAABB(m_scene->GetWorld(), worldView, filter, DrawQueryCallback, &context);
    }
}

void RenderGridSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto cameraView = m_registry.view<Camera, Transform>();
    for (auto [cameraEntity, camera, cameraTransform] : cameraView.each())
    {
        if (camera.isActive == false) continue;

        b2AABB worldView = camera.GetWorldView(cameraTransform.position);
        for (int i = (int)worldView.lowerBound.y; i < worldView.upperBound.y; i++)
        {
            if (i == 0)
                SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
            else if ((i % 10) == 0)
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 63);

            float x, y;
            camera.WorldToView(cameraTransform.position, b2Vec2(0.f, (float)i), x, y);
            SDL_RenderLine(g_renderer, 0.f, y, camera.viewport.w, y);
        }
        for (int i = (int)worldView.lowerBound.x; i < worldView.upperBound.x; i++)
        {
            if (i == 0)
                SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
            else if ((i % 10) == 0)
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 63);

            float x, y;
            camera.WorldToView(cameraTransform.position, b2Vec2((float)i, 0.f), x, y);
            SDL_RenderLine(g_renderer, x, 0.f, x, camera.viewport.h);
        }
    }
}

// affiche l’interface utilisateur
void RenderUISystem::OnUpdate(EntityCommandBuffer &ecb)
{
    // Dessine les objets visibles par la caméra
    // Message : Render()
    UIObjectManager *uiManager = m_scene->GetUIObjectManager();
    uiManager->ProcessVisibleObjects();
    for (auto it = uiManager->visibleObjectsBegin();
         it != uiManager->visibleObjectsEnd(); ++it)
    {
        UIObject *object = *it;
        object->Render();
    }

    // Dessine les gizmos
    // Message : DrawGizmos()
    //if (false)
    {
        for (auto it = uiManager->begin();
             it != uiManager->end(); ++it)
        {
            UIObject *object = *it;
            if (object->GetGizmosEnabled())
            {
                object->DrawGizmos();
            }
        }
    }
}

void RenderImGuiSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    m_manager->Update();

    if (m_manager->ShouldBeRendered())
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        m_manager->Render();
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), g_renderer);
    }
}

//void HierarchySystem::OnFixedUpdate(EntityCommandBuffer &ecb)
//{
//    auto view = m_registry.view<ParentComponent>();
//    for (auto [entity, parentComp] : view.each())
//    {
//        entt::entity parent = parentComp.parent;
//
//        if (m_registry.valid(parent) && m_registry.all_of<ChildrenComponent>(parent))
//        {
//            ChildrenComponent &childrenComp = m_registry.get<ChildrenComponent>(parent);
//            childrenComp.children.emplace(entity);
//        }
//        else
//        {
//            uint32_t id = ecb.RegisterEntity(entity);
//            ecb.RemoveComponent<ParentComponent>(id);
//        }
//    }
//}
//
//void HierarchySystem::OnUpdate(EntityCommandBuffer &ecb)
//{
//    OnFixedUpdate(ecb);
//}
