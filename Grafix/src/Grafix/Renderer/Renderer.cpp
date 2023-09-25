#include "pch.h"
#include "Renderer.h"

#include "Algorithms/GraphicsAlgorithm.h"
#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"

static uint32_t RGBAToUint32(const glm::vec4& color)
{
    uint8_t r = (uint8_t)(color.r * 255.0f);
    uint8_t g = (uint8_t)(color.g * 255.0f);
    uint8_t b = (uint8_t)(color.b * 255.0f);
    uint8_t a = (uint8_t)(color.a * 255.0f);
    return (a << 24) | (b << 16) | (g << 8) | r;
}

namespace Grafix
{
    void Renderer::Render(Scene& scene, EditorCamera& camera)
    {
        m_ActiveScene = &scene;
        m_ActiveCamera = &camera;

        GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_Image->GetWidth(), m_Image->GetHeight());

        // Background
        uint32_t backgroundColor = RGBAToUint32(glm::vec4(m_ActiveScene->GetBackgroundColor(), 1.0f));
        std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), backgroundColor);

        // Entities
        for (auto& entity : m_ActiveScene->GetEntities())
        {
            if (auto line = entity.GetComponent<LineRendererComponent>())
                DrawLine(*line);

            if (auto circle = entity.GetComponent<CircleRendererComponent>())
                DrawCircle(*circle);
        }

        m_Image->SetPiexels(m_Pixels);
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

    ////void Renderer::DrawRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    ////{
    ////    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
    ////        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
    ////        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    ////    DrawRectangle(transform, color);
    ////}

    void Renderer::DrawLine(const LineRendererComponent& line)
    {
        glm::vec4 p0 = m_ActiveCamera->GetViewProjectionMatrix() * glm::vec4(line.P0, 1.0f);
        glm::vec4 p1 = m_ActiveCamera->GetViewProjectionMatrix() * glm::vec4(line.P1, 1.0f);

        LineAlgorithm::Draw(p0, p1, line.Color, line.Style, line.DashLength);
    }

    void Renderer::DrawCircle(const CircleRendererComponent& circle)
    {
        glm::vec4 center = m_ActiveCamera->GetViewProjectionMatrix() * glm::vec4(circle.Center, 1.0f);

        CircleAlgorithm::Draw(center, circle.Radius, circle.Color);
    }
}