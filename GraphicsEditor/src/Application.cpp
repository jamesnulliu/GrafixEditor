#include "Application.h"

#include <imgui.h>

// TODO: Remove this.
void ImGuiExample();

namespace Graphics
{
    void Application::Run() const
    {
        ImGuiExample();
    }
}