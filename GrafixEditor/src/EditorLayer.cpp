#include "EditorLayer.h"

namespace Grafix
{
    static uint32_t s_MaxViewportSize = 16384;

    EditorLayer::EditorLayer()
        : Layer("Editor Layer")
    {
    }

    void EditorLayer::Render()
    {
        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_EditorCamera.SetViewportSize((float)m_ViewportWidth, (float)m_ViewportHeight);

        m_Renderer.Render(*m_ActiveScene, m_EditorCamera);
    }

    void EditorLayer::OnAttach()
    {
        m_EditorScene = std::make_shared<Scene>();
        m_ActiveScene = m_EditorScene;
    }

    void EditorLayer::OnUpdate()
    {
        UpdateMousePosInViewport();
        ////m_EditorCamera.OnUpdate();

        switch (m_ToolState)
        {
        case ToolState::Move: { break; }
        case ToolState::Pen: { OnPenToolUpdate(); break; }
        case ToolState::Line: { OnLineToolUpdate(); break; }
        case ToolState::Arc: { OnArcToolUpdate(); break; }
        case ToolState::Circle: { OnCircleToolUpdate(); break; }
        }

        m_EditorScene->OnUpdate();
    }

    void EditorLayer::OnUIRender()
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

            UI_MenuBar();
            UI_Viewport();
            UI_Toolbar();
            UI_Information();
            UI_Settings();
            UI_Entities();
        }
        ImGui::End(); // DockSpace

        Render();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        // Shortcuts

        if (e.IsRepeat())
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        switch (e.GetKey())
        {
        case Key::M:
        {
            m_ToolState = ToolState::Move;
            GF_INFO("Switched to move tool.");
            break;
        }
        case Key::P:
        {
            m_ToolState = ToolState::Pen;
            GF_INFO("Switched to pen tool.");
            break;
        }
        case Key::B:
        {
            m_ToolState = ToolState::Bucket;
            GF_INFO("Switched to Bucket tool.");
            break;
        }
        case Key::L:
        {
            m_ToolState = ToolState::Line;
            GF_INFO("Switched to line tool.");
            m_IsDrawing = false;
            break;
        }
        case Key::A:
        {
            m_ToolState = ToolState::Arc;
            GF_INFO("Switched to arc tool.");
            m_IsDrawing = false;
            break;
        }
        case Key::R:
        {
            m_ToolState = ToolState::Rectangle;
            GF_INFO("Switched to rectangle tool.");
            break;
        }
        case Key::C:
        {
            m_ToolState = ToolState::Circle;
            GF_INFO("Switched to circle tool.");
            m_IsDrawing = false;
            break;
        }
        }

        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == MouseButton::Left)
        {
        }

        return false;
    }

    void EditorLayer::UpdateMousePosInViewport()
    {
        auto [mx, my] = ImGui::GetMousePos();
        m_MousePosInViewport = glm::i32vec2{
            (int)(mx - m_ViewportBounds[0].x),
            (int)(m_ViewportBounds[1].y - my)
        };
    }

    bool EditorLayer::IsMouseInViewport() const
    {
        return m_MousePosInViewport.x >= 0 && m_MousePosInViewport.x < (int)m_ViewportWidth
            && m_MousePosInViewport.y >= 0 && m_MousePosInViewport.y < (int)m_ViewportHeight;
    }

    // -------------------------------------------------------------------
    // ------------------------------ Tools ------------------------------
    // -------------------------------------------------------------------

    void EditorLayer::OnPenToolUpdate()
    {
    }

    void EditorLayer::OnLineToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && Input::IsMouseButtonPressed(MouseButton::Left))
            {
                m_IsDrawing = true;

                m_EditingEntity = m_ActiveScene->CreateEntity("Line");
                auto& line = m_EditingEntity.AddComponent<LineRendererComponent>();

                line.P0 = { m_MousePosInViewport, 1.0f };
                line.P1 = { m_MousePosInViewport, 1.0f };
            }
        } else
        {
            auto& line = m_EditingEntity.GetComponent<LineRendererComponent>();

            // If ESC is pressed, remove the line
            if (Input::IsKeyPressed(Key::Escape) || Input::IsMouseButtonPressed(MouseButton::Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_EditingEntity);
                return;
            }

            if (Input::IsMouseButtonPressed(MouseButton::Left))
            {
                // Press Shift key to draw horizontal/vertical lines
                if (Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift))
                {
                    if (std::abs(m_MousePosInViewport.x - line.P0.x) < std::abs(m_MousePosInViewport.y - line.P0.y))
                        line.P1 = { line.P0.x, m_MousePosInViewport.y, 1.0f };
                    else
                        line.P1 = { m_MousePosInViewport.x, line.P0.y, 1.0f };
                } else
                {
                    line.P1 = { m_MousePosInViewport, 1.0f };
                }
            } else
            {
                m_IsDrawing = false;

                // If the line has no length, remove it
                if (glm::distance(line.P0, line.P1) < 0.1f)
                    m_ActiveScene->RemoveEntity(m_EditingEntity);
            }
        }
    }

    void EditorLayer::OnArcToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && Input::IsMouseButtonPressed(MouseButton::Left))
            {
                m_IsDrawing = true;
                m_OperationState = 0;
                m_IsConfirmed = false;

                m_EditingEntity = m_ActiveScene->CreateEntity("Arc");
                auto& arc = m_EditingEntity.AddComponent<ArcRendererComponent>();

                arc.Center = { m_MousePosInViewport, 1.0f };
                arc.Radius = 0.0f;
                arc.ShowCenter = true;
                arc.ShowRadius = true;
            }
        } else
        {
            auto& arc = m_EditingEntity.GetComponent<ArcRendererComponent>();

            if (m_OperationState == 0)
            {
                if (Input::IsMouseButtonPressed(MouseButton::Left))
                {
                    float radius = glm::distance(arc.Center, glm::vec3(m_MousePosInViewport, 1.0f));
                    arc.Radius = radius;

                    float dx = m_MousePosInViewport.x - arc.Center.x;
                    float dy = m_MousePosInViewport.y - arc.Center.y;
                    float angle = glm::degrees(glm::atan(dy, dx));
                    arc.Angle1 = angle;
                    arc.Angle2 = angle;
                } else
                {
                    ++m_OperationState;
                }
            } else if (m_OperationState == 1)
            {
                float dx = m_MousePosInViewport.x - arc.Center.x;
                float dy = m_MousePosInViewport.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));
                arc.Angle2 = angle;

                // Press Ctrl key to draw a major arc
                if (Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl))
                    arc.Major = true;
                else
                    arc.Major = false;

                if ((m_ViewportHovered && Input::IsMouseButtonPressed(MouseButton::Left))
                    || Input::IsKeyPressed(Key::Enter))
                {
                    m_IsConfirmed = true;
                } else
                {
                    if (m_IsConfirmed)
                    {
                        m_IsDrawing = false;
                        m_IsConfirmed = false;

                        arc.ShowCenter = false;
                        arc.ShowRadius = false;
                    }
                }
            }
        }
    }

    void EditorLayer::OnCircleToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && Input::IsMouseButtonPressed(MouseButton::Left))
            {
                m_IsDrawing = true;

                m_EditingEntity = m_ActiveScene->CreateEntity("Circle");
                auto& circle = m_EditingEntity.AddComponent<CircleRendererComponent>();
                circle.Center = { m_MousePosInViewport, 1.0f };
                circle.Radius = 0.0f;
                circle.ShowCenter = true;
            }
        } else
        {
            auto& circle = m_EditingEntity.GetComponent<CircleRendererComponent>();

            if (Input::IsMouseButtonPressed(MouseButton::Left))
            {
                circle.Radius = glm::distance(circle.Center, glm::vec3(m_MousePosInViewport, 1.0f));
            } else
            {
                m_IsDrawing = false;
                circle.ShowCenter = false;
            }
        }
    }

    // -----------------------------------------------------------------
    // --------------------------- UI Panels ---------------------------
    // -----------------------------------------------------------------

    void EditorLayer::UI_MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Clear"))
                    m_ActiveScene->Clear();

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
            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

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

            if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
            }

            if (ImGui::Button("Bucket", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to Bucket tool.");
                m_ToolState = ToolState::Bucket;
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

    void EditorLayer::UI_Information()
    {
        ImGui::Begin("Information");
        {
            ImGui::Text("FPS: %d", (uint32_t)Application::Get().GetFPS());

            if (IsMouseInViewport())
                ImGui::Text("Mouse Position: (%d, %d)", (int)m_MousePosInViewport.x, (int)m_MousePosInViewport.y);
            else
                ImGui::Text("Mouse Position:");
        }
        ImGui::End();
    }

    void EditorLayer::UI_Settings()
    {
        ImGui::Begin("Settings");
        {
            ImGui::ColorEdit3("Background Color", glm::value_ptr(m_ActiveScene->GetBackgroundColor()));
            ////ImGui::DragFloat2("Editor Camera", glm::value_ptr(m_EditorCamera.GetPosition()), 0.1f, -200.0f, 200.0f);
            ImGui::Separator();

            for (auto entity : m_ActiveScene->GetEntities())
            {
                ImGui::PushID(entity.GetTag().c_str());
                {
                    if (entity.HasComponent<LineRendererComponent, TransformComponent>())
                    {
                        auto line = entity.GetComponent<LineRendererComponent>();
                        ImGui::Text(entity.GetTag().c_str());

                        ImGui::DragFloat2("Point A", glm::value_ptr(line.P0), 1.0f, -2000.0f, 2000.0f);
                        ImGui::DragFloat2("Point B", glm::value_ptr(line.P1), 1.0f, -2000.0f, 2000.0f);
                        ImGui::SliderFloat("Width", &line.Width, 0.0f, 1000.0f);
                        if (line.Style == LineStyle::Dashed)
                            ImGui::SliderFloat("Dash Length", &line.DashLength, 1, 100);

                        ImGui::ColorEdit4("Color", glm::value_ptr(line.Color));
                    } else if (entity.HasComponent<CircleRendererComponent>())
                    {
                        auto& circle = entity.GetComponent<CircleRendererComponent>();
                        ImGui::Text(entity.GetTag().c_str());

                        ImGui::DragFloat2("Center", glm::value_ptr(circle.Center), 1.0f, -2000.0f, 2000.0f);
                        ImGui::SliderFloat("Radius", &circle.Radius, 0.0f, 2000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

                        ImGui::ColorEdit4("Color", glm::value_ptr(circle.Color));

                        ImGui::Checkbox("Show Center", &circle.ShowCenter);
                    } else if (entity.HasComponent<ArcRendererComponent>())
                    {
                        auto& arc = entity.GetComponent<ArcRendererComponent>();
                        ImGui::Text(entity.GetTag().c_str());

                        ImGui::DragFloat2("Center", glm::value_ptr(arc.Center), 1.0f, -2000.0f, 2000.0f);
                        ImGui::SliderFloat("Radius", &arc.Radius, 0.0f, 2000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                        ImGui::SliderFloat("Angle 1", &arc.Angle1, -720.0f, 720.0f);
                        ImGui::SliderFloat("Angle 2", &arc.Angle2, -720.0f, 720.0f);
                        ImGui::Checkbox("Major", &arc.Major);

                        ImGui::ColorEdit4("Color", glm::value_ptr(arc.Color));

                        ImGui::Checkbox("Show Center", &arc.ShowCenter);
                    }
                }
                ImGui::PopID();
                ImGui::Separator();
            }
        }
        ImGui::End();
    }

    void EditorLayer::UI_Entities()
    {
        ImGui::Begin("Entities");
        {
            for (auto& entity : m_ActiveScene->GetEntities())
                ImGui::Text(entity.GetTag().c_str());
        }
        ImGui::End();
    }
}