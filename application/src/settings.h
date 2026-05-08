/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine.h"

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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#ifdef _MSC_VER
#  define INLINE inline
#else
#  define INLINE static inline
#endif

#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }

#define HD_WIDTH 1280
#define HD_HEIGHT 720
#define FHD_WIDTH 1920
#define FHD_HEIGHT 1080
