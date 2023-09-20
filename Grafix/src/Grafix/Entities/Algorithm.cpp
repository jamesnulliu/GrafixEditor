#include "pch.h"
#include "Algorithm.h"

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

    void LineAlgorithm::Midpoint(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style)
    {
        // If |k| > 1, swap x and y.
        bool XYSwapped = false;
        if (std::abs(p1.y - p0.y) > std::abs(p1.x - p0.x))
        {
            XYSwapped = true;
            std::swap(p0.x, p0.y);
            std::swap(p1.x, p1.y);
        }

        // If p0 is to the right of p1, swap them.
        if (p0.x > p1.x)
            std::swap(p0, p1);

        int sy = (p0.y < p1.y) ? 1 : -1;
        int a = (p0.y - p1.y) * sy;
        int b = (int)(p1.x - p0.x);

        int d = (a << 1) + b;
        int d1 = a << 1, d2 = (a + b) << 1;

        uint32_t x = (uint32_t)p0.x;
        uint32_t y = (uint32_t)p0.y;

        while (x < (uint32_t)p1.x)
        {
            // Set pixel
            if (XYSwapped)
                s_PixelData[x * s_Width + y] = color;
            else
                s_PixelData[y * s_Width + x] = color;

            if (d < 0)
                y += sy, d += d2;
            else
                d += d1;

            ++x;
        }
    }

    void LineAlgorithm::Bresenham(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style)
    {
        // If |k| > 1, swap x and y.
        bool XYSwapped = false;
        if (std::abs(p1.y - p0.y) > std::abs(p1.x - p0.x))
        {
            XYSwapped = true;
            std::swap(p0.x, p0.y);
            std::swap(p1.x, p1.y);
        }

        // If p0 is to the right of p1, swap them.
        if (p0.x > p1.x)
            std::swap(p0, p1);

        int dx = (int)(p1.x - p0.x), dy = (int)(p1.y - p0.y);
        int e = -dx;
        int sy = dy > 0 ? 1 : -1;
        dy = std::abs(dy);

        uint32_t x = (uint32_t)p0.x;
        uint32_t y = (uint32_t)p0.y;

        for (int i = 0; i <= dx; i++)
        {
            // Set pixel
            if (XYSwapped)
                s_PixelData[x * s_Width + y] = color;
            else
                s_PixelData[y * s_Width + x] = color;

            ++x;
            e += dy << 1;

            if (e >= 0)
            {
                y += sy;
                e -= dx << 1;
            }
        }
    }
}