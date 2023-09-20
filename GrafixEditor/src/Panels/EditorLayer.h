#pragma once

#include "Grafix.h"

#include "Grafix/Renderer/Image.h"

class EditorLayer final : public Grafix::Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    void Render(Grafix::Scene& scene);
    virtual void OnUpdate() override;
    virtual void OnUIRender() override;
    virtual void OnEvent(Grafix::Event& e) override;
private:
    bool OnKeyPressed(Grafix::KeyPressedEvent& e);
    bool OnMouseButtonPressed(Grafix::MouseButtonPressedEvent& e);

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

    Grafix::Scene m_Scene;
    Grafix::Renderer m_Renderer;

    enum class ToolState : uint8_t
    {
        Move = 0,
        Brush, Eraser,
        Pen,
        Line, Arc,
        Rectangle, Circle
    };
    ToolState m_ToolState = ToolState::Move;
};