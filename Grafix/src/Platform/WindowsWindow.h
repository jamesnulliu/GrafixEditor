#pragma once

#include "Grafix/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Grafix
{
    // GLFW Window
    class WindowsWindow final : public Window
    {
    public:
        WindowsWindow(const std::string& title, uint32_t width, uint32_t height);
        virtual ~WindowsWindow();

        virtual const std::string& GetTitle() const override { return m_Data.Title; }
        virtual uint32_t GetWidth() const override { return m_Data.Width; }
        virtual uint32_t GetHeight() const override { return m_Data.Height; }

        virtual void* GetNativeWindow() const override { return m_NativeWindow; }

        virtual void OnUpdate() override {}
    private:
        GLFWwindow* m_NativeWindow = nullptr;
        WindowData m_Data{};
    };
}
