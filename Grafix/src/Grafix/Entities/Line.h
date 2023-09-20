#pragma once

#include "Entity.h"

#include <vector>

namespace Grafix
{
    enum class LineStyle
    {
        Solid, Dashed
    };

    class Line : public Entity
    {
    public:
        Line(const std::string& name) : Entity(name) {}
        ~Line() = default;

        glm::vec3& GetPoint0() { return m_Point0; }
        glm::vec3& GetPoint1() { return m_Point1; }

        float& GetWidth() { return m_Width; }
        SpriteRendererComponent& GetSpriteRenderer() { return m_SpriteRenderer; }

        LineStyle& GetStyle() { return m_Style; }
        uint32_t& GetDashLength() { return m_DashLength; }
    protected:
        glm::vec3 m_Point0{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Point1{ 0.0f, 0.0f, 0.0f };

        float m_Width = 1.0f;
        LineStyle m_Style = LineStyle::Solid;

        SpriteRendererComponent m_SpriteRenderer;
        uint32_t m_DashLength = 5.0f;
    };
}
