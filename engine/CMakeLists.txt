
set(NAME engine)
add_library(${NAME} STATIC)

file(GLOB_RECURSE
    PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.c"
)
file(GLOB_RECURSE
    PROJECT_HEADER_FILES CONFIGURE_DEPENDS
    "src/*.hpp" "src/*.h"
)

target_compile_features(${NAME} PUBLIC cxx_std_20)
target_compile_definitions(${NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${NAME} PUBLIC -Wall)
endif()

target_sources(${NAME} PRIVATE
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake_targets")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "sources"
    FILES ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES}
)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${NAME} PUBLIC ${MATH_LIBRARY})
endif()

#-------------------------------------------------------------------------------
# Third party libraries

target_link_libraries(${NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    box2d::box2d
)

#-------------------------------------------------------------------------------
# Other include directories

target_include_directories(
    ${NAME} PUBLIC
    "src"
    "${THIRD_PARTY_IM_GUI_DIR}"
    "${THIRD_PARTY_IM_GUI_DIR}/backends"
    "${THIRD_PARTY_CJSON_DIR}"
    "${THIRD_PARTY_ENTT_DIR}/src"
    "${THIRD_PARTY_MAGIC_ENUM_DIR}/include"
)

#-------------------------------------------------------------------------------
# ImGui files

file(GLOB
    IM_GUI_SOURCE_FILES CONFIGURE_DEPENDS
    "${THIRD_PARTY_IM_GUI_DIR}/*.h"
    "${THIRD_PARTY_IM_GUI_DIR}/*.cpp"
)

set(IM_GUI_BACKENDS_FILES
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdl3.h"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdl3.cpp"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdlrenderer3.h"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)

target_sources("${NAME}" PRIVATE
    ${IM_GUI_SOURCE_FILES} ${IM_GUI_BACKENDS_FILES}
)

source_group("third_party/im_gui" FILES ${IM_GUI_SOURCE_FILES})
source_group("third_party/im_gui/backends" FILES ${IM_GUI_BACKENDS_FILES})

#-------------------------------------------------------------------------------
# cJSON files

file(GLOB
    CJSON_SOURCE_FILES CONFIGURE_DEPENDS
    "${THIRD_PARTY_CJSON_DIR}/*.h"
    "${THIRD_PARTY_CJSON_DIR}/*.c"
)

target_sources("${NAME}" PRIVATE
    ${CJSON_SOURCE_FILES}
)

source_group("third_party/cjson" FILES ${CJSON_SOURCE_FILES})
