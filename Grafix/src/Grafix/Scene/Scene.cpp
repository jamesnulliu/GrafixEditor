#include "pch.h"
#include "Scene.h"

namespace Grafix
{
    Entity& Scene::CreateEntity(const std::string& name)
    {
        m_Entities.emplace_back();

        Entity& entity = m_Entities.back();
        auto& tag = entity.AddComponent<TagComponent>(name)->Tag;
        tag = name.empty() ? "Entity" : name;

        return m_Entities.back();
    }

    void Scene::RemoveEntity(Entity entity)
    {
        auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);

        if (it != m_Entities.end())
            m_Entities.erase(it);
    }

    void Scene::OnUpdate()
    {
    }

    void Scene::OnUpdateEditor()
    {
    }

    void Scene::Clear()
    {
        m_Entities.clear();
    }
}