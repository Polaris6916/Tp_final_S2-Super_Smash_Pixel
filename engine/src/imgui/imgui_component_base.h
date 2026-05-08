/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"

class ImGuiManagerBase;

class ImGuiBaseComponent
{
public:
    ImGuiBaseComponent(std::string componentName)
        : name(componentName)
        , manager(nullptr)
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) { return false; }
    virtual void DrawImGui(entt::registry &registry, entt::entity entity) {}

    std::string name;
    ImGuiManagerBase *manager;
};

class ImGuiBaseTag
{
public:
    ImGuiBaseTag(const std::string &typeName)
        : name(typeName)
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) { return false; }

    std::string name;
};

template<typename T>
class ImGuiTag : public ImGuiBaseTag
{
public:
    ImGuiTag(const std::string &typeName)
        : ImGuiBaseTag(typeName)
    {}

    virtual bool Has(entt::registry &registry, entt::entity entity) override
    {
        return registry.all_of<T>(entity);
    }
};

#define ENUM_STRING(value) std::string(magic_enum::enum_name(value))

class Scene;
class UIObject;
class ImGuiUtils
{
public:
    static void Member(const char *name, entt::entity entity, entt::registry &registry, ImGuiManagerBase *manager);
    static void Member(const char *name, int value);
    static void Member(const char *name, size_t value);
    static void Member(const char *name, float value);
    static void Member(const char *name, bool value);
    static void Member(const char *name, b2Vec2 value);
    static void Member(const char *name, const std::string &value);
    static void Member(const char *name, const char *value);
    static void MemberBits(const char *name, uint64_t value);
    static void Member(const char *name, UIObject *value, ImGuiManagerBase *manager);
    static bool BeginSection(const char *name);
    static void EndSection();

private:
    ImGuiUtils() = delete;
};

