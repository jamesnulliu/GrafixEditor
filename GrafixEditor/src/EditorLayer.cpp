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
            case Key::C:
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
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
            case Key::T:
            {
                if (control && m_SelectedEntity)
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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Line");
                auto& line = m_SelectedEntity.AddComponent<LineComponent>();

                line.P0 = { m_MousePosInViewport, 1.0f };
                line.P1 = { m_MousePosInViewport, 1.0f };
                line.Color = m_PickedColor;
            }
        } else
        {
            auto& line = m_SelectedEntity.GetComponent<LineComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_SelectedEntity);
                m_SelectedEntity = {};
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
                    m_ActiveScene->RemoveEntity(m_SelectedEntity);
                    m_SelectedEntity = {};
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

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_SelectedEntity);
                m_SelectedEntity = {};
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
                        m_ActiveScene->RemoveEntity(m_SelectedEntity);
                        m_SelectedEntity = {};
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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Circle");
                auto& circle = m_SelectedEntity.AddComponent<CircleComponent>();

                circle.Center = { m_MousePosInViewport, 1.0f };
                circle.Color = m_PickedColor;
                circle.ShowCenter = true;
            }
        } else
        {
            auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_SelectedEntity);
                m_SelectedEntity = {};
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
                    m_ActiveScene->RemoveEntity(m_SelectedEntity);
                    m_SelectedEntity = {};
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

                m_SelectedEntity = m_ActiveScene->CreateEntity("Polygon");
                auto& polygon = m_SelectedEntity.AddComponent<PolygonComponent>();

                polygon.Vertices.push_back({ m_MousePosInViewport, 0.0f });
                polygon.Color = m_PickedColor;
                polygon.IsClosed = false;
            }
        } else
        {
            auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(m_SelectedEntity);
                m_SelectedEntity = {};
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
                    m_ActiveScene->RemoveEntity(m_SelectedEntity);
                    m_SelectedEntity = {};
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
                if (ImGui::MenuItem("Transform", nullptr, false, m_SelectedEntity))
                    BeginTransforming();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity"))
            {
                ////if (ImGui::MenuItem("Delete"), nullptr, false, m_SelectedEntity)
                ////    ImGui::OpenPopup("Warning");

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

            if (ImGui::Button("Circle", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to circle tool.");
                m_ToolState = ToolState::Circle;
                m_SelectedEntity = {};
            }

            if (ImGui::Button("Pen", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to pen tool.");
                m_ToolState = ToolState::Pen;
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

    void EditorLayer::UI_Color()
    {
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
                } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                {
                    auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
                    m_PickedColor = circle.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(circle.Color));
                } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
                {
                    auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();
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

    void EditorLayer::UI_Properties()
    {
        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity)
            {
                if (m_SelectedEntity.HasComponent<TransformComponent>())
                {
                    auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
                    ImGui::Text("Transform");

                    DrawFloat3Control("Pivot Point", transform.PivotPoint);

                    DrawFloat3Control("Position", transform.Translation);
                    DrawFloat3Control("Rotation", transform.Rotation);
                    DrawFloat3Control("Scale", transform.Scale);

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
                        } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
                        {
                            auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();
                            auto transform = m_SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();

                            for (glm::vec3& vertex : polygon.Vertices)
                                vertex = transform * glm::vec4(vertex, 1.0f);
                        }

                        EndTransforming();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                        EndTransforming();
                } else
                {
                    if (m_SelectedEntity.HasComponent<TagComponent>())
                    {
                        auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;

                        char buffer[256];
                        memset(buffer, 0, sizeof(buffer));
                        strcpy_s(buffer, sizeof(buffer), tag.c_str());

                        if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
                            tag = std::string(buffer);
                    }

                    if (m_SelectedEntity.HasComponent<LineComponent>())
                    {
                        auto& line = m_SelectedEntity.GetComponent<LineComponent>();

                        ImGui::DragFloat2("Point A", glm::value_ptr(line.P0), 0.5f);
                        ImGui::DragFloat2("Point B", glm::value_ptr(line.P1), 0.5f);
                        ImGui::DragFloat("Width", &line.Width, 0.5f);
                        if (line.Style == LineStyle::Dashed)
                            ImGui::DragFloat("Dash Length", &line.DashLength, 0.5f);

                        ImGui::ColorEdit3("Color", glm::value_ptr(line.Color));
                    } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                    {
                        auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

                        ImGui::DragFloat2("Center", glm::value_ptr(circle.Center), 0.5f);
                        ImGui::DragFloat("Radius", &circle.Radius, 0.5f, 0.0f, std::numeric_limits<float>::max());

                        ImGui::ColorEdit3("Color", glm::value_ptr(circle.Color));

                        ImGui::Checkbox("Show Center", &circle.ShowCenter);
                    } else if (m_SelectedEntity.HasComponent<ArcComponent>())
                    {
                        auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();

                        ImGui::DragFloat2("Center", glm::value_ptr(arc.Center), 0.5f);
                        ImGui::DragFloat("Radius", &arc.Radius, 0.5f, 0.0f, std::numeric_limits<float>::max());
                        ImGui::DragFloat("Angle 1", &arc.Angle1, 0.5f);
                        ImGui::DragFloat("Angle 2", &arc.Angle2, 0.5f);
                        ImGui::Checkbox("Major", &arc.Major);

                        ImGui::ColorEdit3("Color", glm::value_ptr(arc.Color));

                        ImGui::Checkbox("Show Center", &arc.ShowCenter);
                    } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
                    {
                        auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();

                        for (int i = 0; i < polygon.Vertices.size(); ++i)
                        {
                            ImGui::PushID(i);
                            std::string label = "Vertex " + std::to_string(i);
                            ImGui::DragFloat2(label.c_str(), glm::value_ptr(polygon.Vertices[i]), 0.5f);
                            ImGui::PopID();
                        }

                        ImGui::ColorEdit3("Color", glm::value_ptr(polygon.Color));
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

            if (m_SelectedEntity && ImGui::IsKeyPressed(ImGuiKey_Delete))
                ImGui::OpenPopup("Warning");

            m_IsModalOpen = ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (m_IsModalOpen)
            {
                ImGui::Text("Delete this entity?");

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    m_ActiveScene->RemoveEntity(m_SelectedEntity);
                    m_SelectedEntity = {};
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void EditorLayer::DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, 0.5f);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, 0.5f);

        ImGui::SameLine();

        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::DragFloat("##Z", z, 0.5f);

        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    void EditorLayer::DrawFloat3Control(const std::string& label, glm::vec3& values, float columnWidth)
    {
        DrawFloat3Control(label, &values.x, &values.y, &values.z, columnWidth);
    }

    void EditorLayer::BeginTransforming()
    {
        auto& transform = m_SelectedEntity.AddComponent<TransformComponent>();

        if (m_SelectedEntity.HasComponent<LineComponent>())
        {
            auto& line = m_SelectedEntity.GetComponent<LineComponent>();
            transform.PivotPoint = (line.P0 + line.P1) / 2.0f;
        } else if (m_SelectedEntity.HasComponent<CircleComponent>())
        {
            auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
            transform.PivotPoint = circle.Center;
        } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
        {
            auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();
            glm::vec3 referencePoint = { 0.0f, 0.0f, 0.0f };
            for (auto& vertex : polygon.Vertices)
                referencePoint += vertex;
            transform.PivotPoint = referencePoint / (float)polygon.Vertices.size();
        }
    }

    void EditorLayer::EndTransforming()
    {
        m_SelectedEntity.RemoveComponent<TransformComponent>();
    }
}