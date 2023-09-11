#include "pch.h"
#include "Stopwatch.h"

namespace Grafix
{
    Stopwatch::Stopwatch()
    {
        m_StartingPoint = std::chrono::high_resolution_clock::now();
        m_IsStopped = false;
    }

    Stopwatch::~Stopwatch()
    {
        if (m_IsStopped == false)
        {
            Stop();
        }
    }

    void Stopwatch::Stop()
    {
        m_IsStopped = true;
        m_EndingPoint = std::chrono::high_resolution_clock::now();
    }

    void Stopwatch::Reset()
    {
        m_IsStopped = false;
        m_StartingPoint = std::chrono::high_resolution_clock::now();
    }

    float Stopwatch::GetMilliseconds()
    {
        if (m_IsStopped == false)
        {
            Stop();
        }

        float duration = (float)std::chrono::duration_cast<std::chrono::nanoseconds>(m_EndingPoint - m_StartingPoint).count();
        return duration * 0.001f * 0.001f;
    }

    float Stopwatch::GetSeconds()
    {
        return GetMilliseconds() * 0.001f;
    }
}