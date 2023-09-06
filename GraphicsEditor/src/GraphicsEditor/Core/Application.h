#pragma once

#include <string>

struct GLFWwindow;

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
        bool m_IsRunning = true;
        GLFWwindow* m_WindowHandle = nullptr;
        AppSpecification m_Specification;
    };

    // Define the function in client
    Application* CreateApplication();
}
