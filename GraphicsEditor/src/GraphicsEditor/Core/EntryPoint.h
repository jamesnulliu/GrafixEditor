#pragma once

#include "Application.h"

#ifdef GE_WINDOWS

extern GE::Application* GE::CreateApplication();

int main()
{
    GE::Application* app = GE::CreateApplication();
    app->Run();
    delete app;
}

#endif
