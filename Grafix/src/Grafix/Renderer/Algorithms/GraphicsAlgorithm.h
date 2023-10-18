#pragma once

#include <glm/glm.hpp>

#include "Grafix/Scene/Components.h"
#include "Grafix/Utils/ColorConvert.hpp"

namespace Grafix
{
    class GraphicsAlgorithm
    {
    public:
        static void UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height);

        static void DrawSquare(const glm::vec2& center, float length, const glm::vec3& color);
    protected:
        static uint32_t GetWidth();
        static uint32_t GetHeight();

        static void SetPixel(int x, int y, uint32_t colorValue, uint32_t width);

        static uint32_t GetPixelValue(int x, int y);
    };
}
