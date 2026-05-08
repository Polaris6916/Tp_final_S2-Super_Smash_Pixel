/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"

class EntityCommandBuffer;
using VirtualEntityID = uint32_t;

struct EntityCommand
{
public:
    EntityCommand(const EntityCommandBuffer &ecb, VirtualEntityID virtualID) : m_ecb(ecb), m_virtualID(virtualID) {}
    virtual ~EntityCommand() = default;
    VirtualEntityID GetVirtualId() const { return m_virtualID; }

    virtual void Execute(entt::registry &registry, entt::entity entity) {};

protected:
    const EntityCommandBuffer &m_ecb;
    VirtualEntityID m_virtualID;
};

template<typename T>
struct AddComponentCommand : public EntityCommand
{
public:
    AddComponentCommand(EntityCommandBuffer &ecb, VirtualEntityID virtualID, T component)
        : EntityCommand(ecb, virtualID)
        , m_component(std::move(component))
    {}

    void Execute(entt::registry& registry, entt::entity entity) override
    {
        registry.emplace_or_replace<T>(entity, m_component);
    }

private:
    T m_component;
};

template<typename T>
struct RemoveComponentCommand : public EntityCommand
{
public:
    RemoveComponentCommand(EntityCommandBuffer &ecb, VirtualEntityID virtualID)
        : EntityCommand(ecb, virtualID)
    {}

    void Execute(entt::registry& registry, entt::entity entity) override
    {
        if (registry.all_of<T>(entity)) registry.remove<T>(entity);
    }
};

struct DestroyEntityCommand : public EntityCommand
{
public:
    DestroyEntityCommand(EntityCommandBuffer &ecb, VirtualEntityID virtualID) : EntityCommand(ecb, virtualID) {}

    void Execute(entt::registry& registry, entt::entity entity) override
    {
        if (registry.valid(entity)) registry.destroy(entity);
    }
};

class EntityCommandBuffer
{
public:
    EntityCommandBuffer() : m_nextVirtualID(0) {}

    VirtualEntityID CreateEntity()
    {
        VirtualEntityID id = m_nextVirtualID++;
        m_idToEntity.emplace_back(entt::null);
        return id;
    }

    VirtualEntityID RegisterEntity(entt::entity entity)
    {
        VirtualEntityID id = m_nextVirtualID++;
        m_idToEntity.emplace_back(entity);
        return id;
    }

    void DestroyEntity(VirtualEntityID id)
    {
        m_commands.emplace_back(std::make_unique<DestroyEntityCommand>(*this, id));
    }
    void DestroyEntity(entt::entity entity)
    {
        VirtualEntityID id = RegisterEntity(entity);
        m_commands.emplace_back(std::make_unique<DestroyEntityCommand>(*this, id));
    }

    template<typename T>
    void AddComponent(VirtualEntityID id, T component)
    {
        m_commands.emplace_back(std::make_unique<AddComponentCommand<T>>(*this, id, component));
    }

    template<typename T>
    void AddComponent(entt::entity entity, T component)
    {
        VirtualEntityID id = RegisterEntity(entity);
        m_commands.emplace_back(std::make_unique<AddComponentCommand<T>>(*this, id, component));
    }

    template<typename T>
    void RemoveComponent(VirtualEntityID id)
    {
        m_commands.emplace_back(std::make_unique<RemoveComponentCommand<T>>(*this, id));
    }

    template<typename T>
    void RemoveComponent(entt::entity entity)
    {
        VirtualEntityID id = RegisterEntity(entity);
        m_commands.emplace_back(std::make_unique<RemoveComponentCommand<T>>(*this, id));
    }

    void AddCommand(std::unique_ptr<EntityCommand> command)
    {
        m_commands.emplace_back(std::move(command));
    }

    void Flush(entt::registry &registry);

    entt::entity GetEntity(VirtualEntityID id) const
    {
        if (id >= m_idToEntity.size()) return entt::null;
        return m_idToEntity[id];
    }

private:
    VirtualEntityID m_nextVirtualID;
    std::vector<entt::entity> m_idToEntity;
    std::vector<std::unique_ptr<EntityCommand>> m_commands;
};
