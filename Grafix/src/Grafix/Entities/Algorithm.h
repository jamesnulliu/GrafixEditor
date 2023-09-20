#pragma once

#include "Grafix/Entities/Line.h"

#include <glm/glm.hpp>

namespace Grafix
{
    class GraphicsAlgorithm
    {
    public:
        static void UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height);
    protected:
        static void SetPixel(int x, int y, uint32_t color);
    };

    class LineAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style = LineStyle::Solid, uint32_t dashLength = 5.0f);
        static void Bresenham(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style = LineStyle::Solid, uint32_t dashLength = 5.0f);
    };

    class CircleAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(const glm::vec2& center, float radius, uint32_t color);
    protected:
        static void SetCirclePixels(int centerX, int centerY, int a, int b, uint32_t color);
    };
}
