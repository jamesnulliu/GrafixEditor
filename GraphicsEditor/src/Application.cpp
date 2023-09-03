#include "Application.h"

#include <imgui.h>

// TODO: Remove this.
void ImGuiExample();

namespace GE
{
    void Application::Run() const
    {
        ImGuiExample();
    }
}