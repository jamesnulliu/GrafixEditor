#include "pch.h"
#include "Renderer.h"

#include "Grafix/Utils/ColorConvert.hpp"
#include "Grafix/Math/Transformation.h"

#include "Algorithms/GraphicsAlgorithm.h"
#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"
#include "Algorithms/ArcAlgorithm.h"
#include "Algorithms/PolygonAlgorithm.h"
#include "Algorithms/SeedFillAlgorithm.h"
#include "Algorithms/CurveAlgorithm.h"

namespace Grafix
{
    static glm::mat3 s_ViewMatrix = glm::mat3(1.0f);

    void Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
    {
        if (m_Image)
        {
            if (m_Image->GetWidth() == newWidth && m_Image->GetHeight() == newHeight)
                return;
            m_Image->Resize(newWidth, newHeight);
        }
        else
        {
            m_Image = std::make_shared<Image>(newWidth, newHeight, ImageFormat::RGBA);
        }

        delete[] m_Pixels;
        m_Pixels = new uint32_t[newWidth * newHeight];
    }

    void Renderer::BeginScene(const Camera& camera)
    {
        s_ViewMatrix = camera.GetViewMatrix();
        GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_Image->GetWidth(), m_Image->GetHeight());

        // Clear color
        std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), RGBToUint32(m_ClearColor));
    }

    void Renderer::EndScene()
    {
        m_Image->SetPiexels(m_Pixels);
    }

    void Renderer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth, LineStyleType style, LineAlgorithmType algorithm)
    {
        DrawLine(TransformComponent(), p0, p1, color, lineWidth, style);
    }

    void Renderer::DrawLine(const TransformComponent& transform, const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth, LineStyleType style, LineAlgorithmType algorithm)
    {
        switch (algorithm)
        {
        case LineAlgorithmType::Midpoint:
        {
            LineAlgorithm::Midpoint(
                Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p0)),
                Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p1)),
                color, lineWidth, style
            );
            break;
        }
        case LineAlgorithmType::Bresenham:
        {
            LineAlgorithm::Bresenham(
                Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p0)),
                Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p1)),
                color, lineWidth, style
            );
            break;
        }
        }
    }

    void Renderer::DrawCircle(const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        DrawCircle(TransformComponent(), center, radius, color, lineWidth, lineStyle);
    }

    void Renderer::DrawCircle(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        CircleAlgorithm::Midpoint(
            Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center)),
            radius, color, lineWidth, lineStyle
        );
    }

    void Renderer::DrawArc(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        DrawArc(TransformComponent(), center, radius, angle1, angle2, major, color, lineWidth, lineStyle);
    }

    // Wrong
    void Renderer::DrawArc(const TransformComponent& transform, const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth, LineStyleType lineStyle)
    {
        ArcAlgorithm::Midpoint(
            Math::Transform(transform.GetTransformMatrix(), center),
            radius, angle1, angle2, major, color, lineWidth);
    }

    void Renderer::DrawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        DrawPolygon(TransformComponent(), vertices, color);
    }

    void Renderer::DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        if (vertices.size() == 0 || vertices[0] != vertices[vertices.size() - 1])
        {
        }
        else
        {
            std::vector<glm::vec2> transformedVertices(vertices);

            for (auto& vertex : transformedVertices)
                vertex = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), vertex));

            PolygonAlgorithm::Scanline(transformedVertices, color);
        }
    }

    void Renderer::DrawCurve(const std::vector<glm::vec2>& controlPoints, const glm::vec3& color, int order, float step,
        std::vector<float>& knots, std::vector<float>& weights, float lineWidth, CurveAlgorithmType algorithm)
    {
        DrawCurve(TransformComponent(), controlPoints, color, order, step, knots, weights, lineWidth, algorithm);
    }

    void Renderer::DrawCurve(const TransformComponent& transform, const std::vector<glm::vec2>& controlPoints,
        const glm::vec3& color, int order, float step, std::vector<float>& knots,
        std::vector<float>& weights, float lineWidth, CurveAlgorithmType algorithm)
    {
        std::vector<glm::vec2> transformedControlPoints(controlPoints);
        for (auto& controlPoint : transformedControlPoints)
            controlPoint = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), controlPoint));

        switch (algorithm)
        {
        case CurveAlgorithmType::Bezier:
        {
            CurveAlgorithm::Bezier(transformedControlPoints, step, color, lineWidth);
            break;
        }
        case CurveAlgorithmType::NURBS:
        {
            if (controlPoints.size() >= order)
                CurveAlgorithm::NURBS(transformedControlPoints, order, step, color, knots, weights, lineWidth);
            break;
        }
        }
    }

    void Renderer::DrawCross(const glm::vec2& center, float radius, const glm::vec3& color)
    {
        DrawCross(TransformComponent(), center, radius, color);
    }

    void Renderer::DrawCross(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color)
    {
        glm::vec2 transformedCenter = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center));

        LineAlgorithm::Bresenham(
            { transformedCenter.x - radius, transformedCenter.y },
            { transformedCenter.x + radius, transformedCenter.y },
            color, 1
        );

        LineAlgorithm::Bresenham(
            { transformedCenter.x, transformedCenter.y - radius },
            { transformedCenter.x, transformedCenter.y + radius },
            color, 1
        );
    }

    void Renderer::DrawSquare(const glm::vec2& center, float length, const glm::vec3& color)
    {
        GraphicsAlgorithm::DrawSquare(center, length, color);
    }

    void Renderer::Fill(const glm::vec2& seedPoint, const glm::vec3& fillColor)
    {
        uint32_t oldColor = m_Pixels[(uint32_t)seedPoint.x + (uint32_t)seedPoint.y * m_Image->GetWidth()];
        SeedFillAlgorithm::Fill(seedPoint, RGBToUint32(fillColor), oldColor);
    }
}