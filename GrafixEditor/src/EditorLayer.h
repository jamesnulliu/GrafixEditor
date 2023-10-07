#pragma once

#include "Grafix.h"

#include "Panels/HierarchyPanel.h"

namespace Grafix
{
    class EditorLayer final : public Layer
    {
    private:
        enum class ToolState : uint8_t
        {
            Move = 0,
            Bucket,
            Line, Arc,
            Circle,
            Pen
        };
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnUpdate() override;
        virtual void OnUIRender() override;
        virtual void OnEvent(Event& e) override;

        void Render();
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void UpdateMousePosInViewport();
        bool IsMouseInViewport() const;

        void OnPenToolUpdate();
        void OnLineToolUpdate();
        void OnArcToolUpdate();
        void OnCircleToolUpdate();

        void UI_MenuBar();
        void UI_Viewport();
        void UI_Toolbar();
        void UI_Info();
        void UI_Color();

        void BeginTransforming();
    private:
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_ViewportBounds[2];
        bool m_ViewportFocused = false, m_ViewportHovered = false;

        glm::i32vec2 m_MousePosInViewport{ 0, 0 };

        std::shared_ptr<Scene> m_ActiveScene = nullptr;

        Renderer m_Renderer;
        std::shared_ptr<Scene> m_EditorScene = nullptr;
        EditorCamera m_EditorCamera;

        ToolState m_ToolState = ToolState::Move;
        bool m_IsDrawing = false;
        int m_OperationState = 1;

        glm::vec3 m_PickedColor{ 0.9f, 0.9f, 0.9f };

        HierarchyPanel m_HierarchyPanel;
    };
}