/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "rendering/easing_fct.h"

float EasingFct_Linear(float x)
{
    return x;
}

float EasingFct_In(float x)
{
    return x * x;
}

float EasingFct_Out(float x)
{
    float t = 1 - x;
    return 1.f - t * t;
}

float EasingFct_InOut(float x)
{
    if (x < 0.5f)
    {
        return 2.f * x * x;
    }
    else
    {
        float t = 1 - x;
        return 1.f - 2.f * t * t;
    }
}

float EasingFct_Cos(float x)
{
    return 0.5f * (1.f - cosf(B2_PI * x));
}

float EasingFct_Shake2(float t)
{
    const float count = 2.5f;
    float coeff = 1.f - t * t;
    return cosf(count * 2.f * B2_PI * t - 0.5f * B2_PI) * coeff * coeff;
}

float EasingFct_Shake3(float t)
{
    const float count = 3.5f;
    float coeff = 1.f - t * t;
    return cosf(count * 2.f * B2_PI * t - 0.5f * B2_PI) * coeff * coeff;
}

float EasingFct_Shake4(float t)
{
    const float count = 4.5f;
    float coeff = 1.f - t * t;
    return cosf(count * 2.f * B2_PI * t - 0.5f * B2_PI) * coeff * coeff;
}