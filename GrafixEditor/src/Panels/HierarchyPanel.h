#pragma once

#include "Grafix.h"

namespace Grafix
{
    class HierarchyPanel
    {
    public:
        HierarchyPanel() = default;
        ~HierarchyPanel() = default;

        void BindScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene; }

        Entity GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

        void SetTransforming() { m_IsTransforming = true; }
        bool IsTransforming() const { return m_IsTransforming; }

        void OnUIRender();
    private:
        void DrawItem(Entity entity);
        void DrawColorControl(glm::vec3& values);
        void DrawLineWidthAndStyleControl(float* width, LineStyleType& lineStyle);
        void DrawAlgorithmControl(const std::vector<std::string>& algorithmStrings, auto& lineAlgorithm);

        void UI_Hierarchy();
        void UI_Properties();
        void UI_Transformation();
    private:
        void DrawFloatControl(const std::string& label, float* value, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f);

        void DrawFloat2Control(const std::string& label, glm::vec2& values, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f);
        void DrawFloat2Control(const std::string& label, float* x, float* y, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f);

        void DrawFloat3Control(const std::string& label, glm::vec3& values, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f);
        void DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f);

        void DrawIntControl(const std::string& label, int* value, int vMin = -INT_MIN, int vMax = INT_MAX, float columnWidth = 100.0f);
    private:
        Entity m_SelectedEntity{};
        std::shared_ptr<Scene> m_Scene = nullptr;

        bool m_IsTransforming = false;
        bool m_IsModalOpen = false;
        std::string m_TagBuffer;
    };
}
