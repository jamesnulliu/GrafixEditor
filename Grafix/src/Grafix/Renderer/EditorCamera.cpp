#include "pch.h"
#include "EditorCamera.h"

namespace Grafix
{
    EditorCamera::EditorCamera(float aspectRatio, float orthographicSize)
        : m_AspectRatio(aspectRatio), m_OrthographicSize(orthographicSize)
    {
    }

    void EditorCamera::OnUpdate()
    {
        if (Input::IsKeyPressed(Key::Space))
        {
            const glm::vec2& mousePos = Input::GetMousePosition();
            glm::vec2 delta = mousePos - m_LastMousePosition;
            m_LastMousePosition = mousePos;

            if (Input::IsMouseButtonPressed(MouseButton::Left))
            {
                // Move the camera
                m_Position.x -= delta.x;
                m_Position.y += delta.y;

                GF_INFO("Camera moved to: ({0}, {1}, {2})", m_Position.x, m_Position.y, m_Position.z);
                UpdateMatrix();
            }
        }
    }

    void EditorCamera::SetViewportSize(float width, float height)
    {
        m_AspectRatio = width / height;
        m_OrthographicSize = 1.0f;
        UpdateMatrix();
    }

    void EditorCamera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateMatrix();
    }

    void EditorCamera::UpdateMatrix()
    {
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;

        m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, -1.0f, 1.0f);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
        m_ViewMatrix = glm::inverse(transform);
        ////m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_ViewProjectionMatrix = m_ViewMatrix;
    }
}