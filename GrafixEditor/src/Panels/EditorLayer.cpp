#include "EditorLayer.h"

static uint32_t s_MaxViewportSize = 16384;

EditorLayer::EditorLayer() : Layer("Editor Layer") {}

void EditorLayer::Render(Grafix::Scene& scene)
{
    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(scene);
}

void EditorLayer::OnUpdate()
{
    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my;
    m_MouseXInViewport = (int)mx, m_MouseYInViewport = (int)my;

    if (m_MouseXInViewport >= 0 && m_MouseXInViewport < (int)m_ViewportWidth
        && m_MouseYInViewport >= 0 && m_MouseYInViewport < (int)m_ViewportHeight)
    {
        ////GF_INFO("Mouse inside viewport: ({0}, {1})", mouseX, mouseY);
    }
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

        ////ImGui::ShowDemoWindow();
        UI_MenuBar();
        UI_Viewport();
        UI_Toolbar();
        UI_Settings();
        UI_Entities();
    }
    ImGui::End(); // DockSpace
}

void EditorLayer::OnEvent(Grafix::Event& e)
{
    Grafix::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Grafix::KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<Grafix::MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnKeyPressed(Grafix::KeyPressedEvent& e)
{
    // Shortcuts

    if (e.IsRepeat())
        return false;

    bool control = Grafix::Input::IsKeyPressed(Grafix::Key::LeftControl) || Grafix::Input::IsKeyPressed(Grafix::Key::RightControl);
    bool shift = Grafix::Input::IsKeyPressed(Grafix::Key::LeftShift) || Grafix::Input::IsKeyPressed(Grafix::Key::RightShift);

    switch (e.GetKey())
    {
    case Grafix::Key::M:
    {
        m_ToolState = ToolState::Move;
        GF_INFO("Switched to move tool.");
        break;
    }
    case Grafix::Key::B:
    {
        m_ToolState = ToolState::Brush;
        GF_INFO("Switched to brush tool.");
        break;
    }
    case Grafix::Key::E:
    {
        m_ToolState = ToolState::Eraser;
        GF_INFO("Switched to eraser tool.");
        break;
    }
    case Grafix::Key::P:
    {
        m_ToolState = ToolState::Pen;
        GF_INFO("Switched to pen tool.");
        break;
    }
    case Grafix::Key::L:
    {
        m_ToolState = ToolState::Line;
        GF_INFO("Switched to line tool.");
        break;
    }
    case Grafix::Key::A:
    {
        m_ToolState = ToolState::Arc;
        GF_INFO("Switched to arc tool.");
        break;
    }
    case Grafix::Key::R:
    {
        m_ToolState = ToolState::Rectangle;
        GF_INFO("Switched to rectangle tool.");
        break;
    }
    case Grafix::Key::C:
    {
        m_ToolState = ToolState::Circle;
        GF_INFO("Switched to circle tool.");
        break;
    }
    }

    return false;
}

bool EditorLayer::OnMouseButtonPressed(Grafix::MouseButtonPressedEvent& e)
{
    if (e.GetMouseButton() == Grafix::MouseButton::Left)
    {
    }

    return false;
}

void EditorLayer::UI_MenuBar()
{
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
}

void EditorLayer::UI_Viewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    {
        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };  // Top-left
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };  // Bottom-right

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Grafix::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

        m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
        m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

        if (m_ViewportWidth == 0 || m_ViewportWidth > s_MaxViewportSize || m_ViewportHeight == 0 || m_ViewportHeight > s_MaxViewportSize)
        {
            GF_WARN("Attempt to resize viewport to ({0}, {1})", m_ViewportWidth, m_ViewportHeight);
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
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::UI_Toolbar()
{
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoResize);
    {
        if (ImGui::Button("Move", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to move tool.");
            m_ToolState = ToolState::Move;
        }

        if (ImGui::Button("Brush", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to brush tool.");
            m_ToolState = ToolState::Brush;
        }

        if (ImGui::Button("Eraser", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to eraser tool.");
            m_ToolState = ToolState::Eraser;
        }

        if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to pen tool.");
            m_ToolState = ToolState::Pen;
        }

        if (ImGui::Button("Line", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to line tool.");
            m_ToolState = ToolState::Line;
        }

        if (ImGui::Button("Arc", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to arc tool.");
            m_ToolState = ToolState::Arc;
        }

        if (ImGui::Button("Rectangle", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to rectangle tool.");
            m_ToolState = ToolState::Rectangle;
        }

        if (ImGui::Button("Circle", ImVec2{ 80.0f, 30.0f }))
        {
            GF_INFO("Switched to circle tool.");
            m_ToolState = ToolState::Circle;
        }
    }
    ImGui::End();
}

void EditorLayer::UI_Settings()
{
    ImGui::Begin("Settings");
    {
        ImGui::Text("FPS: %d", (uint32_t)Grafix::Application::Get().GetFPS());
        ImGui::Separator();

        ImGui::Text("Background");
        ImGui::PushID(0);
        ImGui::ColorEdit3("Color", glm::value_ptr(m_Scene.GetBackgroundColor()));
        ImGui::PopID();
        ImGui::Separator();

        ////for (int i = 0; i < m_Scene.GetRectangles().size(); ++i)
        ////{
        ////    auto& rect = m_Scene.GetRectangles()[i];

        ////    ImGui::PushID(i);

        ////    ImGui::Text("Rectangle");
        ////    ImGui::DragFloat2("Position", glm::value_ptr(rect.Position), 2.0f, -2000.0f, 2000.0f);
        ////    ImGui::DragFloat("Width", &rect.Width, 2.0f, 0.0f, 2000.0f);
        ////    ImGui::DragFloat("Height", &rect.Height, 2.0f, 0.0f, 2000.0f);
        ////    ImGui::ColorEdit3("Color", glm::value_ptr(rect.Color));

        ////    ImGui::PopID();
        ////    ImGui::Separator();
        ////}

        switch (m_ToolState)
        {
        case ToolState::Move:
        {
            break;
        }case ToolState::Line:
        {
            ImGui::Text("Line");
            ImGui::PushID(1);
            ////ImGui::DragFloat2("Begin", glm::value_ptr(m_Line.BeginPos), 2.0f, -2000.0f, 2000.0f);
            ////ImGui::DragFloat2("End", glm::value_ptr(m_Line.EndPos), 2.0f, -2000.0f, 2000.0f);
            ImGui::DragFloat("Width", &m_Line.Width, 1.0f, 0.0f, 1000.0f);
            ImGui::ColorEdit4("Color", glm::value_ptr(m_Line.Color));
            ImGui::PopID();
            break;
        }
        }
    }
    ImGui::End();
}

void EditorLayer::UI_Entities()
{
    ImGui::Begin("Entities");
    {
    }
    ImGui::End();
}