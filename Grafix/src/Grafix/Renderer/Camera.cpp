#include "pch.h"
#include "Camera.h"

#include "Grafix/Math/Transformation.h"

namespace Grafix
{
    Camera::Camera(float aspectRatio, float orthographicSize)
        : m_AspectRatio(aspectRatio), m_OrthographicSize(orthographicSize)
    {
    }

    void Camera::OnUpdate()
    {
    }

    void Camera::SetViewportSize(float width, float height)
    {
        m_AspectRatio = width / height;
        m_OrthographicSize = 1.0f;
        UpdateViewMatrix();
    }

    void Camera::SetPosition(const glm::vec2& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    glm::mat3 Camera::GetTranslationMatrix() const
    {
        return Math::CalcTranslationMatrix(m_Position);
    }

    // Wrong
    void Camera::UpdateViewMatrix()
    {
        glm::vec2 offset = glm::vec2(-0.5f * m_OrthographicSize * m_AspectRatio, -0.5 * m_OrthographicSize);

        glm::mat3 transform = /*Math::CalcTranslationMatrix(offset) * */Math::CalcTranslationMatrix(m_Position);
        m_ViewMatrix = glm::inverse(transform);
    }
}