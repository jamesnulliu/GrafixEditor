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
        static void SetPixel(int x, int y, const glm::vec4& color);
        static uint32_t RGBAToUint32(const glm::vec4& color);
    };
}
