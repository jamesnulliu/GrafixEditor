#include "pch.h"
#include "Window.h"
#include "Platform/WindowsWindow.h"

namespace GE
{
    std::unique_ptr<Window> Window::Create(const std::string& title, uint32_t width, uint32_t height)
    {
#ifdef GE_WINDOWS
        return std::make_unique<WindowsWindow>(title, width, height);
#else // GE_WINDOWS
#    error "The platform is currently not supported!"
#endif // GE_WINDOWS
    }
}