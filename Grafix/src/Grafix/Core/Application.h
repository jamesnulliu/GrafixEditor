#pragma once

#include <vulkan/vulkan.h>

#include "Window.h"
#include "Grafix/ImGui/ImGuiLayer.h"

#include <string>
#include <memory>
#include <functional>

struct ImGui_ImplVulkanH_Window;

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
        void Close() { m_IsRunning = false; }

        inline static Application& Get() { return *s_AppInstance; }

        // Window
        inline Window& GetWindow() { return *m_Window; }
        inline const Window& GetWindow() const { return *m_Window; }

        // Vulkan
        static VkInstance GetInstance();
        static VkPhysicalDevice GetPhysicalDevice();
        static VkDevice GetDevice();
        static VkCommandBuffer GetCommandBuffer();
        static void FlushCommandBuffer(VkCommandBuffer commandBuffer);
        static void SubmitResourceFree(std::function<void()>&& func);
        ////static uint32_t GetQueueFamily();
        ////static VkQueue GetQueue();
        ////static VkSurfaceKHR GetSurface();
        ////static VkDescriptorPool GetDescriptorPool();
        ////static VkAllocationCallbacks* GetAllocator();
        ////static VkPipelineCache GetPipelineCache();
        ////static VkDebugReportCallbackEXT GetDebugReport();
        ////static ImGui_ImplVulkanH_Window& GetWindowData();
        ////static uint32_t GetMinImageCount();

    private:
        void Init(const AppSpecification& appSpec);
        void Shutdown();

        // Vulkan
        void CreateVkInstance();
        void SelectPhysicalDevice();
        void SelectGraphicsQueueFamily();
        void CreateLogicalDevice();
        void CreateWindowSurface();
        void CreateDescriptorPool();
        void CreateFramebuffers();
    private:
        bool m_IsRunning = true;

        static Application* s_AppInstance;

        ////// Vulkan Data
        ////static VkInstance g_Instance;
        ////static VkPhysicalDevice g_PhysicalDevice;
        ////static VkDevice g_Device;
        ////static uint32_t g_QueueFamily;
        ////static VkQueue g_Queue;
        ////static VkSurfaceKHR g_Surface;
        ////static VkDescriptorPool g_DescriptorPool;
        ////static VkAllocationCallbacks* g_Allocator;
        ////static VkPipelineCache g_PipelineCache;
        ////static VkDebugReportCallbackEXT g_DebugReport;

        ////static ImGui_ImplVulkanH_Window g_WindowData;
        ////static uint32_t g_MinImageCount;
        ////static bool g_SwapChainRebuild = false;

        std::unique_ptr<Window> m_Window;

        std::vector<Layer*> m_LayerStack;
        ImGuiLayer* m_ImGuiLayer;
    };

    // Define the function in your own application.
    Application* CreateApplication();
}
