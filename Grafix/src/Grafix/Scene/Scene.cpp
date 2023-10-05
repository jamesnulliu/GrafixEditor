#include "pch.h"
#include "Scene.h"

#include "Entity.h"

namespace Grafix
{
    Entity Scene::CreateEntity(const std::string& name)
    {
        entt::entity handle = m_Registry.create();
        Entity entity{ handle, this };
        m_Entities.push_back(entity);

        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

        return entity;
    }

    void Scene::RemoveEntity(Entity entity)
    {
        auto iter = std::ranges::find(m_Entities, entity);
        m_Entities.erase(iter);

        m_Registry.destroy((entt::entity)entity);
    }

    Entity Scene::GetEntity(std::string_view tag)
    {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view)
        {
            auto& tagComponent = view.get<TagComponent>(entity);
            if (tagComponent.Tag == tag)
                return Entity{ entity, this };
        }

        return Entity{};
    }

    void Scene::OnUpdate()
    {
    }

    void Scene::OnUpdateEditor()
    {
    }

    void Scene::Clear()
    {
        m_Registry.clear();
        m_Entities.clear();
    }
}