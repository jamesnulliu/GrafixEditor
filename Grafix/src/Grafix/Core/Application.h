#pragma once

#include <vulkan/vulkan.h>

#include "Window.h"
#include "Grafix/ImGui/ImGuiLayer.h"

#include "Grafix/Utils/Stopwatch.h"

#include <string>
#include <memory>
#include <functional>

namespace Grafix
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
        void Close() { m_Running = false; }

        inline static Application& Get() { return *s_AppInstance; }

        inline bool IsMinimized() { return m_Minimized; }
        inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

        inline float GetFPS() const { return m_FPS; }

        // Window
        inline Window& GetWindow() { return *m_Window; }
        inline const Window& GetWindow() const { return *m_Window; }

        // Vulkan
        static VkInstance GetInstance();
        static VkPhysicalDevice GetPhysicalDevice();
        static VkDevice GetDevice();
        ////static uint32_t GetQueueFamily();
        ////static VkQueue GetQueue();
        ////static VkSurfaceKHR GetSurface();
        ////static VkDescriptorPool GetDescriptorPool();
        ////static VkPipelineCache GetPipelineCache();
        ////static VkDebugReportCallbackEXT GetDebugReport();
        ////static ImGui_ImplVulkanH_Window& GetWindowData();
        ////static uint32_t GetMinImageCount();

        static VkCommandBuffer GetCommandBuffer();
        static void FlushCommandBuffer(VkCommandBuffer commandBuffer);
        static void SubmitResourceFree(std::function<void()>&& func);

    private:
        // Vulkan
        void InitVulkan();
        void CleanupVulkan();

        void CreateVkInstance();
        void SelectPhysicalDevice();
        void SelectGraphicsQueueFamily();
        void CreateLogicalDevice();
        void CreateWindowSurface();
        void CreateDescriptorPool();
        void CreateFramebuffers();
    private:
        bool m_Running = true;
        bool m_Minimized = false;

        static Application* s_AppInstance;

        std::unique_ptr<Window> m_Window;

        std::vector<Layer*> m_LayerStack;
        ImGuiLayer* m_ImGuiLayer;

        Stopwatch m_Stopwatch;
        float m_FPS;
    };

    // Define the function in your own application.
    Application* CreateApplication();
}
