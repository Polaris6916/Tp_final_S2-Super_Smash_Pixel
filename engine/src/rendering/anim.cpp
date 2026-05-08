/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "rendering/anim.h"

AnimState::AnimState(float cycleTimeIn, int cycleCountIn, bool saveEventsIn)
    : cycleTime(cycleTimeIn)
    , cycleCount(cycleCountIn)
    , cycleIdx(0)
    , saveEvents(saveEventsIn)
    , speed(1.f)
    , accu(0.f)
    , eventCount(0)
    , events()
    , flags(AnimFlag::NONE)
{
}

float AnimUtils::GetProgression(const AnimState &state)
{
    float t = state.accu / state.cycleTime;
    if (t > 1.f) t -= floorf(t);

    // Animation alternée
    if (uint32_t(state.flags & AnimFlag::ALTERNATE))
    {
        t = (t < 0.5f) ? 2.f * t : 2.f - 2.f * t;
    }

    // Animation inversée
    if (uint32_t(state.flags & AnimFlag::REVERSED))
    {
        t = 1.f - t;
    }

    return t;
}

void AnimUtils::Play(AnimState &state, float delay)
{
    state.accu = -delay;
    state.cycleIdx = 0;
    SubFlags(state, AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void AnimUtils::Resume(AnimState &state)
{
    SubFlags(state, AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void AnimUtils::Stop(AnimState &state)
{
    AddFlags(state, AnimFlag::STOPPED);
}

void AnimUtils::Update(AnimState &state, float dt)
{
    state.eventCount = 0;

    if (IsPlaying(state) == false) return;

    const float epsilon = 0.001f;
    const float prevAccu = state.accu;

    state.accu += state.speed * dt;

    if (prevAccu < 0.f && state.accu >= 0.f)
    {
        EmplaceBackEvent(state, AnimEvent::Type::ANIMATION_START, 0);
    }

    while (state.accu >= state.cycleTime - epsilon)
    {
        EmplaceBackEvent(state, AnimEvent::Type::CYCLE_END, state.cycleIdx);

        state.accu -= state.cycleTime;
        state.cycleIdx++;

        if ((state.cycleCount > 0) && (state.cycleIdx >= state.cycleCount))
        {
            // Fin de l'animation
            EmplaceBackEvent(state, AnimEvent::Type::ANIMATION_END, state.cycleIdx);

            state.accu = state.cycleTime;

            AddFlags(state, AnimFlag::PAUSED);

            if (uint32_t(state.flags & AnimFlag::STOP_AT_END))
            {
                AddFlags(state, AnimFlag::STOPPED);
            }
            return;
        }
    }
}

void AnimUtils::EmplaceBackEvent(AnimState &state, AnimEvent::Type type, int index)
{
    if (state.saveEvents == false || state.eventCount >= MAX_ANIM_EVENT_COUNT) return;
    state.events[state.eventCount++] = AnimEvent(type, index);
}
