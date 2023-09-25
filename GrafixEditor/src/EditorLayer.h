#pragma once

#include "Grafix.h"

namespace Grafix
{
    class EditorLayer final : public Layer
    {
    private:
        enum class ToolState : uint8_t
        {
            Move = 0,
            Pen,
            Line, Arc,
            Rectangle, Circle
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

        void CalculateMousePosInViewport();
        bool IsMouseInViewport() const;

        void UI_MenuBar();
        void UI_Viewport();
        void UI_Toolbar();
        void UI_Information();
        void UI_Settings();
        void UI_Entities();
    private:
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_ViewportBounds[2];
        bool m_ViewportFocused = false, m_ViewportHovered = false;

        glm::i32vec2 m_MousePosInViewport{ 0, 0 };

        std::shared_ptr<Scene> m_EditorScene = nullptr;
        std::shared_ptr<Scene> m_ActiveScene = nullptr;

        EditorCamera m_EditorCamera;
        Renderer m_Renderer;

        ToolState m_ToolState = ToolState::Move;
    };
}