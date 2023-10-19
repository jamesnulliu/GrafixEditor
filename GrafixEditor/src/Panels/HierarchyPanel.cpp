#include "HierarchyPanel.h"
#include "misc/cpp/imgui_stdlib.h"
#include "misc/cpp/imgui_stdlib.cpp"

namespace Grafix
{
    void HierarchyPanel::OnUIRender()
    {
        if (m_IsTransforming)
            UI_Transformation();
        else
            UI_Properties();

        UI_Hierarchy();
    }

    void HierarchyPanel::UI_Hierarchy()
    {
        ImGui::Begin("Hierarchy");
        m_Scene->m_Registry.each([&](auto entityID) {
            DrawItem(Entity{ entityID, m_Scene.get() });
            }
        );

        // Left-click on blank space to unselect
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_SelectedEntity = {};
        // Press Delete to delete selected entity
        if (m_SelectedEntity && ImGui::IsKeyPressed(ImGuiKey_Delete))
            ImGui::OpenPopup("Warning");

        m_IsModalOpen = ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (m_IsModalOpen)
        {
            ImGui::Text("Delete this entity?");

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_Scene->RemoveEntity(m_SelectedEntity);
                m_SelectedEntity = {};
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
        ImGui::End();
    }

    void HierarchyPanel::UI_Properties()
    {
        ImGui::Begin("Properties");
        if (!m_SelectedEntity)
        {
            ImGui::End();
            return;
        }

        ImGui::Text("Tag");
        ImGui::SameLine();
        auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
        m_TagBuffer = tag;

        ImGui::InputText("##Tag", &m_TagBuffer);
        if (!m_TagBuffer.empty())
            tag = m_TagBuffer;

        ImGui::Separator();

        if (m_SelectedEntity.HasComponent<LineComponent>())
        {
            auto& line = m_SelectedEntity.GetComponent<LineComponent>();
            std::vector<std::string> lineAlgorithmStrings = { "Midpoint", "Bresenham" };
            DrawAlgorithmControl(lineAlgorithmStrings, line.Algorithm);
            ImGui::Separator();
            DrawFloat2Control("Point A", &line.P0.x, &line.P0.y);
            DrawFloat2Control("Point B", &line.P1.x, &line.P1.y);
            ImGui::Separator();
            DrawLineWidthAndStyleControl(&line.LineWidth, line.LineStyle);
            ImGui::Separator();
            DrawColorControl(line.Color);
        }
        else if (m_SelectedEntity.HasComponent<CircleComponent>())
        {
            auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();
            DrawFloat2Control("Center", &circle.Center.x, &circle.Center.y);
            DrawFloatControl("Radius", &circle.Radius, 0.0f);
            ImGui::Separator();
            DrawLineWidthAndStyleControl(&circle.LineWidth, circle.LineStyle);
            ImGui::Separator();
            DrawColorControl(circle.Color);
        }
        else if (m_SelectedEntity.HasComponent<ArcComponent>())
        {
            auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();
            DrawFloat2Control("Center", &arc.Center.x, &arc.Center.y);
            DrawFloatControl("Radius", &arc.Radius, 0.0f);
            DrawFloatControl("Angle 1", &arc.Angle1);
            DrawFloatControl("Angle 2", &arc.Angle2);
            ImGui::Checkbox("Major", &arc.Major);
            ImGui::Separator();
            DrawLineWidthAndStyleControl(&arc.LineWidth, arc.LineStyle);
            ImGui::Separator();
            DrawColorControl(arc.Color);
        }
        else if (m_SelectedEntity.HasComponent<PolygonComponent>())
        {
            auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();
            for (int i = 0; i < polygon.Vertices.size(); ++i)
            {
                ImGui::PushID(i);
                std::string label = "Vertex " + std::to_string(i + 1);
                DrawFloat2Control(label, &polygon.Vertices[i].x, &polygon.Vertices[i].y);
                ImGui::PopID();
            }
            ImGui::Separator();
            DrawColorControl(polygon.Color);
        }
        else if (m_SelectedEntity.HasComponent<CurveComponent>())
        {
            auto& curve = m_SelectedEntity.GetComponent<CurveComponent>();
            std::vector<std::string> curveAlgorithmStrings = { "Bezier", "NURBS" };
            DrawAlgorithmControl(curveAlgorithmStrings, curve.Algorithm);
            ImGui::Separator();
            for (int i = 0; i < curve.ControlPoints.size(); ++i)
            {
                ImGui::PushID(i);
                std::string label = "Ctrl Point " + std::to_string(i + 1);
                DrawFloat2Control(label, &curve.ControlPoints[i].x, &curve.ControlPoints[i].y);
                ImGui::PopID();
            }
            ImGui::Separator();
            if (curve.Algorithm == CurveAlgorithmType::NURBS)
                DrawIntControl("Order", &curve.Order, 2, curve.ControlPoints.size());
            DrawFloatControl("Step", &curve.Step, 0.0005f, 0.03f, "%.4f", 0.0001f);
            ImGui::Separator();
            DrawLineWidthAndStyleControl(&curve.LineWidth, curve.LineStyle);
            ImGui::Separator();
            DrawColorControl(curve.Color);
        }
        ImGui::End();
    }

    void HierarchyPanel::UI_Transformation()
    {
        ImGui::Begin("Transformation");
        auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
        ImGui::Text("Transform");

        DrawFloat2Control("Pivot Point", transform.Pivot);
        DrawFloat2Control("Position", transform.Translation);
        DrawFloatControl("Rotation", &transform.Rotation);

        if (m_SelectedEntity.HasAnyOfComponents<CircleComponent, ArcComponent>())
        {
            DrawFloatControl("Scale", &transform.Scale.x, 0.0f);
            transform.Scale.y = transform.Scale.x;
        }
        else
        {
            if (transform.KeepRatio)
            {
                DrawFloatControl("Scale", &transform.Scale.x, 0.0f);
                transform.Scale.y = transform.Scale.x;
            }
            else
            {
                DrawFloat2Control("Scale", transform.Scale, 0.0f);
            }

            ImGui::SameLine();
            ImGui::Checkbox("Keep Ratio", &transform.KeepRatio);
        }

        if (ImGui::Button("Apply"))
        {
            auto transformMatrix = m_SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();

            if (m_SelectedEntity.HasComponent<LineComponent>())
            {
                auto& line = m_SelectedEntity.GetComponent<LineComponent>();

                line.P0 = Math::Transform(transformMatrix, line.P0);
                line.P1 = Math::Transform(transformMatrix, line.P1);
            }
            else if (m_SelectedEntity.HasComponent<CircleComponent>())
            {
                auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

                circle.Center = Math::Transform(transformMatrix, circle.Center);
                circle.Radius = circle.Radius * transform.Scale.x;
            }
            else if (m_SelectedEntity.HasComponent<PolygonComponent>())
            {
                auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();

                for (glm::vec2& vertex : polygon.Vertices)
                    vertex = Math::Transform(transformMatrix, vertex);
            }

            transform = TransformComponent();
            m_IsTransforming = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            transform = TransformComponent();
            m_IsTransforming = false;
        }

        ImGui::End();
    }

    void HierarchyPanel::DrawItem(Entity entity)
    {
        bool selected = m_SelectedEntity == entity;
        ImGui::Selectable(entity.GetTag().c_str(), &selected);
        if (ImGui::IsItemClicked())
            m_SelectedEntity = entity;
    }

    // *********************************************************************
    // *                      Control Panel Templates                      *
    // *********************************************************************

    void HierarchyPanel::DrawFloatControl(const std::string& label, float* value, float vMin, float vMax, const char* format, float speed, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::DragFloat("##Value", value, speed, vMin, vMax, format);

        ImGui::Columns(1);

        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat2Control(const std::string& label, float* x, float* y, float vMin, float vMax, const char* format, float speed, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, speed, vMin, vMax, format);

        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat2Control(const std::string& label, glm::vec2& values, float vMin, float vMax, const char* format, float speed, float columnWidth)
    {
        DrawFloat2Control(label, &values.x, &values.y, vMin, vMax, format, speed, columnWidth);
    }

    void HierarchyPanel::DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float vMin, float vMax, const char* format, float speed, float columnWidth)
    {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::DragFloat("##Z", z, speed, vMin, vMax, format);

        ImGui::PopItemWidth();
        ImGui::Columns(1);
        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat3Control(const std::string& label, glm::vec3& values, float vMin, float vMax, const char* format, float speed, float columnWidth)
    {
        DrawFloat3Control(label, &values.x, &values.y, &values.z, vMin, vMax, format, speed, columnWidth);
    }

    void HierarchyPanel::DrawIntControl(const std::string& label, int* value, int vMin, int vMax, float columnWidth)
    {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::DragInt("##Value", value, 1, vMin, vMax);
        ImGui::Columns(1);
        ImGui::PopID();
    }

    void HierarchyPanel::DrawColorControl(glm::vec3& color)
    {
        ImGui::Text("Color");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Color", glm::value_ptr(color));
    }

    void HierarchyPanel::DrawLineWidthAndStyleControl(float* lineWidth, LineStyleType& lineStyle)
    {
        DrawFloatControl("Width", lineWidth, 1.0f, 50.0f);

        const char* lineStyleStrings[] = { "Solid", "Dashed", "Dotted" };
        const char* currentLineStyleString = lineStyleStrings[(int)lineStyle];
        ImGui::Text("Line Style");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Line Style", currentLineStyleString))
        {
            for (int i = 0; i < 3; ++i)
            {
                bool isSelected = currentLineStyleString == lineStyleStrings[i];
                if (ImGui::Selectable(lineStyleStrings[i], isSelected))
                {
                    currentLineStyleString = lineStyleStrings[i];
                    lineStyle = (LineStyleType)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void HierarchyPanel::DrawAlgorithmControl(const std::vector<std::string>& algorithmStrings, auto& algorithm)
    {
        const char* currentAlgorithmString = algorithmStrings[(int)algorithm].c_str();

        ImGui::Text("Algorithm");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Algorithm", currentAlgorithmString))
        {
            for (int i = 0; i < algorithmStrings.size(); ++i)
            {
                bool isSelected = currentAlgorithmString == algorithmStrings[i];
                if (ImGui::Selectable(algorithmStrings[i].c_str(), isSelected))
                {
                    currentAlgorithmString = algorithmStrings[i].c_str();
                    algorithm = decltype(algorithm)(i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}