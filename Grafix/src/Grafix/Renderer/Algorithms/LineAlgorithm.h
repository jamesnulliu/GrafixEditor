#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class LineAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Draw(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, LineStyle style = LineStyle::Solid, float dashLength = 5.0f);
    private:
        static void Midpoint(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, LineStyle style, uint32_t dashLength);
        static void Bresenham(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, LineStyle style, uint32_t dashLength);
    };
}
