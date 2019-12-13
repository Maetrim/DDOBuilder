// ElapsedTimer.h
//

#pragma once

class ElapsedTimer
{
    public:
        ElapsedTimer();

        void Reset();
        operator double () const; // milliseconds since last reset

        void Pause();
        void Resume();
        bool IsPaused() const;

    private:
        LARGE_INTEGER m_frequency;
        LARGE_INTEGER m_start;
        bool m_paused;
        LARGE_INTEGER m_previous;
};
