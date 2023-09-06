#include "pch.h"

#include "WindowsWindow.h"
#include "GraphicsEditor/Core/Window.h"

namespace GE
{
    static bool s_GLFWWindowCreated = false;

    static void glfw_error_callback(int error, const char* description)
    {
        GE_ERROR("Glfw Error {0}: {1}", error, description);
    }

    WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height)
    {
        m_Data.Title = title;
        m_Data.Width = width;
        m_Data.Height = height;

        if (s_GLFWWindowCreated == false)
        {
            int success = glfwInit();
            GE_ASSERT(success, "Could not initalize GLFW!");
            glfwSetErrorCallback(glfw_error_callback);
        }

#ifdef GE_DEBUG
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif // GE_DEBUG

        m_NativeWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        s_GLFWWindowCreated = true;
    }

    WindowsWindow::~WindowsWindow()
    {
        glfwDestroyWindow(m_NativeWindow);
        glfwTerminate();
        s_GLFWWindowCreated = false;
    }
}