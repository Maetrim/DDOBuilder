// ObserverSubject.h
//
// Interface for the Observer-Subject pattern

#pragma once

#include "CriticalSection.h"
#include <list>
#include <sstream>

class ObserverBase;
class SubjectBase;

template <class T>
class Observer;
template <class T>
class Subject;

// The ObserverBase keeps a set of all the subjects that have been told
// to notify it.
// At the appropriate time (e.g. when it is deleted) all the subjects are told
// to forget this observer and stop notifications.
class ObserverBase
{
    protected:
        ObserverBase();
        virtual ~ObserverBase();

        // a copied observer DOES observe all the original's subjects
        ObserverBase(const ObserverBase & copy);
        ObserverBase & operator =(const ObserverBase & copy);

    private:
        bool InsertSubjectBase(SubjectBase * subject);
        bool RemoveSubjectBase(SubjectBase * subject);

        void AttachAll(const ObserverBase & copy); // attach to a list from another observer
        void DetachAll();

        typedef std::list<SubjectBase *> SubjectList;
        SubjectList m_subjects;

        CriticalSection m_critsec;

    friend class SubjectBase;
};

// The SubjectBase keeps a set of all the observers it has been told to notify.
// The list of observers can be iterated through to allow the notification
// functions on each to be called.
// At the appropriate time (e.g. when it is deleted) all the observers are told
// to forget this subject so they no longer have to tell the subject to stop
// notifications.
class SubjectBase
{
    public:
        SubjectBase();
        virtual ~SubjectBase();

    protected:
        // a copied SubjectBase does NOT notify the original observers
        SubjectBase(const SubjectBase & copy);
        SubjectBase & operator=(const SubjectBase & copy);

        // allow the template Subject to iterate through the observers
        typedef std::list<ObserverBase *> ObserverList;
        ObserverList::const_iterator ObserversBegin() const;
        ObserverList::const_iterator ObserversEnd() const;

        void AttachObserverBase(ObserverBase * pObserver);
        void DetachObserverBase(ObserverBase * pObserver);

        bool InsertObserverBase(ObserverBase * pObserver);
        bool RemoveObserverBase(ObserverBase * pObserver);

        void BeginNotification() const;
        void EndNotification() const;

        CriticalSection m_critsec;

    private:
        void DetachAll();

        mutable ObserverList m_observers;
        mutable long m_notificationLevel;

    friend class ObserverBase;
};

// This gets the implementation to support the links with the subject.
// It doesn't add anything that the straight ObserverBase wouldn't but
// keeping it here may allow us to change the observer-subject pattern
// without having to change all the classes that use it.
template <class T>
class Observer :
    public virtual ObserverBase
{
};

// This provides a type-safe interface to ensure that only the correct
// type of observer get attached to a subject
template <class T>
class Subject :
    public virtual SubjectBase
{
    public:

        void AttachObserver(T * observer)
        {
            SubjectBase::AttachObserverBase(observer);
        }

        void DetachObserver(T * observer)
        {
            SubjectBase::DetachObserverBase(observer);
        }

    protected:

        template <typename F, typename P>
        void NotifyAll(F f, P * a) const
        {
            CriticalSectionLock lock(&m_critsec);
            BeginNotification();
            for (ObserverList::const_iterator it = ObserversBegin(); it != ObserversEnd(); it++)
            {
                if (*it != NULL)
                {
                    T * p = dynamic_cast<T*>(*it);
                    if (p != NULL)
                    {
                        try
                        {
                            (p->*f)(a);
                        }
                        catch(...)
                        {
                            // ignore any exception in the observer
                            ::OutputDebugString("Exception ignored\n");
                        }
                    }
                }
            }
            EndNotification();
        }

        template <typename F, typename P, typename D>
        void NotifyAll(F f, P * a, const D & d) const
        {
            CriticalSectionLock lock(&m_critsec);
            BeginNotification();
            for (ObserverList::const_iterator it = ObserversBegin(); it != ObserversEnd(); it++)
            {
                if (*it != NULL)
                {
                    T * p = dynamic_cast<T*>(*it);
                    if (p != NULL)
                    {
                        try
                        {
                            (p->*f)(a, d);
                        }
                        catch(...)
                        {
                            // ignore any exception in the observer
                            ::OutputDebugString("Exception ignored\n");
                        }
                    }
                }
            }
            EndNotification();
        }

        template <typename F, typename P, typename D1, typename D2>
        void NotifyAll(F f, P * a, const D1 & d1, const D2 & d2) const
        {
            CriticalSectionLock lock(&m_critsec);
            BeginNotification();
            for (ObserverList::const_iterator it = ObserversBegin(); it != ObserversEnd(); it++)
            {
                if (*it != NULL)
                {
                    T * p = dynamic_cast<T*>(*it);
                    if (p != NULL)
                    {
                        try
                        {
                            (p->*f)(a, d1, d2);
                        }
                        catch(...)
                        {
                            // ignore any exception in the observer
                            ::OutputDebugString("Exception ignored\n");
                        }
                    }
                }
            }
            EndNotification();
        }
};
