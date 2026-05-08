/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "rendering/easing_fct.h"

constexpr uint32_t MAX_ANIM_EVENT_COUNT = 8;

enum class AnimFlag : uint32_t
{
    NONE = 0,
    /// @brief Met l'animation en pause.
    PAUSED = 1 << 0,
    /// @brief Joue l'animation dans le sens inverse.
    REVERSED = 1 << 1,
    /// @brief Joue l'animation en avant puis en arrière.
    ALTERNATE = 1 << 2,
    /// @brief Arrête automatiquement l'animation à la fin.
    STOP_AT_END = 1 << 3,
    STOPPED = 1 << 4
};

struct AnimEvent
{
    enum class Type
    {
        UNDEFINED, ANIMATION_START, ANIMATION_END, CYCLE_END
    };

    AnimEvent()
        : type(Type::UNDEFINED), index(0)
    {}
    AnimEvent(Type type, int index)
        : type(type), index(index)
    {}

    Type type;
    int index;
};

struct AnimState
{
    AnimState(float cycleTime, int cycleCount, bool saveEvents = false);

    /// @brief Le coefficient multiplicateur de la vitesse de lecture.
    float speed;

    /// @brief Les flags de l'animation.
    AnimFlag flags;

    /// @brief Accumulateur.
    float accu;

    /// @brief Indice du cycle courant. 
    int cycleIdx;

    float cycleTime;

    /// @brief Nombre de cycles de l'animation.
    /// L'animation tourne en boucle si ce membre vaut -1.
    int cycleCount;


    bool saveEvents;
    int eventCount;
    std::array<AnimEvent, MAX_ANIM_EVENT_COUNT> events;
};

template <typename T>
struct LerpValues
{
    T value0;
    T value1;
    EasingFct easing;
};

class AnimUtils
{
public:
    static float GetProgression(const AnimState &state);

    template <typename T>
    static T GetValue(const AnimState &state, const LerpValues<T> &lerpAnim);

    static void AddFlags(AnimState &state, AnimFlag flags);
    static void SubFlags(AnimState &state, AnimFlag flags);
    static bool IsStopped(const AnimState &state);
    static bool IsDelayed(const AnimState &state);
    static bool IsPlaying(const AnimState &state);

    static void Play(AnimState &state, float delay = 0.f);
    static void Resume(AnimState &state);
    static void Stop(AnimState &state);
    static void Update(AnimState &state, float dt);

private:
    AnimUtils() = delete;

    static void EmplaceBackEvent(AnimState &state, AnimEvent::Type type, int index);
};

constexpr enum AnimFlag operator |(const enum AnimFlag selfValue, const enum AnimFlag inValue)
{
    return static_cast<enum AnimFlag>(
        static_cast<uint32_t>(selfValue) | static_cast<uint32_t>(inValue));
}

constexpr enum AnimFlag operator &(const enum AnimFlag selfValue, const enum AnimFlag inValue)
{
    return static_cast<enum AnimFlag>(
        static_cast<uint32_t>(selfValue) & static_cast<uint32_t>(inValue));
}

constexpr enum AnimFlag operator ~(const enum AnimFlag selfValue)
{
    return static_cast<enum AnimFlag>(
        ~static_cast<uint32_t>(selfValue));
}

template<typename T>
inline T AnimUtils::GetValue(const AnimState &state, const LerpValues<T> &lerpAnim)
{
    float p = GetProgression(state);
    if (lerpAnim.easing) p = lerpAnim.easing(p);
    p = fmaxf(0.f, fminf(p, 1.f));
    return (1.f - p) * lerpAnim.value0 + p * lerpAnim.value1;
}

inline void AnimUtils::AddFlags(AnimState &state, AnimFlag flagsIn)
{
    state.flags = state.flags | flagsIn;
}

inline void AnimUtils::SubFlags(AnimState &state, AnimFlag flagsIn)
{
    state.flags = state.flags & ~flagsIn;
}

inline bool AnimUtils::IsStopped(const AnimState &state)
{
    return static_cast<uint32_t>(state.flags & AnimFlag::STOPPED) != 0;
}

inline bool AnimUtils::IsDelayed(const AnimState &state)
{
    const float epsilon = 0.001f;
    return state.accu < -epsilon;
}

inline bool AnimUtils::IsPlaying(const AnimState &state)
{
    return static_cast<uint32_t>(state.flags & (AnimFlag::STOPPED | AnimFlag::PAUSED)) == 0;
}
