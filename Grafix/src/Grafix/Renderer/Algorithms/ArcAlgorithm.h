#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ArcAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Draw(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec4& color, bool showCenter, bool showRadius);
    private:
        static void Midpoint(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec4& color);
        static void SetArcPixels(int centerX, int centerY, int x, int y, float radius, float beginAngle, float endAngle, const glm::vec4& color);
        static bool IsInRange(int x, int y, float beginAngle, float endAngle);
    };
}