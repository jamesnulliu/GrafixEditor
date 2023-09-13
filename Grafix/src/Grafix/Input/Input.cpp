#include "pch.h"
#include "Input.h"

#include "Grafix/Core/Application.h"

namespace Grafix
{
    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();

        int state = glfwGetMouseButton(windowHandle, (int)button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition()
    {
        GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();

        double xPos, yPos;
        glfwGetCursorPos(windowHandle, &xPos, &yPos);

        return glm::vec2{ (float)xPos, (float)yPos };
    }

    void Input::SetCursorMode(CursorMode mode)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();

        glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
    }

    bool Input::IsKeyPressed(Key keycode)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();

        int state = glfwGetKey(windowHandle, (int)keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
}