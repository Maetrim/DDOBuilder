// ElapsedTimer.cpp
//

#include "stdafx.h"
#include "ElapsedTimer.h"

ElapsedTimer::ElapsedTimer() :
    m_paused(false)
{
    QueryPerformanceFrequency(&m_frequency);
    Reset();
}

void ElapsedTimer::Reset()
{
    QueryPerformanceCounter(&m_start);
    m_previous.QuadPart = 0;
}

ElapsedTimer::operator double () const // milliseconds since last reset
{
    LARGE_INTEGER elapsed;
    if (m_paused)
    {
        elapsed.QuadPart = m_previous.QuadPart;
    }
    else
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        elapsed.QuadPart = (now.QuadPart - m_start.QuadPart + m_previous.QuadPart);
    }
    return elapsed.QuadPart * 1000.0 / m_frequency.QuadPart;
}

void ElapsedTimer::Pause()
{
    m_paused = true;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    m_previous.QuadPart += (now.QuadPart - m_start.QuadPart);
}

void ElapsedTimer::Resume()
{
    m_paused = false;
    QueryPerformanceCounter(&m_start);
}

bool ElapsedTimer::IsPaused() const
{
    return m_paused;
}
