#include "EditorLayer.h"

#include "Grafix/Renderer/Image.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

static uint32_t RGBAToUint32(const glm::vec4& color)
{
    uint8_t r = (uint8_t)(color.r * 255.0f);
    uint8_t g = (uint8_t)(color.g * 255.0f);
    uint8_t b = (uint8_t)(color.b * 255.0f);
    uint8_t a = (uint8_t)(color.a * 255.0f);
    return (a << 24) | (b << 16) | (g << 8) | r;
}

void EditorLayer::OnImGuiRender()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::ShowDemoWindow();

        // Create menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    Grafix::Application::Get().Close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Begin("Tools");
        {
            if (ImGui::Button("Line"))
            {
                GF_INFO("Button pressed: Line");
            }
        }
        ImGui::End(); // Tools

        ImGui::Begin("Viewport");
        {
            m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
            m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

            if (m_FinalImage)
            {
                ImGui::Image(m_FinalImage->GetDescriptorSet(),
                    { (float)m_FinalImage->GetWidth(), (float)m_FinalImage->GetHeight() },
                    ImVec2(0, 1), ImVec2(1, 0)
                );
            }
        }
        ImGui::End(); // Viewport

        if (m_FinalImage)
        {
            if (m_FinalImage->GetWidth() == m_ViewportWidth && m_FinalImage->GetHeight() == m_ViewportHeight)
            {
                // Skip
            } else
            {
                m_FinalImage->Resize(m_ViewportWidth, m_ViewportHeight);
                GF_INFO("Image resized: {0}x{1}", m_FinalImage->GetWidth(), m_FinalImage->GetHeight());
            }
        } else
        {
            m_FinalImage = std::make_shared<Grafix::Image>(m_ViewportWidth, m_ViewportHeight, Grafix::ImageFormat::RGBA);
        }

        delete[] m_Pixels;
        m_Pixels = new uint32_t[m_ViewportWidth * m_ViewportHeight];

        for (uint32_t y = 0; y < m_ViewportHeight; y++)
        {
            for (uint32_t x = 0; x < m_ViewportWidth; x++)
            {
                m_Pixels[y * m_ViewportWidth + x] = RGBAToUint32(glm::vec4(m_BackgroundColor, 1.0f));
            }
        }

        for (uint32_t y = 200; y < std::min(300, (int)m_ViewportHeight); ++y)
        {
            for (uint32_t x = 200; x < std::min(300, (int)m_ViewportWidth); ++x)
            {
                m_Pixels[y * m_ViewportWidth + x] = RGBAToUint32(glm::vec4(m_SquareColor, 1.0f));
            }
        }

        m_FinalImage->SetPiexels(m_Pixels);

        ImGui::Begin("Settings");
        {
            ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
            ImGui::ColorEdit3("Background Color", glm::value_ptr(m_BackgroundColor));
        }
        ImGui::End(); // Settings

        ImGui::Begin("Entities");
        ImGui::End(); // Entities
    }

    ImGui::End(); // DockSpace
}