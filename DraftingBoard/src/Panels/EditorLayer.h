#pragma once

#include "GraphicsEditor.h"

class EditorLayer : public GE::Layer
{
public:
    EditorLayer() : Layer("Drafting Board Layer") {}
    virtual ~EditorLayer() = default;

    virtual void OnImGuiRender() override;
private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};