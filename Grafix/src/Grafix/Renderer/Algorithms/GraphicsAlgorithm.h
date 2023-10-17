#pragma once

#include <glm/glm.hpp>

#include "Grafix/Scene/Components.h"

namespace Grafix
{
    class GraphicsAlgorithm
    {
    public:
        static void UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height);
    protected:
        static uint32_t GetWidth();
        static uint32_t GetHeight();

        static void SetPixel(int x, int y, const glm::vec3& color,float width = 5.0f);
        static void SetPixel(int x, int y, uint32_t color,float width = 5.0f);

        static uint32_t GetPixelValue(int x, int y);
    };
}
