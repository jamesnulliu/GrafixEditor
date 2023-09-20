#pragma once

#include "Image.h"
#include "Grafix/Scene/Scene.h"

#include <memory>

#include "Grafix/Entities/Line.h"

namespace Grafix
{
    class Renderer
    {
    public:
        void Render(Scene& scene);

        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);

        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, float width, const glm::vec4& color, LineStyle style, uint32_t dashLength);
        void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color);

        void DrawRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        void DrawRectangle(const glm::mat4& transform, const glm::vec4& color) {}
    private:
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;

        Scene* m_ActiveScene = nullptr;

        std::vector<uint32_t> m_HorizontalIters;
        std::vector<uint32_t> m_VerticalIters;
    };
}
