#include "pch.h"

#include "ClippingAlgorithm.h"

namespace Grafix
{
    // **************************************************************************************************************
    // Line Clipping
    // **************************************************************************************************************

    static const int LEFT = 1;
    static const int RIGHT = 2;
    static const int BOTTOM = 4;
    static const int TOP = 8;

    ////void ClipAlgorithm::CS_LineClip(const glm::vec2& p0,const glm::vec2& p1, glm::vec2 p2, glm::vec2 p3, const TransformComponent& transform, const glm::vec3& color, LineStyle style, float dashLength, const glm::mat3& s_ViewMatrix)
    ////{
    ////    LineAlgorithm::Draw(
    ////        Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), P0)),
    ////        Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), P1)),
    ////        color, style, dashLength
    ////    );
    ////}

    std::vector<glm::vec2> ClippingAlgorithm::CohenSutherland(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop)
    {
        int code = 0;
        int code1 = Encode(beginPoint, leftBottom, rightTop);
        int code2 = Encode(endPoint, leftBottom, rightTop);

        int x = 0, y = 0;
        std::vector<glm::vec2> res;

        while (code1 != 0 || code2 != 0)
        {
            if ((code1 & code2) != 0)
            {
                res.push_back({ -1, -1 });
                res.push_back({ -1, -1 });
                return res;
            }

            code = code1 == 0 ? code2 : code1;

            if ((code & LEFT) != 0)
            {
                x = leftBottom.x;
                y = beginPoint.y + (endPoint.y - beginPoint.y) * (x - beginPoint.x) / (endPoint.x - beginPoint.x);
            }
            else if ((code & RIGHT) != 0)
            {
                x = rightTop.x;
                y = beginPoint.y + (endPoint.y - beginPoint.y) * (x - beginPoint.x) / (endPoint.x - beginPoint.x);
            }
            else if ((code & BOTTOM) != 0)
            {
                y = leftBottom.y;
                x = beginPoint.x + (endPoint.x - beginPoint.x) * (y - beginPoint.y) / (endPoint.y - beginPoint.y);
            }
            else if ((code & TOP) != 0)
            {
                y = rightTop.y;
                x = beginPoint.x + (endPoint.x - beginPoint.x) * (y - beginPoint.y) / (endPoint.y - beginPoint.y);
            }

            if (code == code1)
            {
                beginPoint.x = x;
                beginPoint.y = y;
                code1 = Encode(beginPoint, leftBottom, rightTop);
            }
            else
            {
                endPoint.x = x;
                endPoint.y = y;
                code2 = Encode(endPoint, leftBottom, rightTop);
            }
        }

        res.push_back(beginPoint);
        res.push_back(endPoint);

        return res;
    }

    std::vector<glm::vec2> ClippingAlgorithm::MidPoint(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop)
    {
        std::vector<glm::vec2> result;

        int code1 = Encode(beginPoint, leftBottom, rightTop);
        int code2 = Encode(endPoint, leftBottom, rightTop);
        int code = 0, x = 0, y = 0;
        float epsilon = 0.1f;

        if (code1 == 0 && code2 == 0)
        {
            result.push_back(beginPoint);
            result.push_back(endPoint);
            return result;
        }

        if ((code1 & code2) != 0)
        {
            result.push_back(glm::vec2{ -1, -1 });
            result.push_back(glm::vec2{ -1, -1 });
            return result;
        }

        if (code1 != 0)
        {
            glm::vec2 begin = beginPoint;
            glm::vec2 end = endPoint;

            while (glm::distance(begin, end) >= epsilon)
            {
                glm::vec2 mid = (begin + end) / 2.0f;
                code = Encode(mid, leftBottom, rightTop);
                if ((code & code1) == 0)
                {
                    end = mid;
                }
                else
                {
                    begin = mid;
                    code1 = code;
                }
            }
            beginPoint = begin;
        }

        if (code2 != 0)
        {
            glm::vec2 begin = beginPoint;
            glm::vec2 end = endPoint;

            while (glm::distance(begin, end) >= epsilon)
            {
                glm::vec2 mid = glm::vec2({ (begin[0] + end[0]) / 2, (begin[1] + end[1]) / 2 });
                code = Encode(mid, leftBottom, rightTop);
                if ((code & code2) == 0)
                    begin = mid;
                else
                {
                    end = mid;
                    code2 = code;
                }
            }
            endPoint = end;
        }

        result.push_back(beginPoint);
        result.push_back(endPoint);

        return result;
    }

    int ClippingAlgorithm::Encode(glm::vec2 point, const glm::vec2& leftBottom, const glm::vec2& rightTop)
    {
        int code = 0;

        if (point.x < leftBottom.x)
            code |= LEFT;
        else if (point.x > rightTop.x)
            code |= RIGHT;
        if (point.y < leftBottom.y)
            code |= BOTTOM;
        else if (point.y > rightTop.y)
            code |= TOP;

        return code;
    }

    // ********************************************************************************************************************************************************************
    // Polygon Clipping
    // ********************************************************************************************************************************************************************

    //  裁剪区域需要为顺时针
    std::vector<glm::vec2> ClippingAlgorithm::SutherlandHodgman(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clippingArea)
    {
        std::vector<glm::vec2> clippedVertices(vertices);
        for (int i = 0; i < clippingArea.size(); i++)
        {
            int j = (i + 1) % clippingArea.size();
            clippedVertices = Clip(clippedVertices, clippingArea[i], clippingArea[j]);
        }

        return clippedVertices;
    }

    std::vector<glm::vec2> ClippingAlgorithm::Clip(std::vector<glm::vec2> vertices, const glm::vec2& point1, const glm::vec2& point2)
    {
        std::vector<glm::vec2> clippedVertices;

        for (int i = 0; i < vertices.size(); i++)
        {
            int k = (i + 1) % vertices.size();  // index of the next vertex

            glm::vec2 pointI(vertices[i]);
            glm::vec2 pointK(vertices[k]);

            int posI = (point2.x - point1.x) * (pointI.y - point1.y) - (point2.y - point1.y) * (pointI.x - point1.x);
            int posK = (point2.x - point1.x) * (pointK.y - point1.y) - (point2.y - point1.y) * (pointK.x - point1.x);

            if (posI < 0 && posK < 0)
            {
                clippedVertices.push_back(pointK);
            }
            else if (posI >= 0 && posK < 0)
            {
                clippedVertices.push_back(Intersection(pointI, pointK, point1, point2));
                clippedVertices.push_back(pointK);
            }
            else if (posI < 0 && posK >= 0)
            {
                clippedVertices.push_back(Intersection(pointI, pointK, point1, point2));
            }
        }

        return clippedVertices;
    }

    glm::vec2 ClippingAlgorithm::Intersection(const glm::vec2& point1, const glm::vec2& point2, const glm::vec2& point3, const glm::vec2& point4)
    {
        glm::vec2 point;

        point.x = ((point1.x * point2.y - point1.y * point2.x) * (point3.x - point4.x) - (point1.x - point2.x) * (point3.x * point4.y - point3.y * point4.x))
            / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));

        point.y = ((point1.x * point2.y - point1.y * point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x * point4.y - point3.y * point4.x))
            / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));

        return point;
    }
}