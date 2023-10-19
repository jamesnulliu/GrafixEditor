#pragma once
#include "GraphicsAlgorithm.h"
#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ClipAlgorithm : public GraphicsAlgorithm
    {
    public:
        static int encode(double x, double y, double XL, double XR, double YB, double YT);
        static void CS_LineClip(const glm::vec2& p0,const glm::vec2& p1, glm::vec2 p2, glm::vec2 p3 , const TransformComponent& transform, const glm::vec3& color, LineStyle style, float dashLength, const glm::mat3& matrix);
    private:
    };
}
