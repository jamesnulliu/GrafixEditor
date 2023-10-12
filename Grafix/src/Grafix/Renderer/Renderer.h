#pragma once

#include "Image.h"

#include "Grafix/Scene/Scene.h"
#include "Grafix/Scene/Entity.h"

#include "Camera.h"
#include "Algorithms/LineAlgorithm.h"

#include <memory>

namespace Grafix
{
    class Renderer final
    {
    public:
        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);

        void BeginScene(const Camera& camera);
        void EndScene();

        void SetClearColor(const glm::vec3& color) { m_ClearColor = color; }

        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style = LineStyle::Solid, float dashLength = 5.0f);
        void DrawLine(const TransformComponent& transform, const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style = LineStyle::Solid, float dashLength = 5.0f);

        void DrawCircle(const glm::vec2& center, float radius, const glm::vec3& color);
        void DrawCircle(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color);

        void DrawArc(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color);
        void DrawArc(const TransformComponent& transform, const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color);

        void DrawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color);
        void DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color);

        void DrawCross(const glm::vec2& center, float radius, const glm::vec3& color, LineStyle lineStyle = LineStyle::Solid, float dashLength = 5.0f);
        void DrawCross(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color, LineStyle lineStyle = LineStyle::Solid, float dashLength = 5.0f);

        void Fill(const glm::vec2& seedPoint, const glm::vec3& fillColor);
        
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;

        glm::vec3 m_ClearColor = { 0.158f, 0.191f, 0.214f };
    };
}
