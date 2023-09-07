#include "pch.h"
#include "Window.h"
#include "Platform/WindowsWindow.h"

namespace Grafix
{
    std::unique_ptr<Window> Window::Create(const std::string& title, uint32_t width, uint32_t height)
    {
#ifdef GF_WINDOWS
        return std::make_unique<WindowsWindow>(title, width, height);
#else // GF_WINDOWS
#    error "The platform is currently not supported!"
#endif // GF_WINDOWS
    }
}