// DLMacros.h
//
// Copyright (c) Sirius Analytical Instruments Ltd

#pragma once

// includes needed for declaring an element

#include "SaxContentElement.h"
#include "SaxVector.h"
#include "SaxMatrix.h"
#include <string>

namespace XmlLib
{
    class SaxWriter;
}

///////////////////////////////////////////////////////////////////////
// DL_STRINGIZE - Stringizes X after it is macro expanded
#define DL_STRINGIZE(X) DL_STRINGIZE_DELAY(X)
#define DL_STRINGIZE_DELAY(X) DL_DO_STRINGIZE(X)
#define DL_DO_STRINGIZE(X) #X

///////////////////////////////////////////////////////////////////////
// DL_CAT - Concatenates X and Y after they are macro expanded
#define DL_CAT(X,Y) DL_CAT_DELAY(X,Y)
#define DL_CAT_DELAY(X,Y) DL_DO_CAT(X,Y)
#define DL_DO_CAT(X,Y) X##Y

///////////////////////////////////////////////////////////////////////
// type support
//---------------------------------------------------------------------
#define DL_FLAG(_, xname) DL_FLAG_##_(xname)
// bool HasXname() const;
// bool ClearXname() const;
// void SetXname();
// void SetValueXName(bool value)
//---------------------------------------------------------------------
#define DL_ENUM(_, type, xname, def, map) DL_ENUM_##_(type, xname, def, map)
// type Xname() const;
// void SetXname(type newValue);
//---------------------------------------------------------------------
#define DL_OPTIONAL_ENUM(_, type, xname, def, map) DL_OPTIONAL_ENUM_##_(type, xname, def, map)
// bool HasXname() const;
// bool ClearXname() const;
// type Xname() const;
// void SetXname(type newValue);
//---------------------------------------------------------------------
#define DL_SIMPLE(_, type, xname, def) DL_SIMPLE_##_(type, xname, def)
// type Xname() const;
// void SetXname(type newValue);
//---------------------------------------------------------------------
#define DL_OPTIONAL_SIMPLE(_, type, xname, def) DL_OPTIONAL_SIMPLE_##_(type, xname, def)
// bool HasXname() const;
// bool ClearXname() const;
// type Xname() const;
// void SetXname(type newValue);
//---------------------------------------------------------------------
// DL_DEFAULT_SIMPLE for simple types that have a specific default value even if not in the file
#define DL_DEFAULT_SIMPLE(_, type, xname, def) DL_DEFAULT_SIMPLE_##_(type, xname, def)
// type Xname() const; // always available as default if not explicitely set
// void SetXname(type newValue);
//---------------------------------------------------------------------
#define DL_STRING(_, xname) DL_STRING_##_(xname)
// const std::string & Xname() const;
// void SetXname(const std::string & newValue);
//---------------------------------------------------------------------
#define DL_OPTIONAL_STRING(_, xname) DL_OPTIONAL_STRING_##_(xname)
// bool HasXname() const;
// bool ClearXname() const;
// const std::string & Xname() const;
// void SetXname(const std::string & newValue);
//---------------------------------------------------------------------
#define DL_VECTOR(_, type, xname) DL_VECTOR_##_(type, xname)
// const std::vector<type> & Xname() const;
// void SetXname(const std::vector<type> & newValue);
//---------------------------------------------------------------------
#define DL_OPTIONAL_VECTOR(_, type, xname) DL_OPTIONAL_VECTOR_##_(type, xname)
// bool HasXname() const;
// bool ClearXname() const;
// const std::vector<type> & Xname() const;
// void SetXname(const std::vector<type> & newValue);
//---------------------------------------------------------------------
#define DL_OBJECT_VECTOR(_, type, xname) DL_OBJECT_VECTOR_##_(type, xname)
// const std::vector<type> & Xname() const;
// void SetXname(const std::vector<type> & newValue);
//---------------------------------------------------------------------
#define DL_OBJECT_LIST(_, type, xname) DL_OBJECT_LIST_##_(type, xname)
// const std::list<type> & Xname() const;
// void SetXname(const std::list<type> & newValue);
//---------------------------------------------------------------------
#define DL_OBJECT(_, type, xname) DL_OBJECT_##_(type, xname)
// const type & Xname() const;
// type * Xname();
// void SetXname(const type & newValue);
//---------------------------------------------------------------------
#define DL_OPTIONAL_OBJECT(_, type, xname) DL_OPTIONAL_OBJECT_##_(type, xname)
// bool HasXname() const;
// bool ClearXname() const;
// const type & Xname() const;
// type * Xname();
// type * CreateXname();
// void SetXname(const type & newValue);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// access declaration support
#define DL_DECLARE_ACCESS(elist) public: elist(ACCESS) private:
#define DL_DECLARE_DEPRECATED_ACCESS(elist) private: elist(ACCESS) private:
#define DL_OPTIONAL_GENERIC_ACCESS(xname) \
        bool DL_CAT(Has,xname)() const; \
        void DL_CAT(Clear,xname)();
#define DL_FLAG_ACCESS(xname) \
        DL_OPTIONAL_GENERIC_ACCESS(xname) \
        void DL_CAT(Set,xname)(); \
        void DL_CAT(SetValue,xname)(bool value);
#define DL_ENUM_ACCESS(type, xname, def, map) \
        type xname() const; \
        void DL_CAT(Set,xname)(type newValue);
#define DL_OPTIONAL_ENUM_ACCESS(type, xname, def, map) \
        type xname() const; \
        void DL_CAT(Set,xname)(type newValue); \
        DL_OPTIONAL_GENERIC_ACCESS(xname);
#define DL_SIMPLE_ACCESS(type, xname, def) \
        type xname() const; \
        void DL_CAT(Set,xname)(type newValue);
#define DL_OPTIONAL_SIMPLE_ACCESS(type, xname, def) \
        DL_SIMPLE_ACCESS(type, xname, def) \
        DL_OPTIONAL_GENERIC_ACCESS(xname)
#define DL_DEFAULT_SIMPLE_ACCESS(type, xname, def) \
        DL_SIMPLE_ACCESS(type, xname, def)
#define DL_STRING_ACCESS(xname) \
        const std::string & xname() const; \
        void DL_CAT(Set,xname)(const std::string & newValue);
#define DL_OPTIONAL_STRING_ACCESS(xname) \
        DL_STRING_ACCESS(xname) \
        DL_OPTIONAL_GENERIC_ACCESS(xname)
#define DL_VECTOR_ACCESS(type, xname) \
        const std::vector<type> & xname() const; \
        void DL_CAT(Set,xname)(const std::vector<type> & newValue);
#define DL_LIST_ACCESS(type, xname) \
        const std::list<type> & xname() const; \
        void DL_CAT(Set,xname)(const std::list<type> & newValue);
#define DL_OPTIONAL_VECTOR_ACCESS(type, xname) \
        DL_VECTOR_ACCESS(type, xname) \
        DL_OPTIONAL_GENERIC_ACCESS(xname)
#define DL_OBJECT_VECTOR_ACCESS(type, xname) \
        DL_VECTOR_ACCESS(type, xname)
#define DL_OBJECT_LIST_ACCESS(type, xname) \
        DL_LIST_ACCESS(type, xname)
#define DL_OBJECT_ACCESS(type, xname) \
        const type & xname() const; \
        type * xname(); \
        void DL_CAT(Set,xname)(const type & newValue);
#define DL_OPTIONAL_OBJECT_ACCESS(type, xname) \
        DL_OBJECT_ACCESS(type, xname) \
        type * DL_CAT(Create,xname)(); \
        DL_OPTIONAL_GENERIC_ACCESS(xname)

///////////////////////////////////////////////////////////////////////
// variable declaration support
#define DL_DECLARE_VARIABLES(elist) private: elist(VARIABLE) private:
#define DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        bool DL_CAT(m_has,xname);
#define DL_FLAG_VARIABLE(xname) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname)
#define DL_ENUM_VARIABLE(type, xname, def, map) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        type DL_CAT(m_,xname);
#define DL_OPTIONAL_ENUM_VARIABLE(type, xname, def, map) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        type DL_CAT(m_,xname);
#define DL_SIMPLE_VARIABLE(type, xname, def) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        type DL_CAT(m_,xname);
#define DL_OPTIONAL_SIMPLE_VARIABLE(type, xname, def) \
        DL_SIMPLE_VARIABLE(type, xname, def)
#define DL_DEFAULT_SIMPLE_VARIABLE(type, xname, def) \
        DL_SIMPLE_VARIABLE(type, xname, def)
#define DL_STRING_VARIABLE(xname) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        std::string DL_CAT(m_,xname);
#define DL_OPTIONAL_STRING_VARIABLE(xname) \
        DL_STRING_VARIABLE(xname)
#define DL_VECTOR_VARIABLE(type, xname) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        XmlLib::SaxVector<type> DL_CAT(m_,xname);
#define DL_OPTIONAL_VECTOR_VARIABLE(type, xname) \
        DL_VECTOR_VARIABLE(type, xname)
#define DL_OBJECT_VECTOR_VARIABLE(type, xname) \
        std::vector<type> DL_CAT(m_,xname);
#define DL_OBJECT_LIST_VARIABLE(type, xname) \
        std::list<type> DL_CAT(m_,xname);
#define DL_OBJECT_VARIABLE(type, xname) \
        DL_OPTIONAL_GENERIC_VARIABLE(xname) \
        type DL_CAT(m_,xname);
#define DL_OPTIONAL_OBJECT_VARIABLE(type, xname) \
        DL_OBJECT_VARIABLE(type, xname)

///////////////////////////////////////////////////////////////////////
// name definition support
#define DL_DEFINE_NAMES(elist) \
        elist(DEFINE_NAME)
#define DL_GENERIC_DEFINE_NAME(xname) \
        const XmlLib::SaxString DL_CAT(f_sax,xname) = DL_CAT(L,DL_STRINGIZE(xname));
#define DL_FLAG_DEFINE_NAME(xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_ENUM_DEFINE_NAME(type, xname, def, map) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OPTIONAL_ENUM_DEFINE_NAME(type, xname, def, map) DL_GENERIC_DEFINE_NAME(xname)
#define DL_SIMPLE_DEFINE_NAME(type, xname, def) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OPTIONAL_SIMPLE_DEFINE_NAME(type, xname, def) DL_GENERIC_DEFINE_NAME(xname)
#define DL_DEFAULT_SIMPLE_DEFINE_NAME(type, xname, def) DL_GENERIC_DEFINE_NAME(xname)
#define DL_STRING_DEFINE_NAME(xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OPTIONAL_STRING_DEFINE_NAME(xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_VECTOR_DEFINE_NAME(type, xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_LIST_DEFINE_NAME(type, xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OPTIONAL_VECTOR_DEFINE_NAME(type, xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OBJECT_VECTOR_DEFINE_NAME(type, xname)
#define DL_OBJECT_LIST_DEFINE_NAME(type, xname)
#define DL_OBJECT_DEFINE_NAME(type, xname) DL_GENERIC_DEFINE_NAME(xname)
#define DL_OPTIONAL_OBJECT_DEFINE_NAME(type, xname) DL_GENERIC_DEFINE_NAME(xname)

///////////////////////////////////////////////////////////////////////
// initialisation support
#define DL_INIT(elist) elist(INIT)
#define DL_OPTIONAL_GENERIC_INIT(xname) \
        DL_CAT(m_has,xname) = false;
#define DL_FLAG_INIT(xname) \
        DL_OPTIONAL_GENERIC_INIT(xname)
#define DL_ENUM_INIT(type, xname, def, map) \
        DL_OPTIONAL_GENERIC_INIT(xname) \
        DL_CAT(m_,xname) = def;
#define DL_OPTIONAL_ENUM_INIT(type, xname, def, map) \
        DL_OPTIONAL_GENERIC_INIT(xname) \
        DL_CAT(m_,xname) = def;
#define DL_SIMPLE_INIT(type, xname, def) \
        DL_OPTIONAL_GENERIC_INIT(xname) \
        DL_CAT(m_,xname) = def;
#define DL_OPTIONAL_SIMPLE_INIT(type, xname, def) \
        DL_SIMPLE_INIT(type, xname, def)
#define DL_DEFAULT_SIMPLE_INIT(type, xname, def) \
        DL_SIMPLE_INIT(type, xname, def)
#define DL_STRING_INIT(xname) \
        DL_OPTIONAL_GENERIC_INIT(xname)
#define DL_OPTIONAL_STRING_INIT(xname) \
        DL_STRING_INIT(xname)
#define DL_VECTOR_INIT(type, xname) \
        DL_OPTIONAL_GENERIC_INIT(xname)
#define DL_OPTIONAL_VECTOR_INIT(type, xname) \
        DL_VECTOR_INIT(type, xname)
#define DL_OBJECT_VECTOR_INIT(type, xname)
#define DL_OBJECT_LIST_INIT(type, xname)
#define DL_OBJECT_INIT(type, xname) \
        DL_OPTIONAL_GENERIC_INIT(xname)
#define DL_OPTIONAL_OBJECT_INIT(type, xname) \
        DL_OPTIONAL_GENERIC_INIT(xname)

///////////////////////////////////////////////////////////////////////
// access definition support
#define DL_DEFINE_ACCESS(elist) elist(DEFINE_ACCESS)
#define DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        bool DL_ELEMENT::DL_CAT(Has,xname)() const \
        { \
            return DL_CAT(m_has,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Clear,xname)() \
        { \
            DL_CAT(m_has,xname) = false; \
        }
#define DL_FLAG_DEFINE_ACCESS(xname) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        void DL_ELEMENT::DL_CAT(Set,xname)() \
        { \
            DL_CAT(m_has,xname) = true; \
        } \
        void DL_ELEMENT::DL_CAT(SetValue,xname)(bool value)\
        { \
            DL_CAT(m_has,xname) = value; \
        }
#define DL_ENUM_DEFINE_ACCESS(type, xname, def, map) \
        type DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(type newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_OPTIONAL_ENUM_DEFINE_ACCESS(type, xname, def, map) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        type DL_ELEMENT::xname() const \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return DL_CAT(m_has,xname) ? DL_CAT(m_,xname) : def; \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(type newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_SIMPLE_DEFINE_ACCESS(type, xname, def) \
        type DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(type newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_OPTIONAL_SIMPLE_DEFINE_ACCESS(type, xname, def) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        type DL_ELEMENT::xname() const \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return DL_CAT(m_has,xname) ? DL_CAT(m_,xname) : def; \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(type newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_DEFAULT_SIMPLE_DEFINE_ACCESS(type, xname, def) \
        type DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(type newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_STRING_DEFINE_ACCESS(xname) \
        const std::string & DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const std::string & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_OPTIONAL_STRING_DEFINE_ACCESS(xname) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        const std::string & DL_ELEMENT::xname() const \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const std::string & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_VECTOR_DEFINE_ACCESS(type, xname) \
        const std::vector<type> & DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const std::vector<type> & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_LIST_DEFINE_ACCESS(type, xname) \
        const std::list<type> & DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const std::list<type> & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_OPTIONAL_VECTOR_DEFINE_ACCESS(type, xname) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        const std::vector<type> & DL_ELEMENT::xname() const \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const std::vector<type> & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_OBJECT_VECTOR_DEFINE_ACCESS(type, xname) \
        DL_VECTOR_DEFINE_ACCESS(type, xname)
#define DL_OBJECT_DEFINE_ACCESS(type, xname) \
        const type & DL_ELEMENT::xname() const \
        { \
            return DL_CAT(m_,xname); \
        } \
        type * DL_ELEMENT::xname() \
        { \
            return &DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const type & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
        }
#define DL_OBJECT_LIST_DEFINE_ACCESS(type, xname) \
        DL_LIST_DEFINE_ACCESS(type, xname)
#define DL_OPTIONAL_OBJECT_DEFINE_ACCESS(type, xname) \
        DL_OPTIONAL_GENERIC_DEFINE_ACCESS(xname) \
        const type & DL_ELEMENT::xname() const \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return DL_CAT(m_,xname); \
        } \
        type * DL_ELEMENT::xname() \
        { \
            ASSERT(DL_CAT(m_has,xname)); \
            return &DL_CAT(m_,xname); \
        } \
        type * DL_ELEMENT::DL_CAT(Create,xname)() \
        { \
            DL_CAT(m_has,xname) = true; \
            return &DL_CAT(m_,xname); \
        } \
        void DL_ELEMENT::DL_CAT(Set,xname)(const type & newValue) \
        { \
            DL_CAT(m_,xname) = newValue; \
            DL_CAT(m_has,xname) = true; \
        }

///////////////////////////////////////////////////////////////////////
// support for SaxElement functions
#define DL_START(elist) elist(START)
#define DL_GENERIC_DUPLICATE(flag,xname) SAXASSERT(!flag,DL_STRINGIZE(DL_ELEMENT) " had duplicate " DL_STRINGIZE(xname) " elements")
#define DL_FLAG_START(xname) \
        if (subHandler == NULL && name == DL_CAT(f_sax,xname)) \
        { \
            DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
            DL_CAT(m_has,xname) = true; \
        }
#define DL_SIMPLE_START(type, xname, def) \
        if (subHandler == NULL && name == DL_CAT(f_sax,xname)) \
        { \
            DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
            subHandler = HandleSimpleElement(&DL_CAT(m_,xname)); \
            DL_CAT(m_has, xname) = true; \
        }
#define DL_ENUM_START(type, xname, def, map) \
        if (subHandler == NULL && name == DL_CAT(f_sax,xname)) \
        { \
            DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
            subHandler = HandleEnumElement(&DL_CAT(m_,xname),map); \
            DL_CAT(m_has, xname) = true; \
        }
#define DL_OPTIONAL_ENUM_START(type, xname, def, map) \
        if (subHandler == NULL && name == DL_CAT(f_sax,xname)) \
        { \
            DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
            subHandler = HandleEnumElement(&DL_CAT(m_,xname),map); \
            DL_CAT(m_has, xname) = true; \
        }
#define DL_OPTIONAL_SIMPLE_START(type, xname, def) \
        DL_SIMPLE_START(type, xname, def)
#define DL_DEFAULT_SIMPLE_START(type, xname, def) \
        DL_SIMPLE_START(type, xname, def)
#define DL_STRING_START(xname) DL_SIMPLE_START(void, xname, def)
#define DL_OPTIONAL_STRING_START(xname) DL_SIMPLE_START(void, xname, def)
#define DL_VECTOR_START(type, xname) \
        if (subHandler == NULL && name == DL_CAT(f_sax,xname)) \
        { \
            DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
            subHandler = HandleSimpleElement(DL_CAT(m_,xname).OverwriteString(attributes)); \
            DL_CAT(m_has, xname) = true; \
        }
#define DL_OPTIONAL_VECTOR_START(type, xname) \
        DL_VECTOR_START(type, xname)
#define DL_OBJECT_VECTOR_START(type, xname) \
        if (subHandler == NULL) \
        { \
            type object; \
            if (object.SaxElementIsSelf(name, attributes)) \
            { \
                DL_CAT(m_,xname).push_back(object); \
                subHandler = &DL_CAT(m_,xname).back(); \
            } \
        }
#define DL_OBJECT_LIST_START(type, xname) \
        if (subHandler == NULL) \
        { \
            type object; \
            if (object.SaxElementIsSelf(name, attributes)) \
            { \
                DL_CAT(m_,xname).push_back(object); \
                subHandler = &DL_CAT(m_,xname).back(); \
            } \
        }
#define DL_OBJECT_START(type, xname) \
        if (subHandler == NULL) \
        { \
            if (DL_CAT(m_,xname).SaxElementIsSelf(name, attributes)) \
            { \
                DL_GENERIC_DUPLICATE(DL_CAT(m_has, xname),xname) \
                subHandler = &DL_CAT(m_,xname); \
                DL_CAT(m_has, xname) = true; \
            } \
        }
#define DL_OPTIONAL_OBJECT_START(type, xname) \
        DL_OBJECT_START(type, xname)
//---------------------------------------------------------------------
#define DL_END(elist) elist(END)
#define DL_GENERIC_MISSING(flag,xname) SAXASSERT(flag,DL_STRINGIZE(DL_ELEMENT) " was missing " DL_STRINGIZE(xname) " element")
#define DL_GENERIC_END(xname) \
        DL_GENERIC_MISSING(DL_CAT(m_has,xname),xname)
#define DL_FLAG_END(xname)
#define DL_ENUM_END(type, xname, def, map) \
        DL_GENERIC_END(xname) \
        if (DL_CAT(m_has,xname)) \
        { \
            if (DL_CAT(m_,xname) == type(0)) \
            { \
                SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::" DL_STRINGIZE(DL_CAT(m_,xname)) " had bad loaded enum value") \
            } \
        }
#define DL_OPTIONAL_ENUM_END(type, xname, def, map) \
        if (DL_CAT(m_has,xname)) \
        { \
            if (DL_CAT(m_,xname) == type(0)) \
            { \
                SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::" DL_STRINGIZE(DL_CAT(m_,xname)) " had bad loaded enum value") \
            } \
        }
#define DL_SIMPLE_END(type, xname, def) \
        DL_GENERIC_END(xname)
#define DL_OPTIONAL_SIMPLE_END(type, xname, def)
#define DL_DEFAULT_SIMPLE_END(type, xname, def)
#define DL_STRING_END(xname) \
        DL_GENERIC_END(xname)
#define DL_OPTIONAL_STRING_END(xname)
#define DL_VECTOR_END(type, xname) \
        DL_GENERIC_END(xname)
#define DL_VECTOR_END(type, xname) \
        DL_GENERIC_END(xname)
#define DL_LIST_END(type, xname) \
        DL_GENERIC_END(xname)
#define DL_OPTIONAL_VECTOR_END(type, xname)
#define DL_OBJECT_VECTOR_END(type, xname)
#define DL_OBJECT_LIST_END(type, xname)
#define DL_OBJECT_END(type, xname) \
        DL_GENERIC_END(xname)
#define DL_OPTIONAL_OBJECT_END(type, xname)
//---------------------------------------------------------------------
#define DL_WRITE(elist) elist(WRITE)
#define DL_FLAG_WRITE(xname) \
        if (DL_CAT(m_has,xname)) \
        { \
            writer->WriteEmptyElement(DL_CAT(f_sax,xname)); \
        }
#define DL_ENUM_WRITE(type, xname, def, map) \
        writer->WriteEnumElement(DL_CAT(f_sax,xname), DL_CAT(m_,xname), map);
#define DL_OPTIONAL_ENUM_WRITE(type, xname, def, map) \
        if (DL_CAT(m_has,xname)) \
        { \
            writer->WriteEnumElement(DL_CAT(f_sax,xname), DL_CAT(m_,xname), map); \
        }
#define DL_SIMPLE_WRITE(type, xname, def) \
        writer->WriteSimpleElement(DL_CAT(f_sax,xname), DL_CAT(m_,xname));
#define DL_OPTIONAL_SIMPLE_WRITE(type, xname, def) \
        if (DL_CAT(m_has,xname)) \
        { \
            DL_SIMPLE_WRITE(type, xname, def) \
        }
#define DL_DEFAULT_SIMPLE_WRITE(type, xname, def) DL_SIMPLE_WRITE(type, xname, def)
#define DL_STRING_WRITE(xname) DL_SIMPLE_WRITE(void, xname, def)
#define DL_OPTIONAL_STRING_WRITE(xname) DL_OPTIONAL_SIMPLE_WRITE(void, xname, def)
#define DL_VECTOR_WRITE(type, xname) \
        DL_CAT(m_,xname).Write(DL_CAT(f_sax,xname), writer);
#define DL_OPTIONAL_VECTOR_WRITE(type, xname) \
        if (DL_CAT(m_has,xname)) \
        { \
            DL_VECTOR_WRITE(type, xname) \
        }
#define DL_OBJECT_VECTOR_WRITE(type, xname) \
        if (!DL_CAT(m_,xname).empty()) \
        { \
            std::vector<type>::const_iterator iter; \
            for (iter = DL_CAT(m_,xname).begin(); iter != DL_CAT(m_,xname).end(); ++iter) \
            { \
                iter->Write(writer); \
            } \
        }
#define DL_OBJECT_LIST_WRITE(type, xname) \
        if (!DL_CAT(m_,xname).empty()) \
        { \
            std::list<type>::const_iterator iter; \
            for (iter = DL_CAT(m_,xname).begin(); iter != DL_CAT(m_,xname).end(); ++iter) \
            { \
                iter->Write(writer); \
            } \
        }
#define DL_OBJECT_WRITE(type, xname) \
        DL_CAT(m_,xname).Write(writer);
#define DL_OPTIONAL_OBJECT_WRITE(type, xname) \
        if (DL_CAT(m_has,xname)) \
        { \
            DL_OBJECT_WRITE(type, xname) \
        }
///////////////////////////////////////////////////////////////////////
