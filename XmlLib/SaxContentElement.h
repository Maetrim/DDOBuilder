// SaxContentElement.h
//
#pragma once

#include "XmlLibExports.h"
#include "SaxString.h"
#include "SaxAttributes.h"
#include <vector>
#include <sstream>

///////////////////////////////////////////////////////////////////////

#define SAXASSERT(expr,stream) {if(!bool(expr)){std::stringstream ss;ss<<stream;ReportSaxReadError(ss.str());}}

namespace XmlLib
{
    // base class for all types of thing that receive parsing events from SAX
    // mostly you will want to use SaxContentElement as your base class
    class SaxContentElementInterface
    {
        public:
            SaxContentElementInterface();
            virtual ~SaxContentElementInterface();
            virtual SaxContentElementInterface * StartElement(
                    const SaxString & name,
                    const SaxAttributes & attributes);
            virtual void Characters(const SaxString & chars);
            virtual void EndElement();

        // error handling - reported read errors are passed up until something handles them
        // or the root is reached where with nowhere else to pass the error it is thrown as
        // an exception and the read terminates
        public:
            virtual void ReportSaxReadError(const std::string & errorDescription);
        private:
            SaxContentElementInterface * SaxReadErrorHandler() const;
            void SetSaxReadErrorHandler(SaxContentElementInterface * handler);
            void ClearSaxReadErrorHandler();

            SaxContentElementInterface * m_readErrorHandler;

        friend class SaxReader;
    };
}

//////////////////////////////////////////////////////////////////////

namespace XmlLib
{
    // class to handle simple types (ie streamable) being received from SAX
    // only use via SaxContentElement::HandleSimpleElement
    template <typename T>
    class SaxSimpleElement :
        public SaxContentElementInterface
    {
        public:

            static SaxSimpleElement<T> * Handle(T * t);

            void Characters(const SaxString & chars);

        private:

            // note: this is NOT THREAD SAFE but a lot faster than creating and deleting them
            static SaxSimpleElement<T> s_singleton;

            T * m_t;
    };

    template <typename T>
    SaxSimpleElement<T> SaxSimpleElement<T>::s_singleton;

    template <typename T>
    SaxSimpleElement<T> * SaxSimpleElement<T>::Handle(T * t)
    {
        s_singleton.m_t = t;
        return &s_singleton;
    }

    template <typename T>
    void SaxSimpleElement<T>::Characters(
            const SaxString & chars)
    {
        std::stringstream ss(chars);
        ss >> *m_t;
    }

    //////////////////////////////////////////////////////////////////////

    template <>
    class SaxSimpleElement<std::string> :
        public SaxContentElementInterface
    {
        public:
            static SaxSimpleElement<std::string> * Handle(std::string * t);

            void Characters(const SaxString & chars);

        private:

            // note: this is NOT THREAD SAFE but a lot faster than creating and deleting them
            static SaxSimpleElement<std::string> s_singleton;

            std::string * m_t;
    };

    //////////////////////////////////////////////////////////////////////

    template <>
    class SaxSimpleElement<std::wstring> :
        public SaxContentElementInterface
    {
        public:
            static SaxSimpleElement<std::wstring> * Handle(std::wstring * t);

            void Characters(const SaxString & chars);

        private:

            // note: this is NOT THREAD SAFE but a lot faster than creating and deleting them
            static SaxSimpleElement<std::wstring> s_singleton;

            std::wstring * m_t;
    };
}

//////////////////////////////////////////////////////////////////////
//
// map for use with enumerated types in SAX parsing
//
// eg days of the week are stored (sunday = 0) as English words use
//
// static enumMapEntry<int> weekdayMap[] =
// {
//     {0, L"Sunday"},
//     {1, L"Monday"},
//     {2, L"Tuesday"},
//     {3, L"Wednesday"},
//     {4, L"Thursday"},
//     {5, L"Friday"},
//     {6, L"Saturday"},
//     {0,NULL} // any value will do, must have string of NULL
// };

namespace XmlLib
{
    template <typename T>
    struct enumMapEntry
    {
        T value;
        const wchar_t * name;
    };

    //////////////////////////////////////////////////////////////////////
    //
    // class to handle enumerations being received from SAX
    // only use via SaxContentElement::HandleEnumElement

    template <typename T>
    class SaxEnumElement :
        public SaxContentElementInterface
    {
        public:

            static SaxEnumElement<T> * Handle(T * t, const enumMapEntry<T> * m);

            void Characters(const SaxString & chars);

        private:

            // note: this is NOT THREAD SAFE but a lot faster than creating and deleting them
            static SaxEnumElement<T> s_singleton;

            T * m_t;
            const enumMapEntry<T> * m_m;
    };

    template <typename T>
    SaxEnumElement<T> SaxEnumElement<T>::s_singleton;

    template <typename T>
    SaxEnumElement<T> * SaxEnumElement<T>::Handle(T * t, const enumMapEntry<T> * m)
    {
        s_singleton.m_t = t;
        s_singleton.m_m = m;
        return &s_singleton;
    }

    template <typename T>
    void SaxEnumElement<T>::Characters(
            const SaxString & chars)
    {
        const enumMapEntry<T> * p = m_m;
        while (p->name != NULL)
        {
            if (chars == p->name)
            {
                *m_t = p->value;
                break;
            }
            ++p;
        }
        if (p->name == NULL)
        {
            std::stringstream ss;
            std::string element = chars;
            ss << "Element named \"" << element << "\" of type " << typeid(T).name() << " not found\n";
            ::OutputDebugString(ss.str().c_str());
        }
    }
}

//////////////////////////////////////////////////////////////////////
//
// the base class for all things expected to handle SAX parsing events

namespace XmlLib
{
    class SaxContentElement :
        public SaxContentElementInterface
    {
        public:

            enum ReadErrorMode
            {
                REM_terminate, // default - throw error so reading stops immediately
                REM_accumulate, // cache up all errors and continue reading
            };

            SaxContentElement(const SaxString & elementName);
            SaxContentElement(const SaxString & elementName, unsigned version);

            // don't overwrite our name if copying - we already have our name
            const SaxContentElement & operator=(const SaxContentElement & copy);

            const SaxString & ElementName() const;

            // return true if the given element name is correct for this (derived) class
            virtual bool SaxElementIsSelf(
                    const SaxString & name,
                    const SaxAttributes & attributes);
            void SaxSetAttributes(const SaxAttributes & attributes);

            // set up a handler for the simple type given
            template <typename T>
            SaxContentElementInterface * HandleSimpleElement(T * t)
            {
                return SaxSimpleElement<T>::Handle(t);
            }

            // set up a handler for the enumerated type given (with a map matching values to names)
            template <typename T>
            SaxContentElementInterface * HandleEnumElement(T * t, const enumMapEntry<T> * m)
            {
                return SaxEnumElement<T>::Handle(t, m);
            }

            void SetReadErrorMode(ReadErrorMode mode);
            ReadErrorMode GetReadErrorMode() const;
            bool HasReadErrors() const;
            const std::vector<std::string> & ReadErrors() const;

        protected:
            unsigned ElementHandlingVersion() const;
            SaxAttributes VersionAttributes() const;

            void ReportSaxReadError(const std::string & errorDescription);

        private:
            // WARNING WARNING WARNING WARNING WARNING WARNING
            // we use our own copy assignment function, not a compiler generated one
            // if elements are added here then the copy assignment function MUST be updated

            SaxString m_elementName; // not copied on copy assignment
            unsigned m_elementVersion; // not copied on copy assignment
            unsigned m_elementHandlingVersion;

            ReadErrorMode m_readErrorMode;
            std::vector<std::string> m_saxReadErrors;

            // WARNING WARNING WARNING WARNING WARNING WARNING
    };
}

//////////////////////////////////////////////////////////////////////
