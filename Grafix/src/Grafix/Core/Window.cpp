#include "pch.h"
#include "Window.h"

#include "Application.h"

#include "Grafix/Events/ApplicationEvent.h"
#include "Grafix/Events/KeyEvent.h"
#include "Grafix/Events/MouseEvent.h"

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

        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            m_Handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            s_WindowCreated = true;
        }

        glfwSetWindowUserPointer(m_Handle, &m_Data);

        // Set GLFW callbacks

        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent e(width, height);
            data.EventCallback(e);
            }
        );

        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent e;
            data.EventCallback(e);
            }
        );

        glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            Key key = (Key)keycode;

            switch (action)
            {
            case GLFW_PRESS:
            {
                KeyPressedEvent e(key, 0);
                data.EventCallback(e);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent e(key);
                data.EventCallback(e);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent e(key, true);
                data.EventCallback(e);
                break;
            }
            }
            }
        );

        glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent e((Key)keycode);
            data.EventCallback(e);
            }
        );

        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int buttonCode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseButton button = (MouseButton)buttonCode;

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent e(button);
                data.EventCallback(e);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent e(button);
                data.EventCallback(e);
                break;
            }
            }
            }
        );

        glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent e((float)xOffset, (float)yOffset);
            data.EventCallback(e);
            }
        );

        glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent e((float)xPos, (float)yPos);
            data.EventCallback(e);
            }
        );
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