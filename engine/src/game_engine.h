/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"

#include "utils/timer.h"
#include "utils/utils.h"
#include "utils/color.h"
#include "utils/gizmos_shape.h"

#include "rendering/renderer.h"
#include "rendering/sprite_sheet.h"
#include "rendering/easing_fct.h"
#include "rendering/anim.h"
#include "rendering/sprite_anim.h"

#include "input/input_manager.h"
#include "input/input_group.h"
#include "input/ui_input.h"

#include "ecs/command_buffer.h"
#include "ecs/basic_components.h"
#include "ecs/basic_systems.h"

#include "scene/asset_manager.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "scene/particle_system.h"

#include "imgui/imgui_component_base.h"
#include "imgui/imgui_basic_components.h"
#include "imgui/imgui_manager_base.h"

#include "ui/ui_object_manager.h"
#include "ui/ui_object.h"
#include "ui/base/ui_canvas.h"
#include "ui/base/ui_selectable.h"
#include "ui/base/ui_selectable_group.h"
#include "ui/base/ui_grid_layout.h"
#include "ui/base/ui_base_list.h"
#include "ui/interaction/ui_button.h"
#include "ui/interaction/ui_item_list.h"
#include "ui/visual/ui_animator.h"
#include "ui/visual/ui_fill_rect.h"
#include "ui/visual/ui_image.h"
#include "ui/visual/ui_text.h"
