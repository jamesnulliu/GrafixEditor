#pragma once

#include <spdlog/spdlog.h>

namespace GE
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

#define GE_TRACE(...) ::GE::Log::GetLogger()->trace(__VA_ARGS__);
#define GE_DEBUG(...) ::GE::Log::GetLogger()->debug(__VA_ARGS__);
#define GE_INFO(...) ::GE::Log::GetLogger()->info(__VA_ARGS__);
#define GE_WARN(...) ::GE::Log::GetLogger()->warn(__VA_ARGS__);
#define GE_ERROR(...) ::GE::Log::GetLogger()->error(__VA_ARGS__);
#define GE_CRITICAL(...) ::GE::Log::GetLogger()->critical(__VA_ARGS__);
