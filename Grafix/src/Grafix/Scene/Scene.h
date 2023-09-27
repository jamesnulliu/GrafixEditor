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

        glm::vec3& GetBackgroundColor() { return m_BgColor; }
        const glm::vec3& GetBackgroundColor() const { return m_BgColor; }
    private:
        glm::vec3 m_BgColor{ 0.235f, 0.257f, 0.270f };

        entt::registry m_Registry;
        std::vector<Entity> m_Entities{};

        friend class Entity;
    };
}
