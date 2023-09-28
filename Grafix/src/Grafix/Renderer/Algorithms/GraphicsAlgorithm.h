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

        // Auxiliary
        static glm::vec4 s_AuxColor;
        static void DrawAuxCross(const glm::vec2& point);
        static void DrawAuxRadius(const glm::vec2& center, float radius, float angle);
    };
}
