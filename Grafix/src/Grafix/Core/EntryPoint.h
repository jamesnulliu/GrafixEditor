#pragma once

#include "Application.h"

#ifdef GF_WINDOWS

extern Grafix::Application* Grafix::CreateApplication();

int main()
{
    Grafix::Log::Init();

    Grafix::Application* app = Grafix::CreateApplication();
    app->Run();
    delete app;
}

#endif
