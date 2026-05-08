/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

namespace random
{
    inline int RangeI(int min, int max)
    {
        return min + (rand() % (max - min + 1));
    }

    inline float RangeF(float min, float max)
    {
        return min + (max - min) * (float)rand() / (float)RAND_MAX;
    }
};

#define RAD_TO_DEG 57.2957795130823f
#define DEG_TO_RAD 0.01745329251994329576923690768489f
#define TAU 6.283185307179586476925286766559

namespace math
{
    inline float Dot(const b2Vec2 &a, const b2Vec2 &b)
    {
        return a.x * b.x + a.y * b.y;
    }

    inline float Det(const b2Vec2 &a, const b2Vec2 &b)
    {
        return a.x * b.y - a.y * b.x;
    }

    inline float Distance(const b2Vec2 &a, const b2Vec2 &b)
    {
        return b2Distance(a, b);
    }

    inline float DistanceSquared(const b2Vec2 &a, const b2Vec2 &b)
    {
        b2Vec2 c = a - b;
        return Dot(c, c);
    }

    template <typename T>
    inline T Min(T a, T b)
    {
        return a < b ? a : b;
    }

    inline b2Vec2 Min(const b2Vec2 &a, const b2Vec2 &b)
    {
        return b2Vec2(Min(a.x, b.x), Min(a.y, b.y));
    }

    template <typename T>
    inline T Max(T a, T b)
    {
        return a > b ? a : b;
    }

    inline b2Vec2 Max(const b2Vec2 &a, const b2Vec2 &b)
    {
        return b2Vec2(Max(a.x, b.x), Max(a.y, b.y));
    }

    template <typename T>
    inline T Clamp(T value, T a, T b)
    {
        return Max(a, Min(value, b));
    }

    float SignedAngleDeg(b2Vec2 from, b2Vec2 to);
    float SignedAngleRad(b2Vec2 from, b2Vec2 to);
    float AngleDeg(b2Vec2 from, b2Vec2 to);
    float AngleRad(b2Vec2 from, b2Vec2 to);

    inline b2Vec2 Perp(b2Vec2 v)
    {
        return { -v.y, v.x };
    }

    inline b2Vec2 PerpCW(b2Vec2 v)
    {
        return { v.y, -v.x };
    }

    inline float DegToRad(float angleDeg)
    {
        return angleDeg * B2_PI / 180.f;
    }

    inline float RadToDeg(float angleRad)
    {
        return angleRad * 180.f / B2_PI;
    }

    inline b2Vec2 UnitVectorRad(float angle)
    {
        return b2Vec2(cosf(angle), sinf(angle));
    }

    inline b2Vec2 UnitVectorDeg(float angle)
    {
        return UnitVectorRad(angle / RAD_TO_DEG);
    }

    template <typename T>
    inline T Lerp(T from, T to, float t)
    {
        t = Clamp(t, 0.f, 1.f);
        return (1.f - t) * from + t * to;
    }

    template <typename T>
    inline T LerpUnclamped(T from, T to, float t)
    {
        return (1.f - t) * from + t * to;
    }

    inline float AngleDifference(float from, float to)
    {
        float difference = fmod(to - from, (float)TAU);
        return fmod(2.0f * difference, (float)TAU) - difference;
    }

    inline float LerpAngle(float from, float to, float t) {
        return from + AngleDifference(from, to) * t;
    }

    bool PointInAABB(const b2Vec2 &point, const b2AABB &aabb);

    float SmoothDamp(
        float current, float target, float &currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);
    b2Vec2 SmoothDamp(
        b2Vec2 current, b2Vec2 target, b2Vec2 &currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);

    inline float Smooth(float value, float target, float k, float dt)
    {
        k = 1.f - powf(k, dt);
        return Lerp(value, target, k);
    }
}

b2Vec2 SDL_FRectDistance(const SDL_FRect &a, const SDL_FRect &b);
