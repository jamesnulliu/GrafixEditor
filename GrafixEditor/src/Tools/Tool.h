#pragma once

#include "Grafix.h"

namespace Grafix
{
    class Tool
    {
    public:
        Tool();
        virtual ~Tool() = default;

        virtual void OnUpdate() = 0;
    protected:
        virtual void OnKeyPressed(KeyPressedEvent& e) = 0;
        virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) = 0;
    };
}
