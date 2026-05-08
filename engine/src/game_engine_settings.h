/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include <algorithm>

#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>

#include <iostream>
#include <iomanip>
#include <string>

#include <cassert>
#include <cmath>
//#include <cstdbool>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <magic_enum/magic_enum.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }
#define DEFAULT_UI_LAYER 1024
#define MAX_TIME_MS (Uint64)0xFFFFFFFFFFFFFFFF
