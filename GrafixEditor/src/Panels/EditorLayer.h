#pragma once

#include "Grafix.h"

#include "Grafix/Renderer/Image.h"

class EditorLayer : public Grafix::Layer
{
public:
    EditorLayer() : Layer("Editor Layer") {}
    virtual ~EditorLayer() = default;

    void Render(Grafix::Scene& scene);
    virtual void OnImGuiRender() override;
private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    bool m_ViewportFocused = false, m_ViewportHovered = false;

    Grafix::Scene m_Scene;

    Grafix::Renderer m_Renderer;
};