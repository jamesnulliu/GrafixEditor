#pragma once

#include "Grafix.h"

namespace Grafix
{
    constexpr static float s_FloatMin = -std::numeric_limits<float>::max();
    constexpr static float s_FloatMax = std::numeric_limits<float>::max();

    class HierarchyPanel
    {
    public:
        HierarchyPanel() = default;
        ~HierarchyPanel() = default;

        void BindScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene; }

        Entity GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

        void OnUIRender();
    private:
        void DrawItem(Entity entity);

        void UI_Hierarchy();
        void UI_Properties();

        void DrawFloatControl(const std::string& label, float* value,
            float vMin = s_FloatMin, float vMax = s_FloatMax, float columnWidth = 100.0f);

        void DrawFloat2Control(const std::string& label, glm::vec2& values, float vMin = s_FloatMin, float vMax = s_FloatMax, float columnWidth = 100.0f);
        void DrawFloat2Control(const std::string& label, float* x, float* y, float vMin = s_FloatMin, float vMax = s_FloatMax, float columnWidth = 100.0f);

        void DrawFloat3Control(const std::string& label, glm::vec3& values, float vMin = s_FloatMin, float vMax = s_FloatMax, float columnWidth = 100.0f);
        void DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float vMin = s_FloatMin, float vMax = s_FloatMax, float columnWidth = 100.0f);
    private:
        Entity m_SelectedEntity{};
        std::shared_ptr<Scene> m_Scene = nullptr;

        bool m_IsModalOpen = false;
    };
}
