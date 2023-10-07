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

        m_HierarchyPanel.BindScene(m_EditorScene);
    }

    void EditorLayer::OnUpdate()
    {
        UpdateMousePosInViewport();

        ////m_EditorCamera.OnUpdate();

        switch (m_ToolState)
        {
        case ToolState::Move: { break; }
        case ToolState::Line: { OnLineToolUpdate(); break; }
        case ToolState::Arc: { OnArcToolUpdate(); break; }
        case ToolState::Circle: { OnCircleToolUpdate(); break; }
        case ToolState::Pen: { OnPenToolUpdate(); break; }
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

            // Temp
            ImGui::ShowDemoWindow();

            UI_MenuBar();
            UI_Viewport();
            UI_Color();
            UI_Toolbar();
            UI_Info();

            m_HierarchyPanel.OnUIRender();
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

        if (!m_IsDrawing)
        {
            switch (e.GetKey())
            {
            case Key::M:
            {
                GF_INFO("Switched to move tool.");
                m_ToolState = ToolState::Move;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::B:
            {
                GF_INFO("Switched to Bucket tool.");
                m_ToolState = ToolState::Bucket;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::L:
            {
                GF_INFO("Switched to line tool.");
                m_ToolState = ToolState::Line;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::A:
            {
                GF_INFO("Switched to arc tool.");
                m_ToolState = ToolState::Arc;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::C:
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::P:
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
                m_HierarchyPanel.SetSelectedEntity({});
                break;
            }
            case Key::T:
            {
                if (control && m_HierarchyPanel.GetSelectedEntity())
                    BeginTransforming();

                break;
            }
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

    void EditorLayer::OnLineToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Line");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& line = entity.AddComponent<LineComponent>();

                line.P0 = { m_MousePosInViewport, 1.0f };
                line.P1 = { m_MousePosInViewport, 1.0f };
                line.Color = m_PickedColor;
            }
        } else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& line = entity.GetComponent<LineComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            // Press Shift key to draw horizontal/vertical lines
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
            {
                if (std::abs(m_MousePosInViewport.x - line.P0.x) < std::abs(m_MousePosInViewport.y - line.P0.y))
                    line.P1 = { line.P0.x, m_MousePosInViewport.y, 1.0f };
                else
                    line.P1 = { m_MousePosInViewport.x, line.P0.y, 1.0f };
            } else
            {
                line.P1 = { m_MousePosInViewport, 1.0f };
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                m_IsDrawing = false;

                // If the line is too short, remove it
                if (glm::distance(line.P0, line.P1) < 0.1f)
                {
                    m_ActiveScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnArcToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;
                m_OperationState = 0;

                Entity entity = m_ActiveScene->CreateEntity("Arc");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& arc = entity.AddComponent<ArcComponent>();

                arc.Center = { m_MousePosInViewport, 1.0f };
                arc.Radius = 0.0f;
                arc.Color = m_PickedColor;

                arc.ShowCenter = true;
                arc.ShowRadius = true;
            }
        } else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& arc = entity.GetComponent<ArcComponent>();

            arc.Color = m_PickedColor;

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            switch (m_OperationState)
            {
            case 0:
            {
                float radius = glm::distance(arc.Center, glm::vec3(m_MousePosInViewport, 1.0f));

                float dx = m_MousePosInViewport.x - arc.Center.x;
                float dy = m_MousePosInViewport.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));

                arc.Radius = radius;
                arc.Angle1 = angle;
                arc.Angle2 = angle;

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    // If the radius is too small, remove the arc
                    if (radius < 0.1f)
                    {
                        m_IsDrawing = false;
                        m_ActiveScene->RemoveEntity(entity);
                        m_HierarchyPanel.SetSelectedEntity({});
                    } else
                    {
                        ++m_OperationState;
                    }
                }
                break;
            }
            case 1:
            {
                float dx = m_MousePosInViewport.x - arc.Center.x;
                float dy = m_MousePosInViewport.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));
                arc.Angle2 = angle;

                // Press Ctrl key to draw a major arc
                if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                    arc.Major = true;
                else
                    arc.Major = false;

                // Confirm the arc
                if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_IsDrawing = false;

                    arc.ShowCenter = false;
                    arc.ShowRadius = false;
                }
                break;
            }
            }
        }
    }

    void EditorLayer::OnCircleToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Circle");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& circle = entity.AddComponent<CircleComponent>();

                circle.Center = { m_MousePosInViewport, 1.0f };
                circle.Color = m_PickedColor;
                circle.ShowCenter = true;
            }
        } else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& circle = entity.GetComponent<CircleComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            circle.Radius = glm::distance(circle.Center, glm::vec3(m_MousePosInViewport, 1.0f));

            // Confirm the circle
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                m_IsDrawing = false;
                circle.ShowCenter = false;

                // If the radius is too small, remove the circle
                if (circle.Radius < 0.1f)
                {
                    m_ActiveScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnPenToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Polygon");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& polygon = entity.AddComponent<PolygonComponent>();

                polygon.Vertices.push_back({ m_MousePosInViewport, 0.0f });
                polygon.Color = m_PickedColor;
                polygon.IsClosed = false;
            }
        } else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& polygon = entity.GetComponent<PolygonComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                polygon.Vertices.push_back({ m_MousePosInViewport, 0.0f });

            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                m_IsDrawing = false;
                polygon.IsClosed = true;

                if (polygon.Vertices.size() < 2)
                {
                    m_ActiveScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
                    return;
                }
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
                if (ImGui::MenuItem("Transform", nullptr, false, m_HierarchyPanel.GetSelectedEntity()))
                    BeginTransforming();

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
            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused);

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
                m_HierarchyPanel.SetSelectedEntity({});
            }

            if (ImGui::Button("Bucket", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to Bucket tool.");
                m_ToolState = ToolState::Bucket;
                m_HierarchyPanel.SetSelectedEntity({});
            }

            if (ImGui::Button("Line", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to line tool.");
                m_ToolState = ToolState::Line;
                m_HierarchyPanel.SetSelectedEntity({});
            }

            if (ImGui::Button("Arc", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to arc tool.");
                m_ToolState = ToolState::Arc;
                m_HierarchyPanel.SetSelectedEntity({});
            }

            if (ImGui::Button("Circle", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
                m_HierarchyPanel.SetSelectedEntity({});
            }

            if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
                m_HierarchyPanel.SetSelectedEntity({});
            }
        }
        ImGui::End();
    }

    void EditorLayer::UI_Info()
    {
        ImGui::Begin("Info");
        {
            ImGui::Text("FPS: %d", (uint32_t)Application::Get().GetFPS());

            if (IsMouseInViewport())
                ImGui::Text("Mouse Position: (%d, %d)", (int)m_MousePosInViewport.x, (int)m_MousePosInViewport.y);
            else
                ImGui::Text("Mouse Position:");
        }
        ImGui::End();
    }

    void EditorLayer::UI_Color()
    {
        ImGui::Begin("Color");
        {
            Entity selectedEntity = m_HierarchyPanel.GetSelectedEntity();

            if (selectedEntity)
            {
                if (selectedEntity.HasComponent<LineComponent>())
                {
                    auto& line = selectedEntity.GetComponent<LineComponent>();
                    m_PickedColor = line.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(line.Color));
                } else if (selectedEntity.HasComponent<ArcComponent>())
                {
                    auto& arc = selectedEntity.GetComponent<ArcComponent>();
                    m_PickedColor = arc.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(arc.Color));
                } else if (selectedEntity.HasComponent<CircleComponent>())
                {
                    auto& circle = selectedEntity.GetComponent<CircleComponent>();
                    m_PickedColor = circle.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(circle.Color));
                } else if (selectedEntity.HasComponent<PolygonComponent>())
                {
                    auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
                    m_PickedColor = polygon.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(polygon.Color));
                }
            } else
            {
                ImGui::ColorPicker3("Color", glm::value_ptr(m_PickedColor));
            }
        }
        ImGui::End();
    }

    void EditorLayer::BeginTransforming()
    {
        Entity selectedEntity = m_HierarchyPanel.GetSelectedEntity();
        auto& transform = selectedEntity.AddComponent<TransformComponent>();

        if (selectedEntity.HasComponent<LineComponent>())
        {
            auto& line = selectedEntity.GetComponent<LineComponent>();
            transform.PivotPoint = (line.P0 + line.P1) / 2.0f;
        } else if (selectedEntity.HasComponent<CircleComponent>())
        {
            auto& circle = selectedEntity.GetComponent<CircleComponent>();
            transform.PivotPoint = circle.Center;
        } else if (selectedEntity.HasComponent<PolygonComponent>())
        {
            auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
            glm::vec3 referencePoint = { 0.0f, 0.0f, 0.0f };
            for (auto& vertex : polygon.Vertices)
                referencePoint += vertex;
            transform.PivotPoint = referencePoint / (float)polygon.Vertices.size();
        }
    }
}