#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ArcAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Draw(const glm::vec2& center, const glm::vec2&  beginPoint , 
            const glm::vec2& thirdPoint ,const glm::vec4 & color, bool majorArc);
    private:
        static void MidPointArc(const glm::vec2& center, const glm::vec2& beginPoint,
            const glm::vec2 thirdPoint, const glm::vec4& color, bool majorArc);
        static void SetMajorArcPixels(int centerX, int centerY, int x, int y,
            const glm::vec4& color, const glm::vec3 f1, const glm::vec3 f2, const glm::vec2& midpoint);
        static void SetMinorArcPixels(int centerX, int centerY, int x, int y,
            const glm::vec4& color, const glm::vec3 f1, const glm::vec3 f2, const glm::vec2& midPoint);
        static glm::vec3 GetLineArguments(const glm::vec2& beginPoint, const glm::vec2& endPoint);
        static bool IsPointAboveLine(int x,int y, const glm::vec3& lineArguments);
    };
}