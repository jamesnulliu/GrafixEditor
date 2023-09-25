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
        for (int i = 0; i < m_Entities.size(); i++)
        {
            if (m_Entities[i] == entity)
            {
                m_Entities.erase(m_Entities.begin() + i);
                break;
            }
        }
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