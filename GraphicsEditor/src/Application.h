#pragma once

namespace Graphics
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Run() const;
    private:
        // Window* m_Window;
    };

    // Define the function in client
    Application* CreateApplication();
}
