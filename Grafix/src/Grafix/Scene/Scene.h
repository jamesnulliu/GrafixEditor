#pragma once

#include "Grafix/Entities/Line.h"
#include "Grafix/Entities/Circle.h"

#include <glm/glm.hpp>
#include <vector>

namespace Grafix
{
    class Scene
    {
    public:
        Scene();
        ~Scene() = default;

        void AddLine();
        void AddCircle();

        std::vector<Line>& GetLines() { return m_Lines; }
        const std::vector<Line>& GetLines() const { return m_Lines; }
        std::vector<Circle>& GetCircles() { return m_Circles; }
        const std::vector<Circle>& GetCircles() const { return m_Circles; }

        void OnUpdate();

        glm::vec4& GetBackgroundColor() { return m_BgColor; }
        const glm::vec4& GetBackgroundColor() const { return m_BgColor; }
    private:
        glm::vec4 m_BgColor{ 0.163f, 0.197f, 0.206f, 1.0f };
        std::vector<Line> m_Lines;
        std::vector<Circle> m_Circles;
    };
}
