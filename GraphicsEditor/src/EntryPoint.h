#pragma once

#include "Application.h"

#ifdef GRAPHICS_PLATFORM_WINDOWS

extern Graphics::Application* Graphics::CreateApplication();

int main()
{
    Graphics::Application* app = Graphics::CreateApplication();
    app->Run();
    delete app;
}

#endif
