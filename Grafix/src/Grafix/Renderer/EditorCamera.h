#pragma once

#include "Camera.h"

#include "Grafix/Input/Input.h"

#include <glm/glm.hpp>

namespace Grafix
{
    class EditorCamera final : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float aspectRatio, float orthographicSize);
        virtual ~EditorCamera() = default;

        void OnUpdate();

        void SetViewportSize(float width, float height);

        glm::vec3& GetPosition() { return m_Position; }
        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position);

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    private:
        void UpdateMatrix();
    private:
        float m_AspectRatio = 16.0f / 9.0f;
        float m_OrthographicSize = 10.0f;

        glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
        float m_MoveSpeed = 1.0f;

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };
    };
}
