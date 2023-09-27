#include "pch.h"
#include "GraphicsAlgorithm.h"
#include "LineAlgorithm.h"

namespace Grafix
{
    uint32_t* s_PixelData = nullptr;
    uint32_t s_Width = 0, s_Height = 0;

    glm::vec4 GraphicsAlgorithm::s_AuxColor{ 0.8f, 0.8f, 0.8f, 1.0f };

    void GraphicsAlgorithm::UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height)
    {
        s_PixelData = pixelData;
        s_Width = width;
        s_Height = height;
    }

    void GraphicsAlgorithm::SetPixel(int x, int y, const glm::vec4& color)
    {
        if (x < 0 || x >= s_Width || y < 0 || y >= s_Height)
            return;

        s_PixelData[(uint32_t)x + (uint32_t)y * s_Width] = RGBAToUint32(color);
    }

    uint32_t GraphicsAlgorithm::RGBAToUint32(const glm::vec4& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    void GraphicsAlgorithm::DrawAuxCross(const glm::vec2& point)
    {
        LineAlgorithm::Draw({ point.x - 3.0f, point.y }, { point.x + 4.0f, point.y }, s_AuxColor);
        LineAlgorithm::Draw({ point.x, point.y - 3.0f }, { point.x, point.y + 4.0f }, s_AuxColor);
    }

    void GraphicsAlgorithm::DrawAuxRadius(const glm::vec2& center, float radius, float angle)
    {
        glm::vec2 delta = radius * glm::vec2{ glm::cos(glm::radians(angle)), glm::sin(glm::radians(angle)) };
        LineAlgorithm::Draw(center, center + delta, s_AuxColor, LineStyle::Dashed, 20.0f);
    }
}