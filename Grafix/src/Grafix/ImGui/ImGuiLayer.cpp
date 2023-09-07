#include "pch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace Grafix
{
    void ImGuiLayer::OnAttach()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        SetThemeColor();

        ////// Setup Platform/Renderer backends
        ////ImGui_ImplGlfw_InitForVulkan(nativeWindow, true);
        ////ImGui_ImplVulkan_InitInfo init_info = {};
        ////init_info.Instance = g_Instance;
        ////init_info.PhysicalDevice = g_PhysicalDevice;
        ////init_info.Device = g_Device;
        ////init_info.QueueFamily = g_QueueFamily;
        ////init_info.Queue = g_Queue;
        ////init_info.PipelineCache = g_PipelineCache;
        ////init_info.DescriptorPool = g_DescriptorPool;
        ////init_info.Subpass = 0;
        ////init_info.MinImageCount = g_MinImageCount;
        ////init_info.ImageCount = wd->ImageCount;
        ////init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ////init_info.Allocator = g_Allocator;
        ////init_info.CheckVkResultFn = check_vk_result;
        ////ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

        ////// Upload Fonts
        ////{
        ////    // Use any command queue
        ////    VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
        ////    VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

        ////    err = vkResetCommandPool(g_Device, command_pool, 0);
        ////    check_vk_result(err);
        ////    VkCommandBufferBeginInfo begin_info = {};
        ////    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        ////    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        ////    err = vkBeginCommandBuffer(command_buffer, &begin_info);
        ////    check_vk_result(err);

        ////    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        ////    VkSubmitInfo end_info = {};
        ////    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        ////    end_info.commandBufferCount = 1;
        ////    end_info.pCommandBuffers = &command_buffer;
        ////    err = vkEndCommandBuffer(command_buffer);
        ////    check_vk_result(err);
        ////    err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
        ////    check_vk_result(err);

        ////    err = vkDeviceWaitIdle(g_Device);
        ////    check_vk_result(err);
        ////    ImGui_ImplVulkan_DestroyFontUploadObjects();
        ////}
    }

    void ImGuiLayer::OnDetach()
    {
    }

    void ImGuiLayer::SetThemeColor()
    {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ////ImGui::Render();
        ////ImDrawData* main_draw_data = ImGui::GetDrawData();
        ////const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        ////wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
        ////wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
        ////wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
        ////wd->ClearValue.color.float32[3] = clear_color.w;
        ////if (!main_is_minimized)
        ////    FrameRender(wd, main_draw_data);
    }
}