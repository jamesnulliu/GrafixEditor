#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    struct Edge
    {
        int YMax = 0;
        float X = 0.0f;
        float DeltaX = 0.0f;

        Edge() = delete;
        Edge(const Edge&) = default;
        Edge(Edge&&) noexcept = default;
        Edge& operator=(const Edge&) = default;

        Edge(int yMax, float x, float deltaX)
            : YMax(yMax), X(x), DeltaX(deltaX)
        {
        }

        bool operator<(const Edge& edge) const
        {
            if (X == edge.X)
                return DeltaX < edge.DeltaX;

            return X < edge.X;
        }
    };

    class PolygonAlgorithm : public GraphicsAlgorithm
    {
        using EdgeTable = std::vector<std::vector<Edge>>;
        using ActiveEdgeTable = std::vector<Edge>;
    public:
        static void Draw(const std::vector<glm::vec2>& vertices, const glm::vec3& color);
    private:
        static void Scanline(const std::vector<glm::vec2>& vertices, const glm::vec3& color);
    };
}
