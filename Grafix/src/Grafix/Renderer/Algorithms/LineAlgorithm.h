#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class LineAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid);
        static void Bresenham(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid);
    };
}
