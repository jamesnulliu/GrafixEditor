#pragma once

#include "Camera.h"

#include "Grafix/Input/Input.h"

#include <glm/glm.hpp>

namespace Grafix
{
    class Camera final  // Support translation only for now
    {
    public:
        Camera() = default;
        Camera(float aspectRatio, float orthographicSize);
        virtual ~Camera() = default;

        void OnUpdate();

        void SetViewportSize(float width, float height);

        glm::vec2& GetPosition() { return m_Position; }
        const glm::vec2& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec2& position);

        glm::mat3 GetTranslationMatrix() const;
        const glm::mat3& GetViewMatrix() const { return m_ViewMatrix; }
    private:
        void UpdateViewMatrix();
    private:
        float m_AspectRatio = 16.0f / 9.0f;
        float m_OrthographicSize = 720.0f;

        glm::vec2 m_Position{ 0.0f, 0.0f };
        float m_MoveSpeed = 1.0f;

        glm::mat3 m_ViewMatrix;

        glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };
    };
}
