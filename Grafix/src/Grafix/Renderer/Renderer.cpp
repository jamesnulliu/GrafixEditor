#include "pch.h"
#include "Renderer.h"

#include "Grafix/Utils/ColorConvert.hpp"

#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"
#include "Algorithms/ArcAlgorithm.h"
#include "Algorithms/PolygonAlgorithm.h"

namespace Grafix
{
    void Renderer::Render(Scene& scene, EditorCamera& camera)
    {
        GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_Image->GetWidth(), m_Image->GetHeight());

        // Clear color
        std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), RGBToUint32(m_ClearColor));

        // Entities
        for (Entity entity : scene.GetEntities())
        {
            if (entity.HasComponent<LineComponent>())
                DrawLine(entity);
            else if (entity.HasComponent<CircleComponent>())
                DrawCircle(entity);
            else if (entity.HasComponent<ArcComponent>())
                DrawArc(entity);
            else if (entity.HasComponent<PolygonComponent>())
                DrawPolygon(entity);
        }

        m_Image->SetPiexels(m_Pixels);
    }

    void Renderer::DrawLine(Entity entity)
    {
        auto& line = entity.GetComponent<LineComponent>();

        if (entity.HasComponent<TransformComponent>())
        {
            auto transform = entity.GetComponent<TransformComponent>().GetTransformMatrix();
            glm::vec2 p0 = transform * glm::vec4(line.P0, 1.0f);
            glm::vec2 p1 = transform * glm::vec4(line.P1, 1.0f);

            LineAlgorithm::Draw(p0, p1, line.Color, line.Style, line.DashLength);
        } else
        {
            LineAlgorithm::Draw(line.P0, line.P1, line.Color, line.Style, line.DashLength);
        }
    }

    void Renderer::DrawCircle(Entity entity)
    {
        auto& circle = entity.GetComponent<CircleComponent>();

        if (entity.HasComponent<TransformComponent>())
        {
            auto& transform = entity.GetComponent<TransformComponent>();
            auto transformMatrix = entity.GetComponent<TransformComponent>().GetTransformMatrix();

            glm::vec2 center = transformMatrix * glm::vec4(circle.Center, 1.0f);
            float radius = transform.Scale.x * circle.Radius;

            CircleAlgorithm::Draw(center, radius, circle.Color, circle.ShowCenter);
        } else
        {
            CircleAlgorithm::Draw(circle.Center, circle.Radius, circle.Color, circle.ShowCenter);
        }
    }

    void Renderer::DrawArc(Entity entity)
    {
        auto& arc = entity.GetComponent<ArcComponent>();

        // Not correct
        if (entity.HasComponent<TransformComponent>())
        {
            auto& transform = entity.GetComponent<TransformComponent>();
            auto transformMatrix = entity.GetComponent<TransformComponent>().GetTransformMatrix();

            glm::vec2 center = transformMatrix * glm::vec4(arc.Center, 1.0f);
            float radius = transform.Scale.x * arc.Radius;

            ArcAlgorithm::Draw(center, radius, arc.Angle1, arc.Angle2, arc.Major, arc.Color, arc.ShowCenter, arc.ShowRadius);
        } else
        {
            ArcAlgorithm::Draw(arc.Center, arc.Radius, arc.Angle1, arc.Angle2, arc.Major, arc.Color, arc.ShowCenter, arc.ShowRadius);
        }
    }

    void Renderer::DrawPolygon(Entity entity)
    {
        auto& polygon = entity.GetComponent<PolygonComponent>();

        if (polygon.Vertices.size() < 2) return;

        if (!polygon.IsClosed)
        {
            // Draw lines between vertices
            for (int i = 0; i < polygon.Vertices.size() - 1; i++)
                LineAlgorithm::Draw(polygon.Vertices[i], polygon.Vertices[i + 1], polygon.Color, LineStyle::Solid, 0);
        } else
        {
            std::vector<glm::vec2> vertices(polygon.Vertices.size());

            if (entity.HasComponent<TransformComponent>())
            {
                auto transform = entity.GetComponent<TransformComponent>().GetTransformMatrix();

                for (int i = 0; i < polygon.Vertices.size(); i++)
                    vertices[i] = glm::vec2(transform * glm::vec4(polygon.Vertices[i], 1.0f));
            } else
            {
                for (int i = 0; i < polygon.Vertices.size(); i++)
                    vertices[i] = glm::vec2(polygon.Vertices[i]);
            }

            PolygonAlgorithm::Draw(vertices, polygon.Color);
        }
    }

    void Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
    {
        if (m_Image)
        {
            if (m_Image->GetWidth() == newWidth && m_Image->GetHeight() == newHeight)
                return;

            m_Image->Resize(newWidth, newHeight);
            GF_INFO("Image resized: {0}x{1}", m_Image->GetWidth(), m_Image->GetHeight());
        } else
        {
            m_Image = std::make_shared<Image>(newWidth, newHeight, ImageFormat::RGBA);
            GF_INFO("Image created: {0}x{1}", m_Image->GetWidth(), m_Image->GetHeight());
        }

        delete[] m_Pixels;
        m_Pixels = new uint32_t[newWidth * newHeight];
    }
}