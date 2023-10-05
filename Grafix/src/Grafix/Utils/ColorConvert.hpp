#pragma once

#include <glm/glm.hpp>

namespace Grafix
{
    inline uint32_t RGBToUint32(const glm::vec3& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);

        return (255 << 24) | (b << 16) | (g << 8) | r;
    }
}