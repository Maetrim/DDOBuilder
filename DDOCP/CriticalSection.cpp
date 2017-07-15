// CriticalSection.cpp
//
// Implementation of critical section wrapper
#include "StdAfx.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
    InitializeCriticalSection(&m_criticalSection);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&m_criticalSection);
}

//////////////////////////////////////////////////////////////////////

// Take a lock on the critical section. This will be released by the destructor.
CriticalSectionLock::CriticalSectionLock(const CriticalSection * critSec) :
    m_pCriticalSection(critSec)
{
    EnterCriticalSection(&(m_pCriticalSection->m_criticalSection));
}

// Release a lock on the critical section.
CriticalSectionLock::~CriticalSectionLock()
{
    LeaveCriticalSection(&(m_pCriticalSection->m_criticalSection));
}

