/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "renderer.h"
#include "cJSON.h"

class SpriteSheet;

class SpriteGroup
{
public:
    ~SpriteGroup();

    bool RenderTexture(int idx, const SDL_FRect *dstRect, Anchor anchor);
    bool RenderTextureRotated(
        int idx, const SDL_FRect *dstRect, Anchor anchor,
        const double angle, SDL_FlipMode flip
    );

    int GetSpriteCount() const;
    SDL_Texture *GetTexture();
    const SDL_FRect *GetSourceRect(int spriteIdx);
    const std::string &GetName() const;

protected:
    friend class SpriteSheet;

    SpriteGroup(SpriteSheet &spriteSheet);

    SpriteSheet &m_spriteSheet;
    std::string m_name;
    int *m_spriteIndices;
    int m_spriteCount;

private:
};

class SpriteSheet
{
public:

    SpriteSheet(SDL_Renderer *renderer, const std::string &path);
    SpriteSheet(SpriteSheet const&) = delete;
    SpriteSheet& operator=(SpriteSheet const&) = delete;
    ~SpriteSheet();

    SDL_Texture *GetTexture();

    SpriteGroup *GetGroup(const std::string &name);
    SpriteGroup *GetGroup(int i);
    int GetGroupCount() const;

    int GetSourceRectCount() const;
    const SDL_FRect *GetSourceRect(int index) const;

protected:

    friend class SpriteGroup;

    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;

    SpriteGroup **m_groups;
    int m_groupCount;
    
    // TODO
    //std::map<std::string, SpriteGroup *> m_groupMap;

    SDL_FRect *m_rects;
    int m_rectCount;

private:

    void LoadRect(cJSON *jRect, int i);
    void LoadPart(cJSON *jPart, int i);
    void LoadGeometry(cJSON *jGeo);
    char *ParseJSON(cJSON *json);
};

inline bool SpriteGroup::RenderTexture(
    int idx, const SDL_FRect *dstRect, Anchor anchor)
{
    const SDL_FRect *srcRect = GetSourceRect(idx);
    return ::RenderTexture(
        m_spriteSheet.m_renderer, m_spriteSheet.m_texture, srcRect, dstRect, anchor
    );
}

inline bool SpriteGroup::RenderTextureRotated(
    int idx, const SDL_FRect *dstRect, Anchor anchor,
    const double angle, SDL_FlipMode flip)
{
    const SDL_FRect *srcRect = GetSourceRect(idx);
    return ::RenderTextureRotated(
        m_spriteSheet.m_renderer, m_spriteSheet.m_texture, srcRect, dstRect, anchor,
        angle, flip
    );
}

inline int SpriteGroup::GetSpriteCount() const
{
    return m_spriteCount;
}

inline SDL_Texture *SpriteGroup::GetTexture()
{
    return m_spriteSheet.GetTexture();
}

inline const SDL_FRect *SpriteGroup::GetSourceRect(int i)
{
    int rectIndex = m_spriteIndices[i % m_spriteCount];
    return &(m_spriteSheet.m_rects[rectIndex]);
}

inline SDL_Texture *SpriteSheet::GetTexture()
{
    return m_texture;
}

inline int SpriteSheet::GetSourceRectCount() const
{
    return m_rectCount;
}

inline const SDL_FRect * SpriteSheet::GetSourceRect(int index) const
{
    assert(0 <= index && index < m_rectCount);
    return &(m_rects[index]);
}
