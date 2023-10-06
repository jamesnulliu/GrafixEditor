#pragma once

#include <entt.hpp>

#include <glm/glm.hpp>
#include <vector>

namespace Grafix
{
    class Entity;

    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity CreateEntity(const std::string& name);
        void RemoveEntity(Entity entity);

        Entity GetEntity(std::string_view tag);
        const std::vector<Entity>& GetEntities() const { return m_Entities; }

        template<typename... T>
        auto GetEntitiesWith()
        {
            return m_Registry.view<T...>();
        }

        void OnUpdate();
        void OnUpdateEditor();

        void Clear();
    private:
        entt::registry m_Registry;
        std::vector<Entity> m_Entities{};

        friend class Entity;
        friend class HierarchyPanel;
    };
}
