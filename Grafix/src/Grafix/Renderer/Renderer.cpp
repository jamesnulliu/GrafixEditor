#include "pch.h"
#include "Renderer.h"

#include "Grafix/Entities/Algorithm.h"

#define Multithreading 1

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
    void Renderer::Render(Scene& scene)
    {
        m_ActiveScene = &scene;

        GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_Image->GetWidth(), m_Image->GetHeight());

        // Background
#if Multithreading == 1
        std::for_each(std::execution::par, m_VerticalIters.begin(), m_VerticalIters.end(), [this](uint32_t y) {
            std::for_each(std::execution::par, m_HorizontalIters.begin(), m_HorizontalIters.end(), [this, y](uint32_t x) {
                m_Pixels[y * m_Image->GetWidth() + x] = RGBAToUint32(m_ActiveScene->GetBackgroundColor());
                });
            });
#else // Multithreading
        for (uint32_t y = 0; y < m_Image->GetHeight(); y++)
            for (uint32_t x = 0; x < m_Image->GetWidth(); x++)
                m_Pixels[y * m_Image->GetWidth() + x] = RGBAToUint32(glm::vec4(m_ActiveScene->GetBackgroundColor(), 1.0f));
#endif // Multithreading

        for (Line& line : m_ActiveScene->GetLines())
            DrawLine(line.GetPoint0(), line.GetPoint1(), line.GetWidth(), line.GetSpriteRenderer().Color, line.GetStyle(), line.GetDashLength());

        for (Circle& circle : m_ActiveScene->GetCircles())
            DrawCircle(circle.GetCenter(), circle.GetRadius(), circle.GetSpriteRenderer().Color);

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

#if Multithreading == 1
        m_HorizontalIters.resize(newWidth);
        for (uint32_t i = 0; i < newWidth; i++)
            m_HorizontalIters[i] = i;

        m_VerticalIters.resize(newHeight);
        for (uint32_t i = 0; i < newHeight; i++)
            m_VerticalIters[i] = i;
#endif
    }

    void Renderer::DrawRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawRectangle(transform, color);
    }

    void Renderer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, float width, const glm::vec4& color, LineStyle style, uint32_t dashLength)
    {
        ////LineAlgorithm::Bresenham(p0, p1, RGBAToUint32(color), style, dashLength);
        LineAlgorithm::Midpoint(p0, p1, RGBAToUint32(color), style, dashLength);
    }

    void Renderer::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color)
    {
        ////LineAlgorithm::Bresenham(p0, p1, RGBAToUint32(color), style, dashLength);
        CircleAlgorithm::Midpoint(center, radius, RGBAToUint32(color));
    }
}