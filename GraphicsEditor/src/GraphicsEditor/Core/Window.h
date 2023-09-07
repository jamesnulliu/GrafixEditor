#pragma once

#include <string>
#include <memory>

namespace GE
{
    class Window
    {
    public:
        virtual ~Window() = default;

        virtual const std::string& GetTitle() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void OnUpdate() = 0;

        virtual void* GetNativeWindow() const = 0;

        ////void SetEventCallback(std::function<void()>()) {}

        static std::unique_ptr<Window> Create(const std::string& title, uint32_t width, uint32_t height);
    protected:
        struct WindowData
        {
            std::string Title;
            uint32_t Width;
            uint32_t Height;
        };
    };
}
