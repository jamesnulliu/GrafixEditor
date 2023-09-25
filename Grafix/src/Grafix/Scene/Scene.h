#pragma once

#include "Entity.h"

#include <glm/glm.hpp>
#include <vector>

namespace Grafix
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity& CreateEntity(const std::string& name);

        std::vector<Entity>& GetEntities() { return m_Entities; }
        const std::vector<Entity>& GetEntities() const { return m_Entities; }

        void RemoveEntity(Entity entity);

        void OnUpdate();
        void OnUpdateEditor();

        void Clear();

        glm::vec3& GetBackgroundColor() { return m_BgColor; }
        const glm::vec3& GetBackgroundColor() const { return m_BgColor; }
    private:
        glm::vec3 m_BgColor{ 0.235f, 0.257f, 0.270f };

        std::vector<Entity> m_Entities{};
        Entity* m_SelectedEntity = nullptr;

        glm::mat4 m_CameraTransform{ 1.0f };
    };
}
