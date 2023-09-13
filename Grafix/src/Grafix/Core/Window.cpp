#include "pch.h"
#include "Window.h"

#include "Application.h"

#include <GLFW/glfw3.h>

void FramePresent();

namespace Grafix
{
    static bool s_WindowCreated = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        GF_ERROR("Glfw Error {0}: {1}", error, description);
    }

    Window::Window(const std::string& title, uint32_t width, uint32_t height)
    {
        m_Data.Title = title;
        m_Data.Width = width;
        m_Data.Height = height;

        if (s_WindowCreated == false)
        {
            int success = glfwInit();
            GF_ASSERT(success, "Could not initalize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        s_WindowCreated = true;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Handle);
        glfwTerminate();

        s_WindowCreated = false;
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();

        if (Application::Get().IsMinimized() == false)
            FramePresent();
    }

    bool Window::ShouldClose() const
    {
        return (bool)glfwWindowShouldClose(m_Handle);
    }
}