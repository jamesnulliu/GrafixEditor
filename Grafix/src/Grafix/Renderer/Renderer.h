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

        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, float lineWidth = 1.0f,
            LineStyleType lineStyle = LineStyleType::Solid, LineAlgorithmType algorithm = LineAlgorithmType::Bresenham);
        void DrawLine(const TransformComponent& transform, const glm::vec2& p0, const glm::vec2& p1,
            const glm::vec3& color, float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, LineAlgorithmType algorithm = LineAlgorithmType::Bresenham);

        void DrawCircle(const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid);
        void DrawCircle(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid);

        void DrawArc(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid);
        void DrawArc(const TransformComponent& transform, const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid);

        void DrawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color);
        void DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color);

        void DrawCurve(const std::vector<glm::vec2>& controlPoints, const glm::vec3& color,
            int order, float step, std::vector<float>& knots, std::vector<float>& weights,
            float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, CurveAlgorithmType algorithm = CurveAlgorithmType::Bezier);
        void DrawCurve(const TransformComponent& transform, const std::vector<glm::vec2>& controlPoints, const glm::vec3& color,
            int order, float step, std::vector<float>& knots, std::vector<float>& weights,
            float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, CurveAlgorithmType algorithm = CurveAlgorithmType::Bezier);

        // *************************** Not Reliant On Camera *************************** //

        void DrawCross(const glm::vec2& center, float radius, const glm::vec3& color);
        void DrawCross(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color);

        void DrawSquare(const glm::vec2& center, float length, const glm::vec3& color);

        void Fill(const glm::vec2& seedPoint, const glm::vec3& fillColor);

        // NEW
        void SetClipRange(const glm::vec2 p0, const glm::vec2 p1);
    private:
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;
        glm::vec2 m_ClipP0 = { 0.0f, 0.0f };
        glm::vec2 m_ClipP1 = { 1280, 720 };
        glm::vec3 m_ClearColor = { 0.158f, 0.191f, 0.214f };
    };
}
