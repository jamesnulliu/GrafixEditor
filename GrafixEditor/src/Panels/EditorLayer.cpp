#include "EditorLayer.h"

static uint32_t s_MaxViewportSize = 16384;

void EditorLayer::Render(Grafix::Scene& scene)
{
    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(scene);
}

void EditorLayer::OnImGuiRender()
{
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    windowFlags |= ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("VulkanAppDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
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
            if (ImGui::Button("Mouse", ImVec2{ 80.0f, 30.0f }))
            {
            }

            if (ImGui::Button("Line", ImVec2{ 80.0f, 30.0f }))
            {
            }

            if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
            {
            }

            if (ImGui::Button("Paint", ImVec2{ 80.0f, 30.0f }))
            {
            }

            if (ImGui::Button("Polygon", ImVec2{ 80.0f, 30.0f }))
            {
            }
        }
        ImGui::End(); // Tools

        ImGui::Begin("Viewport");
        {
            m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
            m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();
            Grafix::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

            glm::vec2 mousePos = Grafix::Input::GetMousePosition();
            GF_TRACE("MousePos: ({0}, {1})", mousePos.x, mousePos.y);

            if (m_ViewportWidth == 0 || m_ViewportWidth > s_MaxViewportSize || m_ViewportHeight == 0 || m_ViewportHeight > s_MaxViewportSize)
            {
                GF_TRACE("Attempt to resize viewport to ({0}, {1})", m_ViewportWidth, m_ViewportHeight);
            } else
            {
                auto image = m_Renderer.GetImage();
                if (image)
                {
                    ImGui::Image(image->GetDescriptorSet(),
                        { (float)image->GetWidth(), (float)image->GetHeight() },
                        ImVec2(0, 1), ImVec2(1, 0)
                    );
                }

                Render(m_Scene);
            }
        }
        ImGui::End(); // Viewport

        ImGui::Begin("Settings");
        {
            ImGui::Text("FPS: %d", (uint32_t)Grafix::Application::Get().GetFPS());
            ImGui::Separator();

            ImGui::Text("Background");
            ImGui::ColorEdit3("Color", glm::value_ptr(m_Scene.GetBackgroundColor()));
            ImGui::Separator();

            for (int i = 0; i < m_Scene.GetRectangles().size(); ++i)
            {
                auto& rect = m_Scene.GetRectangles()[i];

                ImGui::PushID(i);

                ImGui::Text("Rectangle");
                ImGui::DragFloat2("Position", glm::value_ptr(rect.Position), 2.0f, -2000.0f, 2000.0f);
                ImGui::DragFloat("Width", &rect.Width, 2.0f, 0.0f, 2000.f);
                ImGui::DragFloat("Height", &rect.Height, 2.0f, 0.0f, 2000.f);
                ImGui::ColorEdit3("Color", glm::value_ptr(rect.Color));

                ImGui::PopID();
                ImGui::Separator();
            }
        }
        ImGui::End(); // Settings

        ImGui::Begin("Entities");
        {
        }
        ImGui::End(); // Entities
    }

    ImGui::End(); // DockSpace
}