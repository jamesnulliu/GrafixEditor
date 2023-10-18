#include "pch.h"
#include "GraphicsAlgorithm.h"

#include "LineAlgorithm.h"

namespace Grafix
{
    uint32_t* s_PixelData = nullptr;
    uint32_t s_Width = 0, s_Height = 0;

    void GraphicsAlgorithm::UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height)
    {
        s_PixelData = pixelData;
        s_Width = width;
        s_Height = height;
    }

    void GraphicsAlgorithm::DrawSquare(const glm::vec2& center, float length, const glm::vec3& color)
    {
        for (int i = center.y - length / 2.0f; i < center.y + length / 2.0f; i++)
        {
            for (int j = center.x - length / 2.0f; j < center.x + length / 2.0f; j++)
            {
                SetPixel(j, i, RGBToUint32(color), 1);
            }
        }
    }

    uint32_t GraphicsAlgorithm::GetWidth()
    {
        return s_Width;
    }

    uint32_t GraphicsAlgorithm::GetHeight()
    {
        return s_Height;
    }

    void GraphicsAlgorithm::SetPixel(int x, int y, uint32_t colorValue, uint32_t width)
    {
        int halfWidth = (width - 1) >> 1;

        for (int j = y - halfWidth; j <= y + halfWidth; ++j)
        {
            for (int i = x - halfWidth; i <= x + halfWidth; ++i)
            {
                if (i >= 0 && i < s_Width && j >= 0 && j < s_Height)
                    s_PixelData[i + j * s_Width] = colorValue;
            }
        }
    }

    uint32_t GraphicsAlgorithm::GetPixelValue(int x, int y)
    {
        return s_PixelData[x + y * s_Width];
    }
}