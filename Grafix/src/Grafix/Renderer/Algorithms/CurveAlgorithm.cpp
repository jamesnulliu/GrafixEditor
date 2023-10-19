#include "pch.h"
#include "CurveAlgorithm.h"
#include "Grafix/Math/Math.h"

namespace Grafix
{
    void CurveAlgorithm::Bezier(const std::vector<glm::vec2>& controlPoints, float step, const glm::vec3& color)
    {
        uint32_t colorValue = RGBToUint32(color);
        int n = controlPoints.size();
        if (n < 2)
            return;
        for (float u = 0; u <= 1; u += step)
        {
            glm::vec2 point{ 0.0f, 0.0f };
            point = GetBezierPoint(controlPoints, u);
            SetPixel((int)point.x, (int)point.y, colorValue);
        }
    }

    void CurveAlgorithm::NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color, std::vector<float>& knots, std::vector<float>& weights)
    {
        uint32_t colorValue = RGBToUint32(color);

        if (weights.size() != controlPoints.size() || knots.size() != controlPoints.size() + order)
        {
            GenerateKnots(controlPoints.size(), order, knots);
            GenerateWeights(controlPoints.size(), weights);
        }

        for (float u = 0; u <= 1; u += step)
        {
            glm::vec2 point{ 0.0f, 0.0f };
            for (int i = 0; i < controlPoints.size(); i++)
            {
                float factor = BaseFunction(i, order, u, knots);
                point += controlPoints[i] * factor;
            }
            
            SetPixel((int)point.x, (int)point.y, colorValue);
        }
    }

    glm::vec2 CurveAlgorithm::GetBezierPoint(const std::vector<glm::vec2>& points, float u)
    {
        int n = points.size();
        std::vector<glm::vec2> bezierPoint(n-1);
        for(int i=0;i<n-1;i++)
            bezierPoint[i] = (1 - u) * points[i] + u * points[i + 1];
        if (n == 2)
            return bezierPoint[0];
        else 
            return GetBezierPoint(bezierPoint, u);
    }

    float CurveAlgorithm::BaseFunction(int i, int order, float u, const std::vector<float>& knots)
    {
        if (order == 1)
        {
            if (knots[i] <= u && u <= knots[i + 1])
                return 1.0f;
            else
                return 0.0f;
        }
        else
        {
            float div1 = knots[i + order - 1] - knots[i];
            float div2 = knots[i + order] - knots[i + 1];

            if (div1 == 0.0f)
                div1 = 1.0f;
            if (div2 == 0.0f)
                div2 = 1.0f;

            return (u - knots[i]) / div1 * BaseFunction(i, order - 1, u, knots)
                + (knots[i + order] - u) / div2 * BaseFunction(i + 1, order - 1, u, knots);
        }
    }

    void CurveAlgorithm::GenerateKnots(int controlPointSize, int order, std::vector<float>& knots)
    {
        knots.clear();
        int n = controlPointSize + order;
        knots.resize(n);

        srand(time(nullptr));

        for (int i = 0; i < order; i++)
            knots[i] = 0;

        for (int i = n - 1; i >= n - order; i--)
            knots[i] = 1;

        for (int i = order; i <= n - order - 1; i++)
        {
            float r = (float)rand() / (RAND_MAX * n / 3);
            r = r * (1 - knots[i - 1]) + knots[i - 1];
            knots[i] = r;
        }
    }

    void CurveAlgorithm::GenerateWeights(int controlPointSize, std::vector<float>& weights)
    {
        weights.clear();
        srand(time(nullptr));
        for (int i = 0; i < controlPointSize; i++)
        {
            float r = (float)rand() / RAND_MAX;
            weights.push_back(r);
        }

        float sum = 0.0f;
        for (auto t : weights)
            sum += t;
        for (auto t : weights)
            t /= sum;
    }
}