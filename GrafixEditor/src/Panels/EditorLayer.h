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
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Grafix::Event& e) override;
private:
    bool OnKeyPressed(Grafix::KeyPressedEvent& e);
    bool OnMouseButtonPressed(Grafix::MouseButtonPressedEvent& e);

    void UI_MenuBar();
    void UI_Viewport();
    void UI_Toolbar();
    void UI_Settings();
    void UI_Entities();
private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;

    int m_MouseXInViewport = 0, m_MouseYInViewport = 0;

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

    struct Line
    {
        glm::vec2 BeginPos{ 0.0f, 0.0f };
        glm::vec2 EndPos{ 0.0f, 0.0f };

        glm::vec4 Color = { 0.8f, 0.3f, 0.2f, 1.0f };
        float Width = 1.0f;

        enum class LineStyle
        {
            Solid, Dot, Dash, DashDot
        };
        LineStyle Style = LineStyle::Solid;
    };
    Line m_Line;
};