#pragma once

#include <chrono>

namespace Grafix
{
    class Stopwatch
    {
    public:
        Stopwatch();
        ~Stopwatch();

        void Stop();
        void Reset();

        float GetMilliseconds();
        float GetSeconds();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartingPoint;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_EndingPoint;
        bool m_IsStopped = false;
    };
}
