#include "pch.h"
#include "LineAlgorithm.h"

namespace Grafix
{
    void LineAlgorithm::Midpoint(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        uint32_t colorValue = RGBToUint32(color);

        int x0 = (int)p0.x, y0 = (int)p0.y;
        int x1 = (int)p1.x, y1 = (int)p1.y;

        // If |k| > 1, swap x and y.
        bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (XYSwapped)
        {
            XYSwapped = true;
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        // If p0 is to the right of p1, swap them.
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int sy = y0 < y1 ? 1 : -1;
        int a = (y0 - y1) * sy;
        int b = x1 - x0;

        int d = (a << 1) + b;
        int d1 = a << 1, d2 = (a + b) << 1;

        uint32_t dashLength = 40;
        uint32_t gapLength = std::max(dashLength / 5, (uint32_t)1);
        std::vector<char> pattern(dashLength + gapLength, '0');
        for (uint32_t i = 0; i < dashLength; ++i)
            pattern[i] = '1';

        if (lineStyle == LineStyleType::Dotted)
        {
            const char* dot = "000011000011111111111";
            pattern.clear();
            pattern.resize(strlen(dot));
            for (uint32_t i = 0; i < pattern.size(); ++i)
                pattern[i] = dot[i];
        }

        int x = x0, y = y0;
        int count = 0;

        while (x <= x1)
        {
            // Set pixel
            if (lineStyle == LineStyleType::Solid ||
                (lineStyle == LineStyleType::Dashed && pattern[count % pattern.size()] == '1')
                || (lineStyle == LineStyleType::Dotted && pattern[count % pattern.size()] == '1'))
            {
                if (XYSwapped)
                    SetPixel(y, x, colorValue, (uint32_t)lineWidth);
                else
                    SetPixel(x, y, colorValue, (uint32_t)lineWidth);
            }

            if (d < 0)
                y += sy, d += d2;
            else
                d += d1;

            ++x;
            ++count;
        }
    }

    void LineAlgorithm::Bresenham(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        uint32_t colorValue = RGBToUint32(color);

        int x0 = (int)p0.x, y0 = (int)p0.y;
        int x1 = (int)p1.x, y1 = (int)p1.y;

        // If |k| > 1, swap x and y.
        bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (XYSwapped)
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        // If p0 is to the right of p1, swap them.
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int dx = (int)(x1 - x0), dy = (int)(y1 - y0);
        int e = -dx;
        int sy = dy > 0 ? 1 : -1;
        dy = std::abs(dy);

        uint32_t dashLength = 40;
        uint32_t gapLength = std::max(dashLength / 5, (uint32_t)1);
        std::vector<char> pattern(dashLength + gapLength, '0');
        for (uint32_t i = 0; i < dashLength; ++i)
            pattern[i] = '1';

        if (lineStyle == LineStyleType::Dotted)
        {
            const char* dot = "000011000011111111111";
            pattern.clear();
            pattern.resize(strlen(dot));
            for (uint32_t i = 0; i < pattern.size(); ++i)
                pattern[i] = dot[i];
        }

        int x = x0, y = y0;

        for (int i = 0; i <= dx; i++)
        {
            if (lineStyle == LineStyleType::Solid ||
                (lineStyle == LineStyleType::Dashed && pattern[i % pattern.size()] == '1')
                || (lineStyle == LineStyleType::Dotted && pattern[i % pattern.size()] == '1'))
            {
                // Set pixel
                if (XYSwapped)
                    SetPixel(y, x, colorValue, (uint32_t)lineWidth);
                else
                    SetPixel(x, y, colorValue, (uint32_t)lineWidth);
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
}