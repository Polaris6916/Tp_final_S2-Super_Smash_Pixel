/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "rendering/renderer.h"

SDL_FRect FRect_Set(const SDL_FRect *rect, Anchor anchor)
{
    SDL_FRect res = *rect;

    switch (anchor)
    {
    case Anchor::NORTH_WEST:
        break;
    case Anchor::WEST:
        res.y -= res.h / 2.f;
        break;
    case Anchor::SOUTH_WEST:
        res.y -= res.h;
        break;
    case Anchor::NORTH:
        res.x -= res.w / 2.f;
        break;
    case Anchor::CENTER:
        res.x -= res.w / 2.f;
        res.y -= res.h / 2.f;
        break;
    case Anchor::SOUTH:
        res.x -= res.w / 2.f;
        res.y -= res.h;
        break;
    case Anchor::NORTH_EAST:
        res.x -= res.w;
        break;
    case Anchor::EAST:
        res.x -= res.w;
        res.y -= res.h / 2.f;
        break;
    case Anchor::SOUTH_EAST:
        res.x -= res.w;
        res.y -= res.h;
        break;
    default:
        break;
    }

    return res;
}

bool RenderTexture(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor)
{
    SDL_FRect dst = FRect_Set(dstRect, anchor);
    return SDL_RenderTexture(renderer, texture, srcRect, &dst);
}

bool RenderTextureRotated(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    const double angle, SDL_FlipMode flip)
{
    SDL_FRect dst = FRect_Set(dstRect, anchor);

    SDL_FPoint sdlCenter{ 0.f, 0.f };
    switch (anchor)
    {
    case Anchor::NORTH_WEST:
        sdlCenter.x = 0.f;
        sdlCenter.y = 0.f;
        break;
    case Anchor::WEST:
        sdlCenter.x = 0.f;
        sdlCenter.y = dstRect->h / 2.f;
        break;
    case Anchor::SOUTH_WEST:
        sdlCenter.x = 0.f;
        sdlCenter.y = dstRect->h;
        break;
    case Anchor::NORTH:
        sdlCenter.x = dstRect->w / 2.f;
        sdlCenter.y = 0.f;
        break;
    case Anchor::CENTER:
        sdlCenter.x = dstRect->w / 2.f;
        sdlCenter.y = dstRect->h / 2.f;
        break;
    case Anchor::SOUTH:
        sdlCenter.x = dstRect->w / 2.f;
        sdlCenter.y = dstRect->h;
        break;
    case Anchor::NORTH_EAST:
        sdlCenter.x = dstRect->w;
        sdlCenter.y = 0.f;
        break;
    case Anchor::EAST:
        sdlCenter.x = dstRect->w;
        sdlCenter.y = dstRect->h / 2.f;
        break;
    case Anchor::SOUTH_EAST:
        sdlCenter.x = dstRect->w;
        sdlCenter.y = dstRect->h;
        break;
    default:
        break;
    }
    return SDL_RenderTextureRotated(renderer, texture, srcRect, &dst, angle, &sdlCenter, flip);
}

bool RenderTexture9Grid(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    float l, float r, float t, float b, float scale)
{
    SDL_FRect dstAnchored = FRect_Set(dstRect, anchor);
    return SDL_RenderTexture9Grid(renderer, texture, srcRect, l, r, t, b, scale, &dstAnchored);
}
