#pragma once

#include <spdlog/spdlog.h>

namespace Grafix
{
    class Log final
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}

// Using macros to log

#define GF_TRACE(...) ::Grafix::Log::GetLogger()->trace(__VA_ARGS__);
#define GF_INFO(...) ::Grafix::Log::GetLogger()->info(__VA_ARGS__);
#define GF_WARN(...) ::Grafix::Log::GetLogger()->warn(__VA_ARGS__);
#define GF_ERROR(...) ::Grafix::Log::GetLogger()->error(__VA_ARGS__);
#define GF_CRITICAL(...) ::Grafix::Log::GetLogger()->critical(__VA_ARGS__);
