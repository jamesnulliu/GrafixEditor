#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CircleAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Draw(const glm::vec2& center, float radius, const glm::vec3& color);
    private:
        static void Midpoint(const glm::vec2& center, float radius, const glm::vec3& color);
        static void SetCirclePixels(int centerX, int centerY, int x, int y, const glm::vec3& color);
    };
}
