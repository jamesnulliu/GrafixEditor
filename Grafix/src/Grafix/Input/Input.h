#pragma once

#include "Codes.h"
#include <glm/glm.hpp>

namespace Grafix
{
    class Input
    {
    public:
        static bool IsMouseButtonPressed(MouseButton button);
        static glm::vec2 GetMousePosition();
        static void SetCursorMode(CursorMode mode);

        static bool IsKeyPressed(Key keycode);
    };
}
