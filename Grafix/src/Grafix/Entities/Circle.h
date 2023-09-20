#pragma once

#include "Entity.h"

namespace Grafix
{
    class Circle : public Entity
    {
    public:
        Circle(const std::string& name) : Entity(name) {}

        inline glm::vec3& GetCenter() { return m_Center; }
        inline const glm::vec3& GetCenter() const { return m_Center; }
        inline float& GetRadius() { return m_Radius; }

        inline SpriteRendererComponent& GetSpriteRenderer() { return m_SpriteRenderer; }
        inline const SpriteRendererComponent& GetSpriteRenderer() const { return m_SpriteRenderer; }
    private:
        glm::vec3 m_Center{ 50.0f, 0.0f, 0.0f };
        float m_Radius = 1.0f;
        SpriteRendererComponent m_SpriteRenderer;
    };
}
