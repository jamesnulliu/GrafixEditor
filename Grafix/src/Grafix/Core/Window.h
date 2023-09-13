#pragma once

#include <string>
#include <vector>

struct GLFWwindow;

namespace Grafix
{
    class Window
    {
    public:
        Window(const std::string& title, uint32_t width, uint32_t height);
        ~Window();

        void OnUpdate();

        const std::string& GetTitle() const { return m_Data.Title; }
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }

        bool ShouldClose() const;

        GLFWwindow* GetHandle() const { return m_Handle; }

        ////void SetEventCallback(std::function<void()>()) {}
    protected:
        GLFWwindow* m_Handle = nullptr;

        struct WindowData
        {
            std::string Title;
            uint32_t Width;
            uint32_t Height;
        };

        WindowData m_Data{};
    };
}
