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
            UI_Info();
            UI_Properties();
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

        if (!m_IsDrawing)
        {
            switch (e.GetKey())
            {
            case Key::M:
            {
                GF_INFO("Switched to move tool.");
                m_ToolState = ToolState::Move;
                m_SelectedEntity = {};
                break;
            }
            case Key::P:
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
                m_SelectedEntity = {};
                break;
            }
            case Key::B:
            {
                GF_INFO("Switched to Bucket tool.");
                m_ToolState = ToolState::Bucket;
                m_SelectedEntity = {};
                break;
            }
            case Key::L:
            {
                GF_INFO("Switched to line tool.");
                m_ToolState = ToolState::Line;
                m_SelectedEntity = {};
                break;
            }
            case Key::A:
            {
                GF_INFO("Switched to arc tool.");
                m_ToolState = ToolState::Arc;
                m_SelectedEntity = {};
                break;
            }
            case Key::R:
            {
                GF_INFO("Switched to rectangle tool.");
                m_ToolState = ToolState::Rectangle;
                m_SelectedEntity = {};
                break;
            }
            case Key::C:
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
                m_SelectedEntity = {};
                break;
            }
            case Key::T:
            {
                if (control && m_SelectedEntity)
                {
                    auto& transform = m_SelectedEntity.AddComponent<TransformComponent>();
                    if (m_SelectedEntity.HasComponent<LineComponent>())
                    {
                        auto& line = m_SelectedEntity.GetComponent<LineComponent>();
                        transform.ReferencePoint = line.GetDefaultReferencePoint();
                    } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                    {
                        auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
                        transform.ReferencePoint = circle.GetDefaultReferencePoint();
                    }
                }
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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Line");
                auto& line = m_SelectedEntity.AddComponent<LineComponent>();

                line.P0 = { m_MousePosInViewport, 1.0f };
                line.P1 = { m_MousePosInViewport, 1.0f };
                line.Color = m_PickedColor;
            }
        } else
        {
            auto& line = m_SelectedEntity.GetComponent<LineComponent>();

            // If ESC is pressed, remove the line
            if (Input::IsKeyPressed(Key::Escape) || Input::IsMouseButtonPressed(MouseButton::Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_SelectedEntity);
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
                    m_ActiveScene->RemoveEntity(m_SelectedEntity);
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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Arc");
                auto& arc = m_SelectedEntity.AddComponent<ArcComponent>();

                arc.Center = { m_MousePosInViewport, 1.0f };
                arc.Radius = 0.0f;
                arc.Color = m_PickedColor;

                arc.ShowCenter = true;
                arc.ShowRadius = true;
            }
        } else
        {
            auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();
            arc.Color = m_PickedColor;

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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Circle");
                auto& circle = m_SelectedEntity.AddComponent<CircleComponent>();
                circle.Center = { m_MousePosInViewport, 1.0f };
                circle.Radius = 0.0f;
                circle.Color = m_PickedColor;

                circle.ShowCenter = true;
            }
        } else
        {
            auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

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
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Bucket", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to Bucket tool.");
                m_ToolState = ToolState::Bucket;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Line", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to line tool.");
                m_ToolState = ToolState::Line;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Arc", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to arc tool.");
                m_ToolState = ToolState::Arc;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Rectangle", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to rectangle tool.");
                m_ToolState = ToolState::Rectangle;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Circle", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
                m_SelectedEntity = {};
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

    void EditorLayer::UI_Properties()
    {
        ImGui::ShowDemoWindow();

        ImGui::Begin("Color");
        {
            if (m_SelectedEntity)
            {
                if (m_SelectedEntity.HasComponent<LineComponent>())
                {
                    auto& line = m_SelectedEntity.GetComponent<LineComponent>();
                    m_PickedColor = line.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(line.Color));
                } else if (m_SelectedEntity.HasComponent<ArcComponent>())
                {
                    auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();
                    m_PickedColor = arc.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(arc.Color));
                } /*else if (m_SelectedEntity.HasComponent<RectangleRendererComponent>())
                {
                    auto& rectangle = m_SelectedEntity.GetComponent<RectangleRendererComponent>();
                    m_PickedColor = rectangle.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(rectangle.Color));
                }*/ else if (m_SelectedEntity.HasComponent<CircleComponent>())
                {
                    auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
                    m_PickedColor = circle.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(circle.Color));
                }
            } else
            {
                ImGui::ColorPicker3("Color", glm::value_ptr(m_PickedColor));
            }
        }
        ImGui::End();

        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity)
            {
                if (m_SelectedEntity.HasComponent<TransformComponent>())  // Transforming
                {
                    auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
                    ImGui::Text("Transform");

                    ImGui::DragFloat2("Reference Point", glm::value_ptr(transform.ReferencePoint), 1.0f, -2000.0f, 2000.0f);

                    ImGui::DragFloat2("Position", glm::value_ptr(transform.Translation), 1.0f, -5000.0f, 5000.0f);
                    ImGui::DragFloat("Rotation", &transform.Rotation.z, 1.0f, -5000.0f, 5000.0f);

                    if (m_SelectedEntity.HasAnyOfComponents<LineComponent>())  // Ratio is changeable
                    {
                        if (transform.KeepRatio)
                        {
                            ImGui::DragFloat("Scale", &transform.Scale.x, 1.0f, 0.0f, 1000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                            transform.Scale.y = transform.Scale.x;
                        } else
                        {
                            ImGui::DragFloat2("Scale", glm::value_ptr(transform.Scale), 1.0f, 0.0f, 1000.0f);
                        }
                        ImGui::SameLine();

                        ImGui::Checkbox("Keep Ratio", &transform.KeepRatio);
                    } else
                    {
                        ImGui::DragFloat("Scale", &transform.Scale.x, 1.0f, 0.0f, 1000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                        transform.Scale.y = transform.Scale.x;
                    }

                    if (ImGui::Button("Apply"))
                    {
                        if (m_SelectedEntity.HasComponent<LineComponent>())
                        {
                            auto& line = m_SelectedEntity.GetComponent<LineComponent>();
                            auto transform = m_SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();
                            line.P0 = transform * glm::vec4(line.P0, 1.0f);
                            line.P1 = transform * glm::vec4(line.P1, 1.0f);
                        } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                        {
                            auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

                            auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
                            auto transformMatrix = m_SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();

                            circle.Center = transformMatrix * glm::vec4(circle.Center, 1.0f);
                            circle.Radius = transform.Scale.x * circle.Radius;
                        }

                        m_SelectedEntity.RemoveComponent<TransformComponent>();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                        m_SelectedEntity.RemoveComponent<TransformComponent>();
                } else  // Normal properties
                {
                    if (m_SelectedEntity.HasComponent<LineComponent>())
                    {
                        auto& line = m_SelectedEntity.GetComponent<LineComponent>();
                        ImGui::Text(m_SelectedEntity.GetTag().c_str());

                        ImGui::DragFloat2("Point A", glm::value_ptr(line.P0), 1.0f, -5000.0f, 5000.0f);
                        ImGui::DragFloat2("Point B", glm::value_ptr(line.P1), 1.0f, -5000.0f, 5000.0f);
                        ImGui::DragFloat("Width", &line.Width, 1.0f, 0.0f, 1000.0f);
                        if (line.Style == LineStyle::Dashed)
                            ImGui::DragFloat("Dash Length", &line.DashLength, 1.0f, 1.0f, 100.0f);

                        ImGui::ColorEdit3("Color", glm::value_ptr(line.Color));
                    } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                    {
                        auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
                        ImGui::Text(m_SelectedEntity.GetTag().c_str());

                        ImGui::DragFloat2("Center", glm::value_ptr(circle.Center), 1.0f, -5000.0f, 5000.0f);
                        ImGui::SliderFloat("Radius", &circle.Radius, 0.0f, 5000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

                        ImGui::ColorEdit3("Color", glm::value_ptr(circle.Color));

                        ImGui::Checkbox("Show Center", &circle.ShowCenter);
                    } else if (m_SelectedEntity.HasComponent<ArcComponent>())
                    {
                        auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();
                        ImGui::Text(m_SelectedEntity.GetTag().c_str());

                        ImGui::DragFloat2("Center", glm::value_ptr(arc.Center), 1.0f, -2000.0f, 2000.0f);
                        ImGui::SliderFloat("Radius", &arc.Radius, 0.0f, 5000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                        ImGui::DragFloat("Angle 1", &arc.Angle1, 1.0f, -5000.0f, 5000.0f);
                        ImGui::DragFloat("Angle 2", &arc.Angle2, 1.0f, -5000.0f, 5000.0f);
                        ImGui::Checkbox("Major", &arc.Major);

                        ImGui::ColorEdit3("Color", glm::value_ptr(arc.Color));

                        ImGui::Checkbox("Show Center", &arc.ShowCenter);
                    }
                }
            } else
            {
                ImGui::Text("Background");
                ImGui::ColorEdit3("Color", glm::value_ptr(m_ActiveScene->GetBackgroundColor()));
            }
        }
        ImGui::End();
    }

    void EditorLayer::UI_Entities()
    {
        ImGui::Begin("Entities");
        {
            for (auto it = m_ActiveScene->GetEntities().rbegin(); it != m_ActiveScene->GetEntities().rend(); ++it)
            {
                Entity entity = *it;
                bool selected = m_SelectedEntity == entity;

                ImGui::Selectable(entity.GetTag().c_str(), &selected);

                if (ImGui::IsItemClicked())
                    m_SelectedEntity = entity;
            }

            // Left-click on blank space to unselect
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                m_SelectedEntity = {};

            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonDefault_))
            {
                // Not done
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_ActiveScene->CreateEntity("Empty Entity");

                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
}