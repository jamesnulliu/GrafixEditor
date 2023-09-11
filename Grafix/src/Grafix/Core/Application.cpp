#include "pch.h"

#include "Application.h"

// This code is adapted from ImGui/examples/example_glfw_vulkan/main.cpp and Walnut by The Cherno.

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Vulkan Data
static VkInstance g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice g_Device = VK_NULL_HANDLE;
static uint32_t g_QueueFamily = (uint32_t)-1;
static VkQueue g_Queue = VK_NULL_HANDLE;
static VkSurfaceKHR g_Surface = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
////static VkAllocationCallbacks* g_Allocator = nullptr;
static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport{};

static ImGui_ImplVulkanH_Window g_WindowData{};
static uint32_t g_MinImageCount = 2;
static bool g_SwapChainRebuild = false;

// Per-frame-in-flight
static std::vector<std::vector<VkCommandBuffer>> s_CommandBuffers;
static std::vector<std::vector<std::function<void()>>> s_ResourceFreeQueue;

// Unlike g_MainWindowData.FrameIndex, this is not the the swapchain image index
// and is always guaranteed to increase (eg. 0, 1, 2, 0, 1, 2)
static uint32_t s_CurrentFrameIndex = 0;

void CheckVkResult(VkResult result)
{
    GF_ASSERT(result == 0, "Vulkan error! VkResult = {0}", result);
}

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef GF_DEBUG
#    define VULKAN_DEBUG_REPORT
#endif

#ifdef VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    GF_TRACE("Vulkan debug report from ObjectType: {0}, message: {1}", objectType, pMessage);
    return VK_FALSE;
}
#endif // VULKAN_DEBUG_REPORT

static void FrameRender(ImDrawData* draw_data)
{
    VkResult result;

    ImGui_ImplVulkanH_Window* wd = &g_WindowData;
    VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

    result = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        g_SwapChainRebuild = true;
        return;
    }
    GF_ASSERT(result == VK_SUCCESS, "Failed to acquire next image!");

    s_CurrentFrameIndex = (s_CurrentFrameIndex + 1) % wd->ImageCount;

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    {
        result = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        CheckVkResult(result);

        result = vkResetFences(g_Device, 1, &fd->Fence);
        CheckVkResult(result);
    }

    // Free resources in queue
    {
        for (auto& func : s_ResourceFreeQueue[s_CurrentFrameIndex])
            func();
        s_ResourceFreeQueue[s_CurrentFrameIndex].clear();
    }

    // Free command buffers allocated by Application::GetCommandBuffer
    // These use g_MainWindowData.FrameIndex and not s_CurrentFrameIndex because they're tied to the swapchain image index
    {
        auto& allocatedCommandBuffers = s_CommandBuffers[wd->FrameIndex];
        if (allocatedCommandBuffers.size() > 0)
        {
            vkFreeCommandBuffers(g_Device, fd->CommandPool, (uint32_t)allocatedCommandBuffers.size(), allocatedCommandBuffers.data());
            allocatedCommandBuffers.clear();
        }

        result = vkResetCommandPool(g_Device, fd->CommandPool, 0);
        CheckVkResult(result);

        // Begin command buffer
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        GF_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer!");
    }

    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = wd->RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = wd->Width;
        info.renderArea.extent.height = wd->Height;
        info.clearValueCount = 1;
        info.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        result = vkEndCommandBuffer(fd->CommandBuffer);
        GF_ASSERT(result == VK_SUCCESS, "Cannot end command buffer!");

        result = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
        CheckVkResult(result);
    }
}

static void FramePresent()
{
    if (g_SwapChainRebuild) { return; }

    VkResult result;
    ImGui_ImplVulkanH_Window* wd = &g_WindowData;

    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;

    result = vkQueuePresentKHR(g_Queue, &info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        g_SwapChainRebuild = true;
        return;
    }
    CheckVkResult(result);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}

namespace Grafix
{
    Application* Application::s_AppInstance = nullptr;

    // Vulkan Data
    ////VkInstance              Application::g_Instance = VK_NULL_HANDLE;
    ////VkPhysicalDevice        Application::g_PhysicalDevice = VK_NULL_HANDLE;
    ////VkDevice                Application::g_Device = VK_NULL_HANDLE;
    ////uint32_t                Application::g_QueueFamily = (uint32_t)-1;
    ////VkQueue                 Application::g_Queue = VK_NULL_HANDLE;
    ////VkSurfaceKHR            Application::g_Surface = VK_NULL_HANDLE;
    ////VkDescriptorPool        Application::g_DescriptorPool = VK_NULL_HANDLE;
    ////VkAllocationCallbacks* Application::g_Allocator = nullptr;
    ////VkPipelineCache             Application::g_PipelineCache = VK_NULL_HANDLE;
    ////VkDebugReportCallbackEXT    Application::g_DebugReport{};

    ////ImGui_ImplVulkanH_Window    Application::g_WindowData{};
    ////uint32_t                Application::g_MinImageCount = 2;
    ////bool                    Application::g_SwapChainRebuild = false;

    Application::Application(const AppSpecification& appSpec)
    {
        Init(appSpec);
        s_AppInstance = this;
    }

    Application::~Application()
    {
        Shutdown();
        s_AppInstance = nullptr;
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.push_back(layer);
        layer->OnAttach();
    }

    void Application::Run()
    {
        m_IsRunning = true;
        ImGui_ImplVulkanH_Window* windowData = &g_WindowData;
        ImGuiIO& io = ImGui::GetIO();

        // Main loop
        while (!m_Window->ShouldClose() && m_IsRunning)
        {
            m_Stopwatch.Reset();
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Update all layers
            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate();
            }

            // Resize swap chain?
            if (g_SwapChainRebuild)
            {
                int width, height;
                glfwGetFramebufferSize(m_Window->GetHandle(), &width, &height);
                if (width > 0 && height > 0)
                {
                    ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                    ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device,
                        windowData, g_QueueFamily, nullptr, width, height, g_MinImageCount);
                    windowData->FrameIndex = 0;

                    // Clear allocated command buffers from here since entire pool is destroyed
                    s_CommandBuffers.clear();
                    s_CommandBuffers.resize(windowData->ImageCount);

                    g_SwapChainRebuild = false;
                }
            }

            // Start the ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render ImGui of all layers
            for (Layer* layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }

            // End the ImGui frame
            ImGui::Render();
            ImDrawData* main_draw_data = ImGui::GetDrawData();

            bool minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
            if (!minimized)
                FrameRender(main_draw_data);

            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            // Present Main Platform Window
            if (!minimized)
                FramePresent();

            m_Stopwatch.Stop();
            m_FPS = 1.0f / m_Stopwatch.GetSeconds();
        }
    }

    void Application::Init(const AppSpecification& appSpec)
    {
        VkResult result;
        ImGui_ImplVulkanH_Window* wd = &g_WindowData;

        // Initialize window
        m_Window = std::make_unique<Window>(appSpec.Name, appSpec.Width, appSpec.Height);

        // Initialize Vulkan
        GF_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan not supported!");

        CreateVkInstance();
        SelectPhysicalDevice();
        SelectGraphicsQueueFamily();
        CreateLogicalDevice();
        CreateDescriptorPool();
        CreateWindowSurface();
        CreateFramebuffers();

        // Create Framebuffers

        s_CommandBuffers.resize(wd->ImageCount);
        s_ResourceFreeQueue.resize(wd->ImageCount);

        m_ImGuiLayer = new ImGuiLayer();
        PushLayer(m_ImGuiLayer);

        // TODO: move this into ImGuiLayer::OnAttach()
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(m_Window->GetHandle(), true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = g_Instance;
        initInfo.PhysicalDevice = g_PhysicalDevice;
        initInfo.Device = g_Device;
        initInfo.QueueFamily = g_QueueFamily;
        initInfo.Queue = g_Queue;
        initInfo.PipelineCache = g_PipelineCache;
        initInfo.DescriptorPool = g_DescriptorPool;
        initInfo.Subpass = 0;
        initInfo.MinImageCount = g_MinImageCount;
        initInfo.ImageCount = wd->ImageCount;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = CheckVkResult;
        ImGui_ImplVulkan_Init(&initInfo, wd->RenderPass);

        // Upload Fonts
        {
            // Use any command queue
            VkCommandPool commandPool = wd->Frames[wd->FrameIndex].CommandPool;
            VkCommandBuffer commandBuffer = wd->Frames[wd->FrameIndex].CommandBuffer;

            result = vkResetCommandPool(g_Device, commandPool, 0);
            GF_ASSERT(result == VK_SUCCESS, "Cannot reset command pool!");

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
            GF_ASSERT(result == VK_SUCCESS, "Cannot begin command buffer!");

            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

            VkSubmitInfo endInfo = {};
            endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            endInfo.commandBufferCount = 1;
            endInfo.pCommandBuffers = &commandBuffer;

            result = vkEndCommandBuffer(commandBuffer);
            GF_ASSERT(result == VK_SUCCESS, "Cannot end command buffer!");

            result = vkQueueSubmit(g_Queue, 1, &endInfo, VK_NULL_HANDLE);
            GF_ASSERT(result == VK_SUCCESS, "Cannot submit queue!");

            result = vkDeviceWaitIdle(g_Device);
            GF_ASSERT(result == VK_SUCCESS, "Device is still busy!");

            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void Application::Shutdown()
    {
        VkResult result;
        ImGui_ImplVulkanH_Window* wd = &g_WindowData;

        for (Layer* layer : m_LayerStack)
        {
            layer->OnDetach();
        }
        m_LayerStack.clear();

        // Cleanup
        result = vkDeviceWaitIdle(g_Device);
        GF_ASSERT(result == VK_SUCCESS, "Device is not ready.");

        // Free resources in queue
        for (auto& queue : s_ResourceFreeQueue)
        {
            for (auto& func : queue)
                func();
        }
        s_ResourceFreeQueue.clear();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Cleanup Vulkan window
        ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, wd, nullptr);

        // Cleanup Vulkan
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, nullptr);

#ifdef VULKAN_DEBUG_REPORT
        // Remove the debug report callback

        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance,
            "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, nullptr);
#endif // VULKAN_DEBUG_REPORT

        vkDestroyDevice(g_Device, nullptr);
        vkDestroyInstance(g_Instance, nullptr);
    }

    void Application::CreateVkInstance()
    {
        VkResult result;

        uint32_t extensionsCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

        std::vector<const char*> extensions;
        for (uint32_t i = 0; i < extensionsCount; i++)
        {
            extensions.push_back(glfwExtensions[i]);
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#ifdef VULKAN_DEBUG_REPORT
        // Enable validation layers
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;
        extensions.push_back("VK_EXT_debug_report");
#endif // VULKAN_DEBUG_REPORT

        createInfo.enabledExtensionCount = (uint32_t)extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Create instance
        result = vkCreateInstance(&createInfo, nullptr, &g_Instance);
        GF_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

#ifdef VULKAN_DEBUG_REPORT
        // Setup the debug report callback
        auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
        GF_ASSERT(vkCreateDebugReportCallbackEXT, "Cannot load vkCreateDebugReportCallbackEXT!");

        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = DebugReport;
        debug_report_ci.pUserData = nullptr;

        vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, nullptr, &g_DebugReport);
#endif // VULKAN_DEBUG_REPORT
    }

    void Application::SelectPhysicalDevice()
    {
        VkResult result;

        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
        GF_ASSERT(gpu_count > 0, "Cannot find physical devices with Vulkan support!");

        std::vector<VkPhysicalDevice> gpus(gpu_count);
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data());

        // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
        // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
        // dedicated GPUs) is out of scope of this sample.
        for (VkPhysicalDevice& device : gpus)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                g_PhysicalDevice = device;
                return;
            }
        }

        // Use first GPU (Integrated) is a Discrete one is not available.
        GF_ASSERT(gpu_count > 0, "Cannot find a suitable physical device!");
        g_PhysicalDevice = gpus[0];
    }

    void Application::SelectGraphicsQueueFamily()
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queues(count);
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues.data());

        for (uint32_t i = 0; i < count; i++)
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                g_QueueFamily = i;
                return;
            }

        GF_ASSERT(g_QueueFamily != (uint32_t)-1, "Cannot find queue families!");
    }

    void Application::CreateLogicalDevice()
    {
        VkResult result;

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back("VK_KHR_swapchain");

        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queueCreateInfo[1] = {};
        queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[0].queueFamilyIndex = g_QueueFamily;
        queueCreateInfo[0].queueCount = 1;
        queueCreateInfo[0].pQueuePriorities = queue_priority;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = sizeof(queueCreateInfo) / sizeof(queueCreateInfo[0]);
        createInfo.pQueueCreateInfos = queueCreateInfo;
        createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        result = vkCreateDevice(g_PhysicalDevice, &createInfo, nullptr, &g_Device);
        GF_ASSERT(result == VK_SUCCESS, "Cannot create logical device!");

        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    void Application::CreateDescriptorPool()
    {
        VkResult result;

        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
        poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
        poolInfo.pPoolSizes = poolSizes;

        result = vkCreateDescriptorPool(g_Device, &poolInfo, nullptr, &g_DescriptorPool);
        GF_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
    }

    void Application::CreateWindowSurface()
    {
        VkResult result;

        result = glfwCreateWindowSurface(g_Instance, m_Window->GetHandle(), nullptr, &g_Surface);
        GF_ASSERT(result == VK_SUCCESS, "Failed to create window surface!");
    }

    void Application::CreateFramebuffers()
    {
        int width, height;
        glfwGetFramebufferSize(m_Window->GetHandle(), &width, &height);

        ImGui_ImplVulkanH_Window* wd = &g_WindowData;
        wd->Surface = g_Surface;

        // Check for WSI support
        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
        GF_ASSERT(res == VK_TRUE, "No WSI support on physical device!");

        // Select Surface Format
        const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM
        };
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface,
            requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

        // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

        // Create SwapChain, RenderPass, Framebuffer, etc.
        GF_ASSERT(g_MinImageCount >= 2, "MinImageCount must be at least 2");
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, nullptr, width, height, g_MinImageCount);
    }

    VkInstance Application::GetInstance()
    {
        return g_Instance;
    }

    VkPhysicalDevice Application::GetPhysicalDevice()
    {
        return g_PhysicalDevice;
    }

    VkDevice Application::GetDevice()
    {
        return g_Device;
    }

    VkCommandBuffer Application::GetCommandBuffer()
    {
        VkResult result;
        ImGui_ImplVulkanH_Window* wd = &g_WindowData;

        VkCommandPool commandPool = wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer& commandBuffer = s_CommandBuffers[wd->FrameIndex].emplace_back();

        // Allocate command buffer from pool
        {
            VkCommandBufferAllocateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool = commandPool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            info.commandBufferCount = 1;

            result = vkAllocateCommandBuffers(g_Device, &info, &commandBuffer);
            GF_ASSERT(result == VK_SUCCESS, "Cannot allocate command buffer!");
        }

        // Begin command buffer
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            result = vkBeginCommandBuffer(commandBuffer, &info);
            GF_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer!");
        }
        return commandBuffer;
    }

    void Application::FlushCommandBuffer(VkCommandBuffer commandBuffer)
    {
        VkResult result;
        const uint64_t timeout = 100000000000;

        // End command buffer
        VkSubmitInfo endInfo = {};
        endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        endInfo.commandBufferCount = 1;
        endInfo.pCommandBuffers = &commandBuffer;

        result = vkEndCommandBuffer(commandBuffer);
        GF_ASSERT(result == VK_SUCCESS, "Cannot end command buffer!");

        VkFence fence;

        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = 0;
        result = vkCreateFence(g_Device, &fenceCreateInfo, nullptr, &fence);
        GF_ASSERT(result == VK_SUCCESS, "Cannot create fence!");

        // Submit to queue
        result = vkQueueSubmit(g_Queue, 1, &endInfo, fence);
        GF_ASSERT(result == VK_SUCCESS, "Cannot submit to queue!");

        result = vkWaitForFences(g_Device, 1, &fence, VK_TRUE, timeout);
        GF_ASSERT(result == VK_SUCCESS, "Fence is not ready!");

        // Destroy fence
        vkDestroyFence(g_Device, fence, nullptr);
    }

    void Application::SubmitResourceFree(std::function<void()>&& func)
    {
        s_ResourceFreeQueue[s_CurrentFrameIndex].emplace_back(func);
    }

    ////ImGui_ImplVulkanH_Window& Application::GetWindowData() { return g_WindowData; }
    ////uint32_t Application::GetQueueFamily() { return g_QueueFamily; }
    ////VkQueue Application::GetQueue() { return g_Queue; }
    ////VkSurfaceKHR Application::GetSurface() { return g_Surface; }
    ////VkDescriptorPool Application::GetDescriptorPool() { return g_DescriptorPool; }
    ////VkAllocationCallbacks* Application::GetAllocator() { return g_Allocator; }
    ////VkPipelineCache Application::GetPipelineCache() { return g_PipelineCache; }
    ////uint32_t Application::GetMinImageCount() { return g_MinImageCount; }
    ////VkDebugReportCallbackEXT Application::GetDebugReport() { return g_DebugReport; }
}