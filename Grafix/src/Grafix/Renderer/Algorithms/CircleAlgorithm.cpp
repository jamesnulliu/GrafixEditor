#include "pch.h"
#include "CircleAlgorithm.h"

namespace Grafix
{
    void CircleAlgorithm::Draw(const glm::vec2& center, float radius, const glm::vec4& color)
    {
        Midpoint(center, radius, color);
    }

    void CircleAlgorithm::Midpoint(const glm::vec2& center, float radius, const glm::vec4& color)
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

    void CircleAlgorithm::SetCirclePixels(int centerX, int centerY, int x, int y, const glm::vec4& color)
    {
        SetPixel(centerX + x, centerY + y, color);
        SetPixel(centerX + y, centerY + x, color);

        SetPixel(centerX - x, centerY + y, color);
        SetPixel(centerX - y, centerY + x, color);

        SetPixel(centerX - x, centerY - y, color);
        SetPixel(centerX - y, centerY - x, color);

        SetPixel(centerX + x, centerY - y, color);
        SetPixel(centerX + y, centerY - x, color);
    }
}