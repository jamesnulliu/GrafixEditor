#include "pch.h"

namespace GE
{
    std::shared_ptr<spdlog::logger> Log::s_Logger{};

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %v%$");
        s_Logger->set_level(spdlog::level::trace);
    }
}