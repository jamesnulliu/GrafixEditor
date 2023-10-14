#include "EditorLayer.h"

namespace Grafix
{
    static uint32_t s_MaxViewportSize = 16384;

    EditorLayer::EditorLayer() : Layer("Editor Layer") {}

    void EditorLayer::Render()
    {
        for (auto entity : m_EditorScene->GetEntities())
        {
            auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<LineComponent>())
            {
                auto& line = entity.GetComponent<LineComponent>();
                m_Renderer.DrawLine(transform, line.P0, line.P1, line.Color, line.Style, line.DashLength);
            }
            else if (entity.HasComponent<CircleComponent>())
            {
                auto& circle = entity.GetComponent<CircleComponent>();
                m_Renderer.DrawCircle(transform, circle.Center, circle.Radius * transform.Scale.x, circle.Color);
            }
            else if (entity.HasComponent<ArcComponent>())
            {
                auto& arc = entity.GetComponent<ArcComponent>();
                m_Renderer.DrawArc(transform, arc.Center, arc.Radius * transform.Scale.x, arc.Angle1, arc.Angle2, arc.Major, arc.Color);
            }
            else if (entity.HasComponent<PolygonComponent>())
            {
                auto& polygon = entity.GetComponent<PolygonComponent>();
                m_Renderer.DrawPolygon(transform, polygon.Vertices, polygon.Color);
            }
            else if (entity.HasComponent<SeedFillComponent>())
            {
                auto& seed = entity.GetComponent<SeedFillComponent>();
                m_Renderer.Fill(seed.SeedPoint, seed.FillColor);
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                m_Renderer.DrawCurve(curve.ControlPoints, curve.Color, curve.Order, curve.Step, curve.Knots, curve.Weights);
            }
        }

        // Aux lines
        if (auto entity = m_HierarchyPanel.GetSelectedEntity())
        {
            auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<CircleComponent>())
            {
                auto& circle = entity.GetComponent<CircleComponent>();
                m_Renderer.DrawCross(transform, circle.Center, 5.0f, m_AuxColor, LineStyle::Solid);
            }
            else if (entity.HasComponent<ArcComponent>())
            {
                auto& arc = entity.GetComponent<ArcComponent>();
                m_Renderer.DrawCross(transform, arc.Center, 5.0f, m_AuxColor, LineStyle::Solid);

                glm::vec2 delta1 = arc.Radius *
                    glm::vec2{ glm::cos(glm::radians(arc.Angle1)), glm::sin(glm::radians(arc.Angle1)) };
                m_Renderer.DrawLine(transform, arc.Center, arc.Center + delta1, m_AuxColor, LineStyle::Dashed, 20.0f);

                glm::vec2 delta2 = arc.Radius *
                    glm::vec2{ glm::cos(glm::radians(arc.Angle2)), glm::sin(glm::radians(arc.Angle2)) };
                m_Renderer.DrawLine(transform, arc.Center, arc.Center + delta2, m_AuxColor, LineStyle::Dashed, 20.0f);
            }
            else if (entity.HasComponent<PolygonComponent>())
            {
                auto& polygon = entity.GetComponent<PolygonComponent>();
                m_Renderer.DrawPolygon(transform, polygon.Vertices, polygon.Color);
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                for (int i = 0; i < curve.ControlPoints.size() - 1; i++)
                    m_Renderer.DrawLine(curve.ControlPoints[i], curve.ControlPoints[i + 1], m_AuxColor, LineStyle::Dashed, 20.0f);
            }
        }
    }

    void EditorLayer::OnAttach()
    {
        m_EditorScene = std::make_shared<Scene>();
        m_HierarchyPanel.BindScene(m_EditorScene);

        ////// Game
        ////auto entity = m_EditorScene->CreateEntity("Bird");
        ////auto& transform = entity.GetComponent<TransformComponent>();
        ////transform.Translation = { 420.0f, 360.0f };
        ////auto& triangle = entity.AddComponent<PolygonComponent>();
        ////triangle.Vertices = { { -20.0f, 30.0f }, { -20.0f, -30.0f }, { 40.0f, 0.0f }, { -20.0f, 30.0f } };
        ////triangle.Color = glm::vec3(1.0f, 0.824f, 0.0f);
    }

    void EditorLayer::OnUpdate()
    {
        ////// Game
        ////auto entity = m_EditorScene->GetEntity("Bird");
        ////auto& transform = entity.GetComponent<TransformComponent>();
        ////transform.Translation.x += 0.4f;

        ////if (Input::IsKeyPressed(Key::Space))
        ////{
        ////    transform.Translation.y += 1.0f;
        ////    transform.Rotation = std::max(transform.Rotation + 0.0004f, 30.0f);
        ////} else
        ////{
        ////    transform.Translation.y -= 1.0f;
        ////    transform.Rotation = std::min(transform.Rotation - 0.0004f, -30.0f);
        ////}

        UpdateMousePosInCanvas();

        ////m_Camera.SetPosition(glm::vec2{ transform.Translation.x - 320.f, 0.0f });
        ////m_Camera.OnUpdate();

        switch (m_ToolState)
        {
        case ToolState::Move: { break; }
        case ToolState::Bucket: { OnBucketToolUpdate(); break; }
        case ToolState::Line: { OnLineToolUpdate(); break; }
        case ToolState::Arc: { OnArcToolUpdate(); break; }
        case ToolState::Circle: { OnCircleToolUpdate(); break; }
        case ToolState::Pen: { OnPenToolUpdate(); break; }
        case ToolState::Curve: { OnCurveUpdate(); break; }
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

        // Update

        m_Renderer.OnResize(m_CanvasWidth, m_CanvasHeight);
        m_Camera.SetViewportSize((float)m_CanvasWidth, (float)m_CanvasHeight);

        m_Renderer.BeginScene(m_Camera);
        Render();
        m_Renderer.EndScene();
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
        return false;
    }

    void EditorLayer::UpdateMousePosInCanvas()
    {
        auto [mx, my] = ImGui::GetMousePos();

        m_MousePosInCanvas = {
            mx - m_CanvasBounds[0].x,
            m_CanvasBounds[1].y - my
        };
    }

    bool EditorLayer::IsMouseInViewport() const
    {
        return m_MousePosInCanvas.x >= 0.0f && m_MousePosInCanvas.x < (float)m_CanvasWidth
            && m_MousePosInCanvas.y >= 0.0f && m_MousePosInCanvas.y < (float)m_CanvasHeight;
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

                Entity entity = m_EditorScene->CreateEntity("Line");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& line = entity.AddComponent<LineComponent>();

                line.P0 = m_MousePosInWorld;
                line.P1 = m_MousePosInWorld;
                line.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& line = entity.GetComponent<LineComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            // Press Shift key to draw horizontal/vertical lines
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
            {
                if (std::abs(m_MousePosInWorld.x - line.P0.x) < std::abs(m_MousePosInWorld.y - line.P0.y))
                    line.P1 = { line.P0.x, m_MousePosInWorld.y };
                else
                    line.P1 = { m_MousePosInWorld.x, line.P0.y };
            }
            else
            {
                line.P1 = m_MousePosInWorld;
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                m_IsDrawing = false;

                // If the line is too short, remove it
                if (glm::distance(line.P0, line.P1) < 0.1f)
                {
                    m_EditorScene->RemoveEntity(entity);
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

                Entity entity = m_EditorScene->CreateEntity("Arc");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& arc = entity.AddComponent<ArcComponent>();

                arc.Center = m_MousePosInWorld;
                arc.Radius = 0.0f;
                arc.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& arc = entity.GetComponent<ArcComponent>();

            arc.Color = m_PickedColor;

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            switch (m_OperationState)
            {
            case 0:
            {
                float radius = glm::distance(arc.Center, m_MousePosInWorld);

                float dx = m_MousePosInWorld.x - arc.Center.x;
                float dy = m_MousePosInWorld.y - arc.Center.y;
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
                        m_EditorScene->RemoveEntity(entity);
                        m_HierarchyPanel.SetSelectedEntity({});
                    }
                    else
                    {
                        ++m_OperationState;
                    }
                }
                break;
            }
            case 1:
            {
                float dx = m_MousePosInWorld.x - arc.Center.x;
                float dy = m_MousePosInWorld.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));
                arc.Angle2 = angle;

                // Press Ctrl key to draw a major arc
                if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                    arc.Major = true;
                else
                    arc.Major = false;

                // Confirm the arc
                if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    m_IsDrawing = false;
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

                Entity entity = m_EditorScene->CreateEntity("Circle");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& circle = entity.AddComponent<CircleComponent>();

                circle.Center = m_MousePosInWorld;
                circle.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& circle = entity.GetComponent<CircleComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            circle.Radius = glm::distance(circle.Center, m_MousePosInWorld);

            // Confirm the circle
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                m_IsDrawing = false;

                // If the radius is too small, remove the circle
                if (circle.Radius < 0.1f)
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnBucketToolUpdate()
    {
        if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            Entity entity = m_EditorScene->CreateEntity("Fill");
            m_HierarchyPanel.SetSelectedEntity(entity);

            auto& bucket = entity.AddComponent<SeedFillComponent>();
            bucket.SeedPoint = m_MousePosInCanvas;
            bucket.FillColor = m_PickedColor;
        }
    }

    void EditorLayer::OnPenToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_EditorScene->CreateEntity("Polygon");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& polygon = entity.AddComponent<PolygonComponent>();

                polygon.Vertices.push_back(m_MousePosInWorld);
                polygon.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& polygon = entity.GetComponent<PolygonComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                polygon.Vertices.push_back(m_MousePosInWorld);

            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                m_IsDrawing = false;

                if (polygon.Vertices.size() > 2)
                {
                    polygon.Vertices.push_back(polygon.Vertices[0]);
                }
                else
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnCurveUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_EditorScene->CreateEntity("Curve");
                m_HierarchyPanel.SetSelectedEntity(entity);

                auto& curve = entity.AddComponent<CurveComponent>();

                curve.ControlPoints.push_back(m_MousePosInWorld);
                curve.GenerateKnots();
                curve.RandomWeights();
                curve.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = m_HierarchyPanel.GetSelectedEntity();
            auto& curve = entity.GetComponent<CurveComponent>();

            // If ESC is pressed, cancel drawing
            if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SetSelectedEntity({});
                return;
            }

            if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                curve.ControlPoints.push_back(m_MousePosInWorld);
                curve.GenerateKnots();
                curve.RandomWeights();
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                m_IsDrawing = false;

                if (curve.ControlPoints.size() <= 1)
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SetSelectedEntity({});
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
            m_CanvasBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };  // Top-left
            m_CanvasBounds[1] = { m_CanvasBounds[0].x + m_CanvasWidth, m_CanvasBounds[0].y + m_CanvasHeight };  // Bottom-right

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused);

            if (m_CanvasWidth == 0 || m_CanvasWidth > s_MaxViewportSize || m_CanvasHeight == 0 || m_CanvasHeight > s_MaxViewportSize)
            {
                GF_WARN("Attempt to resize viewport to ({0}, {1})", m_CanvasWidth, m_CanvasHeight);
            }
            else
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

            if (ImGui::Button("Curve", ImVec2{ 80.0f, 30.0f }))
            {
                GF_INFO("Switched to curve tool.");
                m_ToolState = ToolState::Curve;
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

            glm::vec2 cameraPos = m_Camera.GetPosition();
            ImGui::Text("Camera Position: (%d, %d)", (int)cameraPos.x, (int)cameraPos.y);

            if (IsMouseInViewport())
            {
                glm::mat3 translationMatrix = m_Camera.GetTranslationMatrix();
                m_MousePosInWorld = Math::Transform(translationMatrix, m_MousePosInCanvas);
                ImGui::Text("Mouse Position In World: (%d, %d)", (int)m_MousePosInWorld.x, (int)m_MousePosInWorld.y);
                ImGui::Text("Mouse Position In Canvas: (%d, %d)", (int)m_MousePosInCanvas.x, (int)m_MousePosInCanvas.y);
            }
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
                }
                else if (selectedEntity.HasComponent<ArcComponent>())
                {
                    auto& arc = selectedEntity.GetComponent<ArcComponent>();
                    m_PickedColor = arc.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(arc.Color));
                }
                else if (selectedEntity.HasComponent<CircleComponent>())
                {
                    auto& circle = selectedEntity.GetComponent<CircleComponent>();
                    m_PickedColor = circle.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(circle.Color));
                }
                else if (selectedEntity.HasComponent<PolygonComponent>())
                {
                    auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
                    m_PickedColor = polygon.Color;
                    ImGui::ColorPicker3("Color", glm::value_ptr(polygon.Color));
                }
            }
            else
            {
                ImGui::ColorPicker3("Color", glm::value_ptr(m_PickedColor));
            }
        }
        ImGui::End();
    }

    void EditorLayer::BeginTransforming()
    {
        m_HierarchyPanel.SetTransforming();

        auto selectedEntity = m_HierarchyPanel.GetSelectedEntity();
        auto& transform = selectedEntity.GetComponent<TransformComponent>();

        if (selectedEntity.HasComponent<LineComponent>())
        {
            auto& line = selectedEntity.GetComponent<LineComponent>();
            transform.Pivot = line.GetCenterOfGravity();
        }
        else if (selectedEntity.HasComponent<CircleComponent>())
        {
            auto& circle = selectedEntity.GetComponent<CircleComponent>();
            transform.Pivot = circle.GetCenterOfGravity();
        }
        else if (selectedEntity.HasComponent<PolygonComponent>())
        {
            auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
            glm::vec2 referencePoint = glm::vec2{ 0.0f, 0.0f };
            for (auto& vertex : polygon.Vertices)
                referencePoint += vertex;
            transform.Pivot = referencePoint / (float)polygon.Vertices.size();
        }
    }
}