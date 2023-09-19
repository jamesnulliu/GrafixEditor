#pragma once

#include "Grafix/Core/Layer.h"

namespace Grafix
{
    class ImGuiLayer final : public Layer
    {
    public:
        ImGuiLayer() : Layer("ImGui Layer") {}
        ~ImGuiLayer() {}

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        void BeginFrame();
        void EndFrame();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void SetThemeColor();
    private:
        bool m_BlockEvents = true;
    };
}
