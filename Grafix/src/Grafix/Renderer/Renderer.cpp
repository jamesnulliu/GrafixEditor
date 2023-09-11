#include "pch.h"
#include "Renderer.h"

#include <algorithm>

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

        // Background
        for (uint32_t y = 0; y < m_Image->GetHeight(); y++)
        {
            for (uint32_t x = 0; x < m_Image->GetWidth(); x++)
            {
                m_Pixels[y * m_Image->GetWidth() + x] = RGBAToUint32(glm::vec4(m_ActiveScene->GetBackgroundColor(), 1.0f));
            }
        }

        // Square
        for (auto& rect : m_ActiveScene->GetRectangles())
        {
            for (int y = std::max((int)rect.Position.y, 0);
                y < std::min((int)(rect.Position.y + rect.Height), (int)m_Image->GetHeight());
                ++y)
            {
                for (int x = std::max((int)rect.Position.x, 0);
                    x < std::min((int)(rect.Position.x + rect.Width), (int)m_Image->GetWidth());
                    ++x)
                {
                    m_Pixels[y * m_Image->GetWidth() + x] = RGBAToUint32(glm::vec4(rect.Color, 1.0f));
                }
            }
        }

        m_Image->SetPiexels(m_Pixels);
    }

    void Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
    {
        if (m_Image)
        {
            if (m_Image->GetWidth() == newWidth && m_Image->GetHeight() == newHeight)
            {
                // Skip
            } else
            {
                m_Image->Resize(newWidth, newHeight);
                GF_INFO("Image resized: {0}x{1}", m_Image->GetWidth(), m_Image->GetHeight());
            }
        } else
        {
            m_Image = std::make_shared<Grafix::Image>(newWidth, newHeight, Grafix::ImageFormat::RGBA);
        }

        delete[] m_Pixels;
        m_Pixels = new uint32_t[newWidth * newHeight];
    }
}