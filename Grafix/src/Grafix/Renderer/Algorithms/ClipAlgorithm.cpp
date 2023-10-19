#include "pch.h"
#include "ClipAlgorithm.h"
#include"LineAlgorithm.h"
namespace Grafix
{
    int ClipAlgorithm::encode(double x, double y, double XL, double XR, double YB, double YT)
    {
        int c = 0;
        if (x < XL) c += 1;
        if (x > XR) c += 2;
        if (y < YB) c += 4;
        if (y > YT) c += 8;
        return c;
    }

    void ClipAlgorithm::CS_LineClip(const glm::vec2& p0,const glm::vec2& p1, glm::vec2 p2, glm::vec2 p3, const TransformComponent& transform, const glm::vec3& color, LineStyle style, float dashLength, const glm::mat3& s_ViewMatrix)
    {
        double x1 = p0.x;
        double x2 = p1.x;
        double y1 = p0.y;
        double y2 = p1.y;
        double XL = (p2.x < p3.x) ? p2.x : p3.x;
        double XR = (p3.x < p2.x) ? p2.x : p3.x;
        double YB = (p2.y < p3.y) ? p2.y : p3.y;
        double YT = (p3.y < p2.y) ? p2.y : p3.y;

        int code1, code2, code;
        code1 = encode(x1, y1, XL, XR, YB, YT);
        code2 = encode(x2, y2, XL, XR, YB, YT);
        double x, y;
        while (code1 != 0 || code2 != 0)
        {
            if ((code1 & code2) != 0)
                return;
            code = code1;
            if (code1 == 0)
            {
                code = code2;
            }
            if ((1 & code) != 0)
            {
                x = XL;
                y = y1 + (y2 - y1) * (XL - x1) / (x2 - x1);
            }
            else if ((2 & code) != 0)
            {
                x = XR;
                y = y1 + (y2 - y1) * (XR - x1) / (x2 - x1);
            }
            else if ((4 & code) != 0)
            {
                y = YB;
                x = x1 + (x2 - x1) * (YB - y1) / (y2 - y1);
            }
            else if ((8 & code) != 0)
            {
                y = YT;
                x = x1 + (x2 - x1) * (YT - y1) / (y2 - y1);
            }
            if (code == code1)
            {
                x1 = x; y1 = y; code1 = encode(x, y, XL, XR, YB, YT);
            }
            else
            {
                x2 = x; y2 = y; code2 = encode(x, y, XL, XR, YB, YT);
            }
        }
        glm::vec2 P0 = { x1,y1 };
        glm::vec2 P1 = { x2,y2 };

        LineAlgorithm::Draw(
            Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), P0)),
            Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), P1)),
            color, style, dashLength
        );
    }
}