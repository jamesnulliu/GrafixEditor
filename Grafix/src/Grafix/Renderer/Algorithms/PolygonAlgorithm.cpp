#include "pch.h"
#include "PolygonAlgorithm.h"
#include "SutherlandHodgman.h"

namespace Grafix
{
    void PolygonAlgorithm::Scanline(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        uint32_t colorValue = RGBToUint32(color);

        std::vector<glm::vec2> clip;
        uint32_t canvasWidth = GetWidth(), canvasHeight = GetHeight();

        clip.push_back({ 0, 0 });
        clip.push_back({ 0 , canvasHeight });
        clip.push_back({ canvasWidth , canvasHeight });
        clip.push_back({ canvasWidth , 0 });
        std::vector<glm::vec2> clippedVertices;
        clippedVertices = Grafix::SutherlandHodgman::SutherHodgClip(vertices, clip);

        if (clippedVertices.size() < 2 || clippedVertices.front() != clippedVertices.back())
            return;

        int yMax = (int)(clippedVertices[0].y + 0.5);
        int yMin = (int)(clippedVertices[0].y + 0.5);

        for (int i = 1; i < clippedVertices.size() - 1; ++i)
        {
            int roundedY = (int)(clippedVertices[i].y + 0.5);

            if (roundedY > yMax)
                yMax = roundedY;

            if (roundedY < yMin)
                yMin = roundedY;
        }

        // Initialize ET
        EdgeTable ET(yMax + 1);

        for (int i = 0; i < clippedVertices.size() - 1; ++i)
        {
            const glm::vec2* bottomVertex = &clippedVertices[i];
            const glm::vec2* topVertex = &clippedVertices[i + 1];

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
                    SetSinglePixel(x, y, colorValue);
            }

            for (Edge& edge : AEL)
                edge.X += edge.DeltaX;
        }
    }
}