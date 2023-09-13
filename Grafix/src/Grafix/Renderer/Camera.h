#pragma once

#include <glm/glm.hpp>

namespace Grafix
{
    class Camera
    {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        void OnUpdate();

    protected:
        glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

        float m_MoveSpeed = 10.0f;
    };
}
