#include "pch.h"
#include "ArcAlgorithm.h"

#include "LineAlgorithm.h"
#include "CircleAlgorithm.h"

namespace Grafix
{
    void ArcAlgorithm::Midpoint(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth)
    {
        if (glm::abs(angle2 - angle1) < 1e-9f)
            return;

        uint32_t colorValue = RGBToUint32(color);

        // Restrict angles to 0 <= angle < 360
        angle1 = glm::mod(angle1, 360.0f);
        angle2 = glm::mod(angle2, 360.0f);

        float beginAngle = glm::min(angle1, angle2), endAngle = glm::max(angle1, angle2);
        if ((!major && endAngle - beginAngle > 180.0f)
            || (major && endAngle - beginAngle <= 180.0f))
        {
            std::swap(beginAngle, endAngle);
            endAngle += 360.0f;  // 0 <= endAngle < 540
        }

        int a = 0, b = (int)radius, e = 1 - (int)radius;

        while (a <= b)
        {
            SetArcPixels((int)center.x, (int)center.y, a, b, radius, beginAngle, endAngle, colorValue, (uint32_t)lineWidth);

            if (e < 0)
            {
                e += 2 * a + 3;
            }
            else
            {
                e += 2 * (a - b) + 5;
                b--;
            }
            a++;
        }
    }

    void ArcAlgorithm::SetArcPixels(int centerX, int centerY, int x, int y, float radius, float beginAngle, float endAngle, uint32_t colorValue, uint32_t lineWidth)
    {
        if (IsInRange(x, y, beginAngle, endAngle))
            SetPixel(centerX + x, centerY + y, colorValue, lineWidth);
        if (IsInRange(y, x, beginAngle, endAngle))
            SetPixel(centerX + y, centerY + x, colorValue, lineWidth);

        if (IsInRange(-x, y, beginAngle, endAngle))
            SetPixel(centerX - x, centerY + y, colorValue, lineWidth);
        if (IsInRange(-y, x, beginAngle, endAngle))
            SetPixel(centerX - y, centerY + x, colorValue, lineWidth);

        if (IsInRange(-x, -y, beginAngle, endAngle))
            SetPixel(centerX - x, centerY - y, colorValue, lineWidth);
        if (IsInRange(-y, -x, beginAngle, endAngle))
            SetPixel(centerX - y, centerY - x, colorValue, lineWidth);

        if (IsInRange(x, -y, beginAngle, endAngle))
            SetPixel(centerX + x, centerY - y, colorValue, lineWidth);
        if (IsInRange(y, -x, beginAngle, endAngle))
            SetPixel(centerX + y, centerY - x, colorValue, lineWidth);
    }

    bool ArcAlgorithm::IsInRange(int x, int y, float beginAngle, float endAngle)
    {
        float angle = glm::degrees(glm::atan((float)y, (float)x));
        if (y < 0)
            angle += 360.0f;

        if (360.0f + angle <= endAngle)
            angle += 360.0f;

        return angle >= beginAngle && angle <= endAngle;
    }
}