#include "pch.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace GE
{
    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %v%$");
        s_Logger = spdlog::stdout_color_mt("App");
        s_Logger->set_level(spdlog::level::trace);
    }
}