/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/command_buffer.h"
#include "ecs/basic_components.h"

void EntityCommandBuffer::Flush(entt::registry &registry)
{
    for (entt::entity &entity : m_idToEntity)
    {
        if (entity == entt::null)
        {
            entity = registry.create();
        }
    }

    for (std::unique_ptr<EntityCommand> &cmd : m_commands)
    {
        VirtualEntityID id = cmd->GetVirtualId();
        if (id >= m_idToEntity.size()) continue;

        entt::entity entity = m_idToEntity[id];
        if (registry.valid(entity) == false) continue;

        cmd->Execute(registry, entity);
    }

    m_commands.clear();
    m_idToEntity.clear();
    m_nextVirtualID = 0;
}
