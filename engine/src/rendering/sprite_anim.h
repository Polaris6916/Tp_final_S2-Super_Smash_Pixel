/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "rendering/sprite_sheet.h"
#include "rendering/anim.h"

struct SpriteAnimEvent;
struct SpriteAnimState;
class SpriteAnim;
class SpriteAnimManager;

using AnimID = uint32_t;

struct SpriteAnimEvent
{
    enum class Type
    {
        UNDEFINED, ANIMATION_START, ANIMATION_END, CYCLE_END, FRAME_CHANGED
    };

    SpriteAnimEvent(Type type = Type::UNDEFINED, int index = 0, AnimID id = 0);

    Type type;
    int index;
    AnimID id;
};

class SpriteAnim
{
public:
    SpriteAnim(AnimID id, SpriteGroup *spriteGroup);
    virtual ~SpriteAnim();

    virtual void Update(SpriteAnimState &state, float dt) const;

    void SetCycleCount(int cycleCount);
    int GetCycleCount() const;
    void SetCycleTime(float cycleTime);
    float GetCycleTime() const;
    float GetTotalTime() const;
    void SetFPS(float fps);
    SpriteGroup *GetSpriteGroup();
    int GetSpriteCount() const;

    static void EmplaceBackEvent(SpriteAnimState &state, SpriteAnimEvent::Type type, int index, AnimID id);

protected:
    float GetProgression(const SpriteAnimState &state) const;
    AnimID m_id;

    SpriteGroup *m_spriteGroup;

    /// @brief Courbe d'accélération de l'animation.
    /// @brief La durée d'un cycle.
    float m_cycleTime;

    /// @brief Nombre de cycles de l'animation.
    /// L'animation tourne en boucle si ce membre vaut -1.
    int m_cycleCount;

    std::vector<float> m_keyFrames;
};

struct SpriteAnimState
{
    SpriteAnimState(AnimID id, bool saveEvents = false);

    AnimID id;

    /// @brief Le coefficient multiplicateur de la vitesse de lecture.
    float speed;

    /// @brief Les flags de l'animation.
    /// Les valeurs possibles des flags sont données dans l'énumération AnimFlags.
    AnimFlag flags;

    /// @brief Accumulateur.
    float accu;

    /// @brief Indice du cycle courant. 
    int cycleIdx;

    int frameIdx;

    bool saveEvents;

    int eventCount;
    std::array<SpriteAnimEvent, MAX_ANIM_EVENT_COUNT> events;
};

class SpriteAnimUtils
{
public:
    static void AddFlags(SpriteAnimState &state, AnimFlag flags);
    static void SubFlags(SpriteAnimState &state, AnimFlag flags);
    static bool IsStopped(const SpriteAnimState &state);
    static bool IsDelayed(const SpriteAnimState &state);
    static bool IsPlaying(const SpriteAnimState &state);

    static void Play(SpriteAnimState &state, float delay = 0.f);
    static void Resume(SpriteAnimState &state);
    static void Stop(SpriteAnimState &state);
    static void SetAnimation(SpriteAnimState &state, AnimID id);

private:
    SpriteAnimUtils() = delete;
};

using AnimIDToStringFct = std::string (*)(AnimID);

class SpriteAnimManager
{
public:
    SpriteAnimManager(AnimIDToStringFct idToStr = nullptr);
    ~SpriteAnimManager() = default;

    void AddSpriteAnim(AnimID id, SpriteGroup *spriteGroup);
    SpriteAnim *GetSpriteAnim(AnimID id);
    SpriteGroup *GetSpriteGroup(AnimID id);
    SpriteAnim *GetSpriteAnim(SpriteAnimState &state);
    SpriteGroup *GetSpriteGroup(SpriteAnimState &state);

    void SetAnimIDToString(AnimIDToStringFct idToStr);

    std::string GetAnimName(AnimID id) const;

    void Update(SpriteAnimState &state, float dt) const;

private:
    std::map<AnimID, SpriteAnim> m_animMap;
    AnimIDToStringFct m_idToStr;
};


inline void SpriteAnimUtils::AddFlags(SpriteAnimState &state, AnimFlag flagsIn)
{
    state.flags = state.flags | flagsIn;
}

inline void SpriteAnimUtils::SubFlags(SpriteAnimState &state, AnimFlag flagsIn)
{
    state.flags = state.flags & ~flagsIn;
}

inline bool SpriteAnimUtils::IsStopped(const SpriteAnimState &state)
{
    return static_cast<uint32_t>(state.flags & AnimFlag::STOPPED) != 0;
}

inline bool SpriteAnimUtils::IsDelayed(const SpriteAnimState &state)
{
    const float epsilon = 0.001f;
    return state.accu < -epsilon;
}

inline bool SpriteAnimUtils::IsPlaying(const SpriteAnimState &state)
{
    return static_cast<uint32_t>(state.flags & (AnimFlag::STOPPED | AnimFlag::PAUSED)) == 0;
}

inline int SpriteAnim::GetCycleCount() const
{
    return m_cycleCount;
}

inline float SpriteAnim::GetCycleTime() const
{
    return m_cycleTime;
}

inline float SpriteAnim::GetTotalTime() const
{
    return m_cycleCount >= 0 ? m_cycleCount * m_cycleTime : m_cycleTime;
}

inline void SpriteAnim::SetCycleTime(float cycleTime)
{
    m_cycleTime = cycleTime;
}

inline int SpriteAnim::GetSpriteCount() const
{
    return m_spriteGroup->GetSpriteCount();
}

inline SpriteGroup *SpriteAnim::GetSpriteGroup()
{
    return m_spriteGroup;
}

inline void SpriteAnim::SetFPS(float fps)
{
    m_cycleTime = GetSpriteCount() / fps;
}

inline float SpriteAnim::GetProgression(const SpriteAnimState &state) const
{
    return state.accu / m_cycleTime;
}

inline SpriteAnim *SpriteAnimManager::GetSpriteAnim(SpriteAnimState &state)
{
    return GetSpriteAnim(state.id);
}

inline SpriteGroup *SpriteAnimManager::GetSpriteGroup(AnimID id)
{
    SpriteAnim *spriteAnim = GetSpriteAnim(id);
    if (spriteAnim) return spriteAnim->GetSpriteGroup();
    return nullptr;
}

inline SpriteGroup *SpriteAnimManager::GetSpriteGroup(SpriteAnimState &state)
{
    SpriteAnim *spriteAnim = GetSpriteAnim(state.id);
    if (spriteAnim) return spriteAnim->GetSpriteGroup();
    return nullptr;
}
