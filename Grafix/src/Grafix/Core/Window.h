#pragma once

#include "Grafix/Events/Event.h"

struct GLFWwindow;

namespace Grafix
{
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window(const std::string& title, uint32_t width, uint32_t height);
        ~Window();

        void OnUpdate();

        const std::string& GetTitle() const { return m_Data.Title; }
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }

        bool ShouldClose() const;
        inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

        GLFWwindow* GetHandle() const { return m_Handle; }
    private:
        GLFWwindow* m_Handle = nullptr;

        struct WindowData
        {
            std::string Title;
            uint32_t Width;
            uint32_t Height;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data{};
    };
}
