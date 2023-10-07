#include "HierarchyPanel.h"

namespace Grafix
{
    void HierarchyPanel::OnUIRender()
    {
        UI_Properties();
        UI_Hierarchy();
    }

    void HierarchyPanel::UI_Hierarchy()
    {
        ImGui::Begin("Hierarchy");

        m_Scene->m_Registry.each([&](auto entityID)
            {
                DrawItem(Entity{ entityID, m_Scene.get() });
            }
        );

        // Left-click on blank space to unselect
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_SelectedEntity = {};

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
        {
            if (m_SelectedEntity)
            {
                if (m_SelectedEntity.HasComponent<TransformComponent>())
                {
                    auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
                    ImGui::Text("Transform");

                    DrawFloat3Control("Pivot Point", transform.PivotPoint);

                    DrawFloat3Control("Position", transform.Translation);

                    if (m_SelectedEntity.HasAnyOfComponents<CircleComponent, ArcComponent>())
                    {
                        DrawFloatControl("Rotation", &transform.Rotation.z);
                        DrawFloatControl("Scale", &transform.Scale.x, 0.0f);
                        transform.Scale.y = transform.Scale.x;
                    } else
                    {
                        DrawFloat3Control("Rotation", transform.Rotation);

                        if (transform.KeepRatio)
                        {
                            DrawFloatControl("Scale", &transform.Scale.x, 0.0f);
                            transform.Scale.y = transform.Scale.x;
                        } else
                        {
                            DrawFloat3Control("Scale", transform.Scale, 0.0f);
                        }

                        ImGui::SameLine();
                        ImGui::Checkbox("Keep Ratio", &transform.KeepRatio);
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
                        } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
                        {
                            auto& polygon = m_SelectedEntity.GetComponent<PolygonComponent>();
                            auto transform = m_SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();

                            for (glm::vec3& vertex : polygon.Vertices)
                                vertex = transform * glm::vec4(vertex, 1.0f);
                        }
                        m_SelectedEntity.RemoveComponent<TransformComponent>();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                        m_SelectedEntity.RemoveComponent<TransformComponent>();
                } else
                {
                    if (m_SelectedEntity.HasComponent<TagComponent>())
                    {
                        ImGui::Text("Tag");
                        ImGui::SameLine();
                        auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;

                        char buffer[256];
                        memset(buffer, 0, sizeof(buffer));
                        strcpy_s(buffer, sizeof(buffer), tag.c_str());

                        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                            tag = std::string(buffer);

                        ImGui::Separator();
                    }

                    if (m_SelectedEntity.HasComponent<LineComponent>())
                    {
                        auto& line = m_SelectedEntity.GetComponent<LineComponent>();

                        DrawFloat2Control("Point A", &line.P0.x, &line.P0.y);
                        DrawFloat2Control("Point B", &line.P1.x, &line.P1.y);
                        DrawFloatControl("Width", &line.Width, 0.0f);

                        const char* lineStyleStrings[] = { "Solid", "Dashed" };
                        const char* currentLineStyleString = lineStyleStrings[(int)line.Style];

                        ImGui::Text("Line Style");
                        ImGui::SameLine();
                        if (ImGui::BeginCombo("##Line Style", currentLineStyleString))
                        {
                            for (int i = 0; i < 2; ++i)
                            {
                                bool isSelected = currentLineStyleString == lineStyleStrings[i];
                                if (ImGui::Selectable(lineStyleStrings[i], isSelected))
                                {
                                    currentLineStyleString = lineStyleStrings[i];
                                    line.Style = (LineStyle)i;
                                }

                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        if (line.Style == LineStyle::Dashed)
                            DrawFloatControl("Dash Length", &line.DashLength, 1.0f);

                        ImGui::Separator();

                        ImGui::Text("Color");
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##Color", glm::value_ptr(line.Color));
                    } else if (m_SelectedEntity.HasComponent<CircleComponent>())
                    {
                        auto& circle = m_SelectedEntity.GetComponent<CircleComponent>();

                        DrawFloat2Control("Center", &circle.Center.x, &circle.Center.y);
                        DrawFloatControl("Radius", &circle.Radius, 0.0f);

                        ImGui::Checkbox("Show Center", &circle.ShowCenter);

                        ImGui::Separator();

                        ImGui::Text("Color");
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##Color", glm::value_ptr(circle.Color));
                    } else if (m_SelectedEntity.HasComponent<ArcComponent>())
                    {
                        auto& arc = m_SelectedEntity.GetComponent<ArcComponent>();

                        DrawFloat2Control("Center", &arc.Center.x, &arc.Center.y);
                        DrawFloatControl("Radius", &arc.Radius, 0.0f);
                        DrawFloatControl("Angle 1", &arc.Angle1);
                        DrawFloatControl("Angle 2", &arc.Angle2);
                        ImGui::Checkbox("Major", &arc.Major);

                        ImGui::Checkbox("Show Center", &arc.ShowCenter);

                        ImGui::Separator();

                        ImGui::Text("Color");
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##Color", glm::value_ptr(arc.Color));
                    } else if (m_SelectedEntity.HasComponent<PolygonComponent>())
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

                        ImGui::Text("Color");
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##Color", glm::value_ptr(polygon.Color));
                    }
                }
            }
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

    void HierarchyPanel::DrawFloatControl(const std::string& label, float* value, float vMin, float vMax, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::DragFloat("##Value", value, 0.5f, vMin, vMax);

        ImGui::Columns(1);

        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat2Control(const std::string& label, float* x, float* y, float vMin, float vMax, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, 0.5f, vMin, vMax);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, 0.5f, vMin, vMax);

        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat2Control(const std::string& label, glm::vec2& values, float vMin, float vMax, float columnWidth)
    {
        DrawFloat2Control(label, &values.x, &values.y, vMin, vMax, columnWidth);
    }

    void HierarchyPanel::DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float vMin, float vMax, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, 0.5f, vMin, vMax);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, 0.5f, vMin, vMax);

        ImGui::SameLine();

        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::DragFloat("##Z", z, 0.5f, vMin, vMax);

        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    void HierarchyPanel::DrawFloat3Control(const std::string& label, glm::vec3& values, float vMin, float vMax, float columnWidth)
    {
        DrawFloat3Control(label, &values.x, &values.y, &values.z, vMin, vMax, columnWidth);
    }
}