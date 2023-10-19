#include "pch.h"

#include "PolygonAlgorithm.h"
#include "ClippingAlgorithm.h"

namespace Grafix
{
    void PolygonAlgorithm::Scanline(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        std::vector<glm::vec2> clippedVertices = CalculateClippedVertices(vertices);
        if (clippedVertices.size() <= 2)
            return;

        uint32_t colorValue = RGBToUint32(color);

        int yMax = (int)(clippedVertices[0].y + 0.5);
        int yMin = (int)(clippedVertices[0].y + 0.5);

        for (int i = 1; i < clippedVertices.size(); ++i)
        {
            int roundedY = (int)(clippedVertices[i].y + 0.5);

            if (roundedY > yMax)
                yMax = roundedY;

            if (roundedY < yMin)
                yMin = roundedY;
        }

        // Initialize ET
        EdgeTable ET(yMax + 1);

        for (int i = 0; i < clippedVertices.size(); ++i)
        {
            const glm::vec2* bottomVertex = &clippedVertices[i];
            const glm::vec2* topVertex = (i < clippedVertices.size() - 1) ? &clippedVertices[i + 1] : &clippedVertices[0];

            int bottomY = (int)(bottomVertex->y + 0.5), topY = (int)(topVertex->y + 0.5);
            if (bottomY == topY)
            {
                continue;
            }
            else if (bottomY > topY)
            {
                std::swap(bottomVertex, topVertex);
                std::swap(bottomY, topY);
            }

            float deltaX = (topVertex->x - bottomVertex->x) / (topVertex->y - bottomVertex->y);

            ET[bottomY].push_back(Edge(topY, bottomVertex->x, deltaX));
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
                    SetSinglePixel(x, y, colorValue);
            }

            for (Edge& edge : AEL)
                edge.X += edge.DeltaX;
        }
    }

    std::vector<glm::vec2> PolygonAlgorithm::CalculateClippedVertices(const std::vector<glm::vec2>& vertices)
    {
        float canvasWidth = (float)GetWidth(), canvasHeight = (float)GetHeight();

        std::vector<glm::vec2> clippingArea{
            { 0.0f, 0.0f },
            { 0.0f , canvasHeight },
            { canvasWidth , canvasHeight },
            { canvasWidth , 0.0f}
        };

        return ClippingAlgorithm::SutherlandHodgman(vertices, clippingArea);
    }
}