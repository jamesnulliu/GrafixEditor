#pragma once

#include "Grafix.h"

#include "Grafix/Renderer/Image.h"

class EditorLayer : public Grafix::Layer
{
public:
    EditorLayer() : Layer("Editor Layer") {}
    virtual ~EditorLayer() = default;

    virtual void OnImGuiRender() override;
private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    std::shared_ptr<Grafix::Image> m_FinalImage = nullptr;
    uint32_t* m_Pixels = nullptr;

    ////glm::vec4 m_BackgroundColor{ 0.633f, 0.734f, 0.76f, 1.0f };
    ////glm::vec4 m_SquareColor{ 0.271f, 0.427f, 0.461f, 1.0f };
    glm::vec3 m_BackgroundColor{ 0.633f, 0.734f, 0.76f };
    glm::vec3 m_SquareColor{ 0.271f, 0.427f, 0.461f };
};