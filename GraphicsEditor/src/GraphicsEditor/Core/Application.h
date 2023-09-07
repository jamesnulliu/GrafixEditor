#pragma once

#include "Window.h"
#include "GraphicsEditor/ImGui/ImGuiLayer.h"

#include <vector>
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

        void PushLayer(Layer* layer);
        void Close() { m_IsRunning = false; }

        inline Window& GetWindow() { return *m_Window; }
        inline static Application& Get() { return *s_AppInstance; }

    private:
        void Init();
        void Shutdown();
    private:
        AppSpecification m_AppSpec;

        std::unique_ptr<Window> m_Window;

        std::vector<Layer*> m_LayerStack;
        ImGuiLayer* m_ImGuiLayer;

        bool m_IsRunning = true;

        static Application* s_AppInstance;
    };

    // Define the function in client
    Application* CreateApplication();
}
