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

        void SetThemeColor();

        void Begin();
        void End();
    };
}
