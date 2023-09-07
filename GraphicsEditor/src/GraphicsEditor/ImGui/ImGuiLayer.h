#pragma once

#include "GraphicsEditor/Core/Layer.h"

namespace GE
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
