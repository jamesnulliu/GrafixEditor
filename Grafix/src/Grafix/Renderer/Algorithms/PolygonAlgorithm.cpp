#include "pch.h"
#include "PolygonAlgorithm.h"
#include "LineAlgorithm.h"

#include <ranges>

namespace Grafix
{
    ////void PolygonAlgorithm::Draw(const std::vector<glm::vec3>& vertices, const glm::vec4& color, bool closed)
    ////{
    ////    if (vertices.size() < 2)
    ////        return;

    ////    if (!closed)
    ////    {
    ////        // Draw lines between vertices
    ////        for (int i = 0; i < vertices.size() - 1; i++)
    ////            LineAlgorithm::Draw(vertices[i], vertices[i + 1], color);

    ////        return;
    ////    }

    ////    int yMin = vertices[0].y, yMax = vertices[0].y;
    ////    for (int i = 1; i < vertices.size(); i++)
    ////    {
    ////        if (vertices[i].y < yMin)
    ////            yMin = vertices[i].y;

    ////        if (vertices[i].y > yMax)
    ////            yMax = vertices[i].y;
    ////    }

    ////    std::vector<std::list<Edge>> ET(yMax + 1);
    ////    std::list<Edge> AEL;

    ////    for (int i = 0; i < vertices.size(); i++)
    ////    {
    ////        Edge* ed = NULL;
    ////        if (i < x.size() - 1 && y[i] < y[i + 1])
    ////        {
    ////            ed = new Edge;
    ////            ed->ymax = y[i + 1];
    ////            ed->ymin = y[i];
    ////            ed->x = x[i] * 1.0;
    ////            ed->deltax = (x[i + 1] - x[i]) * 1.0 / (y[i + 1] - y[i]);
    ////            ed->nextEdge = NULL;
    ////        } else if (i < x.size() - 1 && y[i] > y[i + 1])
    ////        {
    ////            ed = new Edge;
    ////            ed->ymax = y[i];
    ////            ed->ymin = y[i + 1];
    ////            ed->x = x[i + 1] * 1.0;
    ////            ed->deltax = (x[i + 1] - x[i]) * 1.0 / (y[i + 1] - y[i]);
    ////            ed->nextEdge = NULL;
    ////        } else if (i == x.size() - 1)
    ////        {
    ////            if (y[i] < y[0])
    ////            {
    ////                ed = new Edge;
    ////                ed->ymax = y[0];
    ////                ed->ymin = y[i];
    ////                ed->x = x[i] * 1.0;
    ////                ed->deltax = (x[0] - x[i]) * 1.0 / (y[0] - y[i]);
    ////                ed->nextEdge = NULL;
    ////            } else if (y[0] < y[i])
    ////            {
    ////                ed = new Edge;
    ////                ed->ymax = y[i];
    ////                ed->ymin = y[0];
    ////                ed->x = x[0] * 1.0;
    ////                ed->deltax = (x[0] - x[i]) * 1.0 / (y[0] - y[i]);
    ////                ed->nextEdge = NULL;
    ////            }
    ////        }
    ////        if (ed == NULL)
    ////            continue;

    ////        if (ET[ed->ymin] == NULL)
    ////        {
    ////            ET[ed->ymin] = ed;
    ////        } else if (ET[ed->ymin]->x > ed->x || (ET[ed->ymin]->x == ed->x && ET[ed->ymin]->deltax < ed->deltax))
    ////        {
    ////            ed->nextEdge = ET[ed->ymin]->nextEdge;
    ////            ET[ed->ymin]->nextEdge = ed;
    ////        } else if (ET[ed->ymin]->nextEdge == NULL)
    ////        {
    ////            ed->nextEdge = ET[ed->ymin];
    ////            ET[ed->ymin] = ed;
    ////        } else
    ////        {
    ////            Edge* ep = ET[ed->ymin];
    ////            while (ep->nextEdge->nextEdge != NULL && (ep->nextEdge->nextEdge->x < ed->x || (ep->nextEdge->nextEdge->x == ed->x && ep->nextEdge->nextEdge->deltax > ed->deltax)))
    ////            {
    ////                ep = ep->nextEdge;
    ////            }
    ////            ed->nextEdge = ep->nextEdge;
    ////            ep->nextEdge = ed;
    ////        }
    ////    }
    ////    for (int i = Ymin; i < Ymax; i++)
    ////    {
    ////        if (ET[i] != NULL)
    ////        {
    ////            Edge* eq = ET[i];
    ////            while (eq != NULL)
    ////            {
    ////                AEL.push_back(eq);
    ////                eq = eq->nextEdge;
    ////            }
    ////            std::ranges::sort(AEL);
    ////        }
    ////        for (int k = 0; k < AEL.size(); k++)
    ////        {
    ////            if (AEL[k]->ymax == i)
    ////            {
    ////                AEL.erase(AEL.begin() + k);
    ////            }
    ////        }
    ////        int k = 0;

    ////        while (k < AEL.size())
    ////        {
    ////            Edge* m = AEL[k];
    ////            k++;
    ////            if (k >= AEL.size())
    ////                break;
    ////            Edge* n = AEL[k];
    ////            k++;
    ////            for (int j = (int)(m->x + 0.5); j < (int)(n->x + 0.5); j++)
    ////                SetPixel(i, j, color);
    ////        }

    ////        for (k = 0; k < AEL.size(); k++)
    ////            AEL[k].X += AEL[k].DeltaX;
    ////    }
    ////}
}