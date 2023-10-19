#include "pch.h"
#include "CohenSutherland.h"

namespace Grafix
{
    std::vector<glm::vec2> CohenSutherland::Clip(glm::vec2 beginPoint, glm::vec2 endPoint
        , const glm::vec2& leftBottom, const glm::vec2& rightTop)
    {
        int code = 0, x = 0, y = 0;
        std::vector<glm::vec2> res;
        int code1 = Encode(beginPoint, leftBottom, rightTop);
        int code2 = Encode(endPoint, leftBottom, rightTop);

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

    int CohenSutherland::Encode(glm::vec2 point, const glm::vec2& leftBottom, const glm::vec2& rightTop)
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
}