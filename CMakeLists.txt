cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(NAME super_pixel_smash_ecs)
project(${NAME} VERSION 1.0)

option(VS_DEBUG_RELEASE "Generate only DEBUG and RELEASE configuration on VS" ON)

message(STATUS "[INFO] Current directory: " ${CMAKE_SOURCE_DIR})

if(MSVC AND VS_DEBUG_RELEASE)
    message(STATUS "[INFO] Configurations: DEBUG; RELEASE")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "[INFO] Multi-configuration generator")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/_bin/")
else()
    message(STATUS "[INFO] Single-configuration generator")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/_bin/${CMAKE_BUILD_TYPE}")
endif()


#-------------------------------------------------------------------------------
# Third party libraries

if(MSVC)
    message(STATUS "MSCV DETECTED")
    set(THIRD_PARTY_SDL3_DIR       "${CMAKE_SOURCE_DIR}/third_party/SDL3")
    set(THIRD_PARTY_SDL3_IMAGE_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL3_image")
    set(THIRD_PARTY_SDL3_TTF_DIR   "${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf")
    set(THIRD_PARTY_SDL3_MIXER_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL3_mixer")

    set(THIRD_PARTY_BOX_2D_DIR     "${CMAKE_SOURCE_DIR}/third_party/box2d-3.1")
    set(THIRD_PARTY_IM_GUI_DIR     "${CMAKE_SOURCE_DIR}/third_party/imgui-1.91.9b")
    set(THIRD_PARTY_CJSON_DIR      "${CMAKE_SOURCE_DIR}/third_party/cJSON")
    set(THIRD_PARTY_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/third_party/magic_enum")
    set(THIRD_PARTY_ENTT_DIR       "${CMAKE_SOURCE_DIR}/third_party/entt-3.15.0")

    list(APPEND CMAKE_PREFIX_PATH
        "${THIRD_PARTY_SDL3_DIR}/cmake"
        "${THIRD_PARTY_SDL3_IMAGE_DIR}/cmake"
        "${THIRD_PARTY_SDL3_TTF_DIR}/cmake"
        "${THIRD_PARTY_SDL3_MIXER_DIR}/cmake"
        "${THIRD_PARTY_BOX_2D_DIR}/lib/cmake/box2d"
    )
endif()

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(box2d REQUIRED)

#-------------------------------------------------------------------------------
# Projects

add_subdirectory(engine)
add_subdirectory(application)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT application)
endif()
