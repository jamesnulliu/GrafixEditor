#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CircleAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle);
    private:
        static void SetCirclePixels(int centerX, int centerY, int x, int y, uint32_t colorValue, uint32_t lineWidth);
    };
}
