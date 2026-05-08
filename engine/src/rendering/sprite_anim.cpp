/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "rendering/sprite_anim.h"

SpriteAnimEvent::SpriteAnimEvent(Type type, int index, AnimID id)
    : type(type)
    , index(index)
    , id(id)
{}

SpriteAnimState::SpriteAnimState(AnimID id, bool saveEvents)
    : id(id)
    , speed(1.f)
    , flags(AnimFlag::NONE)
    , events{}
    , eventCount(0)
    , saveEvents(saveEvents)
    , accu(0.f)
    , cycleIdx(0)
    , frameIdx(0)
{
}

SpriteAnim::SpriteAnim(AnimID id, SpriteGroup *spriteGroup)
    : m_id(id)
    , m_cycleCount(-1)
    , m_cycleTime(1.f)
    , m_spriteGroup(spriteGroup)
    , m_keyFrames()
{
    assert(spriteGroup);
    int frameCount = m_spriteGroup->GetSpriteCount();
    m_keyFrames.reserve(frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        m_keyFrames.push_back((float)i / (float)frameCount);
    }
}

SpriteAnim::~SpriteAnim()
{
}

void SpriteAnim::SetCycleCount(int cycleCount)
{
    m_cycleCount = cycleCount;
}

void SpriteAnim::Update(SpriteAnimState &state, float dt) const
{
    state.eventCount = 0;

    if (SpriteAnimUtils::IsPlaying(state) == false) return;

    const float epsilon = 0.001f;
    const float prevAccu = state.accu;
    float prevProgression = GetProgression(state);

    state.accu += state.speed * dt;

    if (prevAccu < 0.f && state.accu >= 0.f)
    {
        EmplaceBackEvent(state, SpriteAnimEvent::Type::ANIMATION_START, 0, m_id);
    }

    while (state.accu >= m_cycleTime - epsilon)
    {
        for (int i = 0; i < m_keyFrames.size(); i++)
        {
            if (m_keyFrames[i] > prevProgression - epsilon)
            {
                EmplaceBackEvent(state, SpriteAnimEvent::Type::FRAME_CHANGED, i, m_id);
            }
        }
        prevProgression = 0.f;

        EmplaceBackEvent(state, SpriteAnimEvent::Type::CYCLE_END, state.cycleIdx, m_id);

        state.accu -= m_cycleTime;
        state.cycleIdx++;

        if ((m_cycleCount > 0) && (state.cycleIdx >= m_cycleCount))
        {
            // Fin de l'animation
            EmplaceBackEvent(state, SpriteAnimEvent::Type::ANIMATION_END, 0, m_id);

            state.accu = m_cycleTime;
            SpriteAnimUtils::AddFlags(state, AnimFlag::PAUSED);

            if (static_cast<uint32_t>(state.flags & AnimFlag::STOP_AT_END))
            {
                SpriteAnimUtils::AddFlags(state, AnimFlag::STOPPED);
            }
            return;
        }
    }

    const float currProgression = GetProgression(state);
    for (int i = 0; i < m_keyFrames.size(); i++)
    {
        if (m_keyFrames[i] > prevProgression - epsilon &&
            m_keyFrames[i] <= currProgression - epsilon)
        {
            EmplaceBackEvent(state, SpriteAnimEvent::Type::FRAME_CHANGED, i, m_id);
            state.frameIdx = i;
        }
    }
}


void SpriteAnimUtils::Play(SpriteAnimState &state, float delay)
{
    state.accu = -delay;
    state.cycleIdx = 0;
    SubFlags(state, AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void SpriteAnimUtils::Resume(SpriteAnimState &state)
{
    SubFlags(state, AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void SpriteAnimUtils::Stop(SpriteAnimState &state)
{
    AddFlags(state, AnimFlag::STOPPED);
}

void SpriteAnimUtils::SetAnimation(SpriteAnimState &state, AnimID id)
{
    state.id = id;
    state.speed = 1.f;
    state.flags = AnimFlag::NONE;
    state.accu = 0.f;
    state.cycleIdx = 0;
    state.frameIdx = 0;
}

void SpriteAnim::EmplaceBackEvent(
    SpriteAnimState &state, SpriteAnimEvent::Type type, int index, AnimID id)
{
    if (state.saveEvents == false || state.eventCount >= MAX_ANIM_EVENT_COUNT) return;
    state.events[state.eventCount++] = SpriteAnimEvent(type, index, id);
}


SpriteAnimManager::SpriteAnimManager(AnimIDToStringFct idToStr)
    : m_idToStr(idToStr)
{
}

void SpriteAnimManager::AddSpriteAnim(AnimID id, SpriteGroup *spriteGroup)
{
    auto it = m_animMap.find(id);
    if (it != m_animMap.end())
    {
        std::cout
            << "ERROR - Add sprite anim " << id << std::endl
            << "      - The sprite anim already exists" << std::endl;
        return;
    }
    m_animMap.emplace(id, SpriteAnim{ id, spriteGroup });
}

SpriteAnim *SpriteAnimManager::GetSpriteAnim(AnimID id)
{
    auto it = m_animMap.find(id);
    if (it != m_animMap.end())
    {
        return &(it->second);
    }
    std::cout << "ERROR - Get sprite anim " << id << std::endl;
    return nullptr;
}

void SpriteAnimManager::Update(SpriteAnimState &state, float dt) const
{
    auto it = m_animMap.find(state.id);
    if (it != m_animMap.end())
    {
        const SpriteAnim &spriteAnim = it->second;
        spriteAnim.Update(state, dt);
    }
}

std::string SpriteAnimManager::GetAnimName(AnimID id) const
{
    if (m_idToStr) return m_idToStr(id);
    return std::to_string(id);
}

void SpriteAnimManager::SetAnimIDToString(AnimIDToStringFct idToStr)
{
    m_idToStr = idToStr;
}
