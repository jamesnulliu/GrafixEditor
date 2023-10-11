#include "pch.h"
#include "PolygonAlgorithm.h"

#include "LineAlgorithm.h"

namespace Grafix
{
    void PolygonAlgorithm::Draw(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        Scanline(vertices, color);
    }

    // The last vertex is the same as the first one
    void PolygonAlgorithm::Scanline(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        int yMax = (int)(vertices[0].y + 0.5);
        int yMin = (int)(vertices[0].y + 0.5);

        for (int i = 1; i < vertices.size() - 1; ++i)
        {
            int roundedY = (int)(vertices[i].y + 0.5);

            if (roundedY > yMax)
                yMax = roundedY;

            if (roundedY < yMin)
                yMin = roundedY;
        }

        // Initialize ET
        EdgeTable ET(yMax + 1);

        for (int i = 0; i < vertices.size() - 1; ++i)
        {
            const glm::vec2* bottomVertex = &vertices[i];
            const glm::vec2* topVertex = &vertices[i + 1];

            int bottomY = (int)(bottomVertex->y + 0.5), topY = (int)(topVertex->y + 0.5);
            if (bottomY == topY)
            {
                continue;
            } else if (bottomY > topY)
            {
                std::swap(bottomVertex, topVertex);
                std::swap(bottomY, topY);
            }

            float deltaX = (topVertex->x - bottomVertex->x) / (topVertex->y - bottomVertex->y);

            Edge edge(topY, bottomVertex->x, deltaX);
            ET[bottomY].push_back(std::move(edge));

            std::sort(ET[bottomY].begin(), ET[bottomY].end());
        }

        // Create AEL
        ActiveEdgeTable AEL{};

        for (int y = yMin; y < yMax; ++y)
        {
            // If there are new edges, add them to AEL
            if (!ET[y].empty())
            {
                for (Edge& edge : ET[y])
                    AEL.push_back(edge);
            }

            std::sort(AEL.begin(), AEL.end());

            // If there are edges that end at y, remove them from AEL
            for (int i = 0; i < AEL.size(); ++i)
            {
                if (y == AEL[i].YMax)
                {
                    AEL.erase(AEL.begin() + i);
                    --i;
                }
            }

            // Draw the scanline
            for (int i = 0; i < AEL.size(); i += 2)
            {
                int x1 = (int)(AEL[i].X + 0.5), x2 = (int)(AEL[i + 1].X + 0.5);
                for (int x = x1; x < x2; ++x)
                    SetPixel(x, y, color);
            }

            for (Edge& edge : AEL)
                edge.X += edge.DeltaX;
        }
    }
}