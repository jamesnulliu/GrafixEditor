#include "pch.h"
#include "CurveAlgorithm.h"

#include "Grafix/Math/Math.h"

#include <iostream>
namespace Grafix
{
    void CurveAlgorithm::Bezier(const std::vector<glm::vec2>& controlPoints, float step, const glm::vec3& color)
    {
        int n = controlPoints.size();

        for (float u = 0; u <= 1; u += step)
        {
            glm::vec2 point{ 0.0f, 0.0f };
            for (int i = 0; i < n; ++i)
            {
                float factor = Math::NChooseK(n - 1, i) * pow(u, i) * pow(1 - u, n - 1 - i);
                point += controlPoints[i] * factor;
            }
            SetPixel((int)point.x, (int)point.y, color);
        }
    }

    void CurveAlgorithm::NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color, const std::vector<float>& knots, const std::vector<float>& weights)
    {
        for (float u = 0; u <= 1; u += step)
        {
            glm::vec2 point{ 0.0f, 0.0f };
            for (int i = 0; i < controlPoints.size(); i++)
            {
                float factor = BaseFunction(i, order, u, knots);
                point += controlPoints[i] * factor;
            }

            SetPixel((int)point.x, (int)point.y, color);
        }
    }

    float CurveAlgorithm::BaseFunction(int i, int k, float u, const std::vector<float>& knots)
    {
        if (k == 1)
        {
            if (knots[i] <= u && u <= knots[i + 1])
                return 1.0f;
            else
                return 0.0f;
        }
        else
        {
            float div1 = knots[i + k - 1] - knots[i];
            float div2 = knots[i + k] - knots[i + 1];

            if (div1 == 0.0f)
                div1 = 1.0f;
            if (div2 == 0.0f)
                div2 = 1.0f;

            return (u - knots[i]) / div1 * BaseFunction(i, k - 1, u, knots)
                + (knots[i + k] - u) / div2 * BaseFunction(i + 1, k - 1, u, knots);
        }
    }
}