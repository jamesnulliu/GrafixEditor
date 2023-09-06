#pragma once

#include "Window.h"

#include <string>

namespace GE
{
    struct AppSpecification
    {
        std::string Name;
        uint32_t Width;
        uint32_t Height;
    };

    class Application
    {
    public:
        Application(const AppSpecification& appSpec);
        virtual ~Application();

        void Run();
    private:
        AppSpecification m_AppSpec;
        std::unique_ptr<Window> m_Window;

        bool m_IsRunning = true;
    };

    // Define the function in client
    Application* CreateApplication();
}
