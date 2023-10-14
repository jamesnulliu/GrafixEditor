#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CurveAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Bezier(const std::vector<glm::vec2>& point, float step, const glm::vec3& color);
        // n+1个控制点，k阶，次数=k-1
        static void NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color);
    private:
        static float BaseFunction(int i, int k, float u, const std::vector<float>& knots);
        static std::vector<float> GenerateKnots(int n, int k);
    };
}
