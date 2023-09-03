#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace GE
{
    class Log
    {
    public:
        static void Init();
        static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}

#define GRAPHICS_INFO(...) GE::Log::Getlogger()->info(__VA_ARG__);
