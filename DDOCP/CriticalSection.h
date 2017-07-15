// CriticalSection.h
//
// This is a wrapper for the operating system critical section
// objects. These allow a resource to be protected so that only
// one object can use it at once (lock the critical section).
// The lock class allows a lock to be taken, and then dropped
// automatically when the lock goes out of scope (is destroyed).
// This dropping of locks is particularly important for
// exceptions handling, so that the lock is dropped if an
// exception occurs and the normal flow of control is not followed.

#pragma once
#include <windows.h>

class CriticalSection
{
    public:
        CriticalSection();
        ~CriticalSection();

    private:
        // cannot be copied or assigned
        CriticalSection(const CriticalSection & other);
        const CriticalSection & operator=(const CriticalSection & other);

        mutable CRITICAL_SECTION m_criticalSection;

        friend class CriticalSectionLock;
};

class CriticalSectionLock
{
    public:
        CriticalSectionLock(const CriticalSection * critSec);
        virtual ~CriticalSectionLock();

    private:
        // cannot be copied or assigned
        CriticalSectionLock(const CriticalSectionLock & other);
        const CriticalSectionLock & operator=(const CriticalSectionLock & other);

        const CriticalSection * m_pCriticalSection;
};
