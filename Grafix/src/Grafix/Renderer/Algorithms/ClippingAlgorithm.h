#pragma once
#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ClippingAlgorithm : public GraphicsAlgorithm
    {
        // Line
    public:
        static std::vector<glm::vec2> CohenSutherland(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop);
        static std::vector<glm::vec2> MidPoint(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop);
    private:
        static int Encode(glm::vec2 point, const glm::vec2& leftBottom, const glm::vec2& rightTop);

        //Polygon
    public:
        static std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clippingArea);
    private:
        static std::vector<glm::vec2> Clip(std::vector<glm::vec2> vertices, const glm::vec2& point1, const glm::vec2& point2);
        static glm::vec2 Intersection(const glm::vec2& point1, const glm::vec2& point2, const glm::vec2& point3, const glm::vec2& point4);
    };
}
