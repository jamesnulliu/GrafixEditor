#include "pch.h"
#include "CircleAlgorithm.h"

namespace Grafix
{
    void CircleAlgorithm::Midpoint(const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        uint32_t colorValue = RGBToUint32(color);

        int centerX = (int)center.x;
        int centerY = (int)center.y;

        int x = 0, y = (int)radius;
        int e = 1 - (int)radius;

        while (x <= y)
        {
            SetCirclePixels(centerX, centerY, x, y, colorValue, (uint32_t)lineWidth);

            if (e < 0)
            {
                e += 2 * x + 3;
            }
            else
            {
                e += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }

    void CircleAlgorithm::SetCirclePixels(int centerX, int centerY, int x, int y, uint32_t colorValue, uint32_t lineWidth)
    {
        SetPixel(centerX + x, centerY + y, colorValue, lineWidth);
        SetPixel(centerX + y, centerY + x, colorValue, lineWidth);

        SetPixel(centerX - x, centerY + y, colorValue, lineWidth);
        SetPixel(centerX - y, centerY + x, colorValue, lineWidth);

        SetPixel(centerX - x, centerY - y, colorValue, lineWidth);
        SetPixel(centerX - y, centerY - x, colorValue, lineWidth);

        SetPixel(centerX + x, centerY - y, colorValue, lineWidth);
        SetPixel(centerX + y, centerY - x, colorValue, lineWidth);
    }
}