#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CurveAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Bezier(const std::vector<glm::vec2>& point, float step, const glm::vec3& color);
        // n+1�����Ƶ㣬k�ף�����=k-1
        static void NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color, const std::vector<float>& knots, const std::vector<float>& weights);
    private:
        static float BaseFunction(int i, int k, float u, const std::vector<float>& knots);
    };
}
