#pragma once

#include "GraphicsAlgorithm.h"

#include <list>

namespace Grafix
{
    struct Edge
    {
        int Ymax;
        int Ymin;
        float X, DeltaX;

        bool operator<(const Edge& edge) const
        {
            if (X == edge.X)
                return DeltaX < edge.DeltaX;

            return X < edge.X;
        }
    };

    class PolygonAlgorithm : public GraphicsAlgorithm
    {
    public:
        static void Draw(const std::vector<glm::vec3>& vertices, const glm::vec4& color, bool closed);
    };
}
