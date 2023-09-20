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

    inline void GraphicsAlgorithm::SetPixel(int x, int y, uint32_t color)
    {
        if (x < 0 || x >= s_Width || y < 0 || y >= s_Height)
            return;

        s_PixelData[(uint32_t)x + (uint32_t)y * s_Width] = color;
    }

    // ------------------------------------------------
    // --------------------- Line ---------------------
    // ------------------------------------------------

    void LineAlgorithm::Midpoint(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style, uint32_t dashLength)
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

        int gapLength = std::max((int)dashLength / 5, 1);
        std::vector<uint8_t> bitString(dashLength + gapLength, 0);
        for (int i = 0; i < dashLength; ++i)
            bitString[i] |= 1;

        int x = (int)p0.x;
        int y = (int)p0.y;

        int count = 0;
        while (x < (int)p1.x)
        {
            // Set pixel
            if (style == LineStyle::Solid ||
                (style == LineStyle::Dashed && bitString[count % bitString.size()]))
            {
                if (XYSwapped)
                    SetPixel(y, x, color);
                else
                    SetPixel(x, y, color);
            }

            if (d < 0)
                y += sy, d += d2;
            else
                d += d1;

            ++x;
            ++count;
        }
    }

    void LineAlgorithm::Bresenham(glm::vec2 p0, glm::vec2 p1, uint32_t color, LineStyle style, uint32_t dashLength)
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

        int gapLength = std::max((int)dashLength / 5, 1);
        std::vector<uint8_t> bitString(dashLength + gapLength, 0);
        for (int i = 0; i < dashLength; ++i)
            bitString[i] |= 1;

        int x = (int)p0.x;
        int y = (int)p0.y;

        for (int i = 0; i <= dx; i++)
        {
            if (style == LineStyle::Solid ||
                (style == LineStyle::Dashed && bitString[i % bitString.size()]))
            {
                // Set pixel
                if (XYSwapped)
                    SetPixel(y, x, color);
                else
                    SetPixel(x, y, color);
            }

            ++x;
            e += dy << 1;

            if (e >= 0)
            {
                y += sy;
                e -= dx << 1;
            }
        }
    }

    // ------------------------------------------------
    // -------------------- Circle --------------------
    // ------------------------------------------------

    void CircleAlgorithm::Midpoint(const glm::vec2& center, float radius, uint32_t color)
    {
        int centerX = (int)center.x;
        int centerY = (int)center.y;

        int x = 0, y = (int)radius;
        int e = 1 - (int)radius;

        while (x <= y)
        {
            SetCirclePixels(centerX, centerY, x, y, color);

            if (e < 0)
            {
                e += 2 * x + 3;
            } else
            {
                e += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }

    void CircleAlgorithm::SetCirclePixels(int centerX, int centerY, int x, int y, uint32_t color)
    {
        SetPixel(centerX + x, centerY + y, color);
        SetPixel(centerX - x, centerY + y, color);
        SetPixel(centerX - x, centerY - y, color);
        SetPixel(centerX + x, centerY - y, color);
        SetPixel(centerX + y, centerY + x, color);
        SetPixel(centerX - y, centerY + x, color);
        SetPixel(centerX - y, centerY - x, color);
        SetPixel(centerX + y, centerY - x, color);
    }
}