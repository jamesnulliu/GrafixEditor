#pragma once

#include "Grafix.h"

class EditorLayer : public Grafix::Layer
{
public:
    EditorLayer() : Layer("Editor Layer") {}
    virtual ~EditorLayer() = default;

    virtual void OnImGuiRender() override;
private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};