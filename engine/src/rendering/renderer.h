/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

enum class Anchor : uint32_t
{
    NORTH_WEST, NORTH_EAST, NORTH,
    EAST, CENTER, WEST,
    SOUTH_WEST, SOUTH, SOUTH_EAST
};

bool RenderTexture(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor
);

bool RenderTextureRotated(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    const double angle, SDL_FlipMode flip
);

bool RenderTexture9Grid(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    float l, float r, float t, float b, float scale
);
