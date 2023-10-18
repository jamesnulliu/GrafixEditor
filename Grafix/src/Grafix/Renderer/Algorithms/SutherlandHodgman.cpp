#include "pch.h"
#include "SutherlandHodgman.h"

namespace Grafix
{
    //裁剪区域需要为顺时针
    std::vector<glm::vec2> SutherlandHodgman::SutherHodgClip(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clipArea)
    {
        // TEMP
        std::vector<glm::vec2> clipedVertices(vertices);
        clipedVertices.pop_back();

        std::vector<glm::vec2> area = clipArea;

        for (int i = 0; i < area.size(); i++)
        {
            int k = (i + 1) % area.size();
            clipedVertices = Clip(clipedVertices, area[i], area[k]);
        }

        if (clipedVertices.size() != 0)
            clipedVertices.push_back(clipedVertices[0]);

        return clipedVertices;
    }

    std::vector<glm::vec2> SutherlandHodgman::Clip(std::vector<glm::vec2> vertices, const glm::vec2& point1, const glm::vec2& point2)
    {
        std::vector<glm::vec2> outputList;

        for (int i = 0; i < vertices.size(); i++)
        {
            // k is the next vertex
            int k = (i + 1) % vertices.size();
            glm::vec2 pointI(vertices[i]);
            glm::vec2 pointK(vertices[k]);

            int pos_i = (point2.x - point1.x) * (pointI.y - point1.y) - (point2.y - point1.y) * (pointI.x - point1.x);
            int pos_k = (point2.x - point1.x) * (pointK.y - point1.y) - (point2.y - point1.y) * (pointK.x - point1.x);

            if (pos_i < 0 && pos_k < 0)
            {
                outputList.push_back(pointK);
            }
            else if (pos_i >= 0 && pos_k < 0)
            {
                outputList.push_back(Intersection(pointI, pointK, point1, point2));
                outputList.push_back(pointK);
            }
            else if (pos_i < 0 && pos_k >= 0)
            {
                outputList.push_back(Intersection(pointI, pointK, point1, point2));
            }
        }

        return outputList;
    }

    glm::vec2 SutherlandHodgman::Intersection(const glm::vec2& point1, const glm::vec2& point2, const glm::vec2& point3, const glm::vec2& point4)
    {
        glm::vec2 point;
        point.x = ((point1.x * point2.y - point1.y * point2.x) * (point3.x - point4.x) - (point1.x - point2.x) * (point3.x * point4.y - point3.y * point4.x)) / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));
        point.y = ((point1.x * point2.y - point1.y * point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x * point4.y - point3.y * point4.x)) / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));
        return point;
    }
}