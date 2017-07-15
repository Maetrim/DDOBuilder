// ObserverSubject.cpp
//
// Implementation of the Observer-Subject pattern
#include "stdafx.h"
#include "ObserverSubject.h"

//////////////////////////////////////////////////////////////////////

ObserverBase::ObserverBase()
{
}

ObserverBase::~ObserverBase()
{
    DetachAll();
}

// a copied observer DOES observe all the original's subjects
ObserverBase::ObserverBase(const ObserverBase & copy)
{
    AttachAll(copy);
}

ObserverBase & ObserverBase::operator =(const ObserverBase & copy)
{
    DetachAll();
    AttachAll(copy);
    return *this;
}

bool ObserverBase::InsertSubjectBase(SubjectBase * subject)
{
    CriticalSectionLock lock(&m_critsec);

    bool alreadyPresent = false;

    // If it is not already present, add to the beginning of the list
    for (SubjectList::iterator it = m_subjects.begin();
            it != m_subjects.end() && !alreadyPresent;
            it++)
    {
        if (*it == subject) 
        { 
            alreadyPresent = true;
        }
    }

    if (!alreadyPresent)
    {
        m_subjects.push_back(subject);
    }

    // Turn the logic upside down.
    bool hasBeenAdded = !alreadyPresent;
    return hasBeenAdded;
}

bool ObserverBase::RemoveSubjectBase(SubjectBase * subject)
{
    CriticalSectionLock lock(&m_critsec);

    bool wasRemoved = false;
    SubjectList::iterator it = m_subjects.begin();
    while (!wasRemoved
            && it != m_subjects.end()) 
    {
        if (*it == subject) 
        { 
            m_subjects.erase(it); 
            wasRemoved = true;
        }
        else
        {
            it++;
        }
    }

    return wasRemoved;
}

// drop all observed subjects
void ObserverBase::DetachAll()
{
    CriticalSectionLock lock(&m_critsec);
    for (SubjectList::iterator it = m_subjects.begin(); it != m_subjects.end(); it++)
    {
        (*it)->RemoveObserverBase(this);
    }
    m_subjects.clear();
}


void ObserverBase::AttachAll(const ObserverBase & copy)
{
    // stop any other thread messing with the subjects while we are messing
    CriticalSectionLock lock(&m_critsec);

    // keeping a lock on the copy ensures that all the subjects are valid (cannot be removed)
    CriticalSectionLock copyLock(&copy.m_critsec);
    m_subjects = copy.m_subjects;

    for (SubjectList::iterator it = m_subjects.begin(); it != m_subjects.end(); it++)
    {
        (*it)->InsertObserverBase(this);
    }
}


//////////////////////////////////////////////////////////////////////
SubjectBase::SubjectBase() :
    m_notificationLevel(0)
{
}

SubjectBase::~SubjectBase()
{
    DetachAll();
}

// a copied subject does NOT notify the original's observers
SubjectBase::SubjectBase(const SubjectBase & copy) :
    m_notificationLevel(0)
{
    UNREFERENCED_PARAMETER(copy);
}

SubjectBase & SubjectBase::operator=(const SubjectBase & copy)
{
    UNREFERENCED_PARAMETER(copy);
    DetachAll(); // copy over drops all the original observers
    return *this;
}

void SubjectBase::AttachObserverBase(ObserverBase * pObserver)
{
    if (InsertObserverBase(pObserver))
    {
        pObserver->InsertSubjectBase(this);
    }
}

void SubjectBase::DetachObserverBase(ObserverBase * pObserver)
{
    if (RemoveObserverBase(pObserver))
    {
        pObserver->RemoveSubjectBase(this);
    }
}

bool SubjectBase::InsertObserverBase(ObserverBase * pObserver)
{
    CriticalSectionLock lock(&m_critsec);

    bool alreadyPresent = false;

    // Find the first NULL element to add our subject to or if there are no NULL's (i.e. no free slots)
    // add it on the end.
    for (ObserverList::iterator it = m_observers.begin(); 
            it != m_observers.end() && !alreadyPresent; 
            it++)
    {
        if (*it == pObserver) 
        { 
            alreadyPresent = true;
        }
    }

    if (!alreadyPresent)
    {
        // no need to worry about adding during a notification as it is a list and
        // items added to the end do not invalidate any iterators
        m_observers.push_front(pObserver);
    }

    // Turn the logic upside down.
    bool hasBeenAdded = !alreadyPresent;
    return hasBeenAdded;
}

bool SubjectBase::RemoveObserverBase(ObserverBase * pObserver)
{
    CriticalSectionLock lock(&m_critsec);

    bool wasRemoved = false;
    ObserverList::iterator it = m_observers.begin();
    while (!wasRemoved
            && it != m_observers.end())
    {
        if (*it == pObserver) 
        { 
            if (m_notificationLevel > 0)
            {
                // during the notification just NULL out anything to be removed
                // as removing them from the list would invalidate any iterators
                *it = NULL; 
            }
            else
            {
                m_observers.erase(it);
            }
            wasRemoved = true;
        }
        else
        {
            it++;
        }
    }

    return wasRemoved;
}

void SubjectBase::DetachAll()
{
    CriticalSectionLock lock(&m_critsec);
    ASSERT(m_notificationLevel == 0);
    for (ObserverList::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        (*it)->RemoveSubjectBase(this);
    }
    m_observers.clear();
}

void SubjectBase::BeginNotification() const
{
    CriticalSectionLock lock(&m_critsec);
    ++m_notificationLevel;
}

void SubjectBase::EndNotification() const
{
    CriticalSectionLock lock(&m_critsec);

    if (m_notificationLevel == 1)
    {
        // remove any observers that have been NULLed out during the notification
        ObserverList::iterator it = m_observers.begin();
        while (it != m_observers.end())
        {
            if (*it == NULL)
            {
                it = m_observers.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    --m_notificationLevel;
}

SubjectBase::ObserverList::const_iterator SubjectBase::ObserversBegin() const
{
    return m_observers.begin();
}

SubjectBase::ObserverList::const_iterator SubjectBase::ObserversEnd() const
{
    return m_observers.end();
}
