#include "pch.h"
#include "GraphicsAlgorithm.h"

#include "LineAlgorithm.h"
#include "Grafix/Utils/ColorConvert.hpp"

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

    uint32_t GraphicsAlgorithm::GetWidth()
    {
        return s_Width;
    }

    uint32_t GraphicsAlgorithm::GetHeight()
    {
        return s_Height;
    }

    void GraphicsAlgorithm::SetPixel(int x, int y, const glm::vec3& color)
    {
        SetPixel(x, y, RGBToUint32(color));
    }

    void GraphicsAlgorithm::SetPixel(int x, int y, uint32_t color)
    {
        if (x < 0 || x >= s_Width || y < 0 || y >= s_Height)
            return;

        s_PixelData[(uint32_t)x + (uint32_t)y * s_Width] = color;
    }
    
    uint32_t GraphicsAlgorithm::GetPixelValue(int x, int y)
    {
        return s_PixelData[x + y * s_Width];
    }
}