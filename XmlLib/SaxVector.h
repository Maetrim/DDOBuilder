// SaxVector.h
//
#pragma once

#include "SaxWriter.h"
#include "VectorConversion.h"
#include <vector>

namespace XmlLib
{
    template <typename T>
    class SaxVector
    {
        public:
            SaxVector();
            explicit SaxVector(const std::vector<T> & v);
            virtual ~SaxVector(); // ensure member destructors get called

            const SaxVector & operator=(const SaxVector & v);
            void push_back(const T & x);
            void pop_back();
            void resize(size_t n, T x = T());

            // conversion to/from vector - for use
            operator const std::vector<T> & () const;
            const SaxVector & operator=(const std::vector<T> & v);

            bool operator==(const SaxVector & other) const; 
            bool operator!=(const SaxVector & other) const;

            // Sax handlers
            void Write(const XmlLib::SaxString & saxElementName, XmlLib::SaxWriter * writer, size_t numDecimalPlaces = 6) const;
            std::wstring * OverwriteString(const SaxAttributes & attributes);

            void Clear();

            // also implement const parts of the standard vector interface

            typedef typename std::vector<T>::size_type size_type;
            typedef typename std::vector<T>::const_iterator const_iterator;
            typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
            typedef typename std::vector<T>::const_reference const_reference;

            const_iterator begin() const { return RealiseVector().begin(); }
            const_iterator end() const { return RealiseVector().end(); }
            const_reverse_iterator rbegin() { return RealiseVector().rbegin(); }
            const_reverse_iterator rend() const { return RealiseVector().rend(); }
            size_type size() const { return RealiseVector().size(); }
            bool empty() const { return RealiseVector().empty(); }
            const_reference at(size_type pos) const { return RealiseVector().at(pos); }
            const_reference front() const { return RealiseVector().front(); }
            const_reference back() const { return RealiseVector().back(); }
            const_reference operator[](size_type pos) const { return RealiseVector()[pos]; }

            // also implement 'clear' from the standard vector interface as it is so useful

            void clear() { Clear(); }

        private:
            const std::vector<T> & RealiseVector() const;
            const std::wstring & RealiseString(size_t numDecimalPlaces) const;
            mutable bool m_validVector;
            mutable bool m_validString;
            mutable std::vector<T> m_vector;
            mutable std::wstring m_string;

            size_t m_saxSize;
    };
}

template <typename T>
XmlLib::SaxVector<T>::SaxVector() :
    m_validVector(true), // empty
    m_validString(true) // empty
{
}

template <typename T>
XmlLib::SaxVector<T>::SaxVector(const std::vector<T> & v) :
    m_validVector(true),
    m_validString(false),
    m_vector(v)
{
}

template <typename T>
XmlLib::SaxVector<T>::~SaxVector()
{
}

template <typename T>
const XmlLib::SaxVector<T> & XmlLib::SaxVector<T>::operator=(const SaxVector<T> & v)
{
    m_vector = v.RealiseVector();
    m_validVector = true;
    m_validString = false;
    return *this;
}

template <typename T>
void XmlLib::SaxVector<T>::push_back(const T & x)
{
    RealiseVector();
    m_vector.push_back(x);
    m_validVector = true;
    m_validString = false;
}

template <typename T>
void XmlLib::SaxVector<T>::pop_back()
{
    RealiseVector();
    m_vector.pop_back();
    m_validVector = true;
    m_validString = false;
}

template <typename T>
void XmlLib::SaxVector<T>::resize(size_t n, T x)
{
    RealiseVector();
    m_vector.resize(n, x);
    m_validVector = true;
    m_validString = false;
}

template <typename T>
const XmlLib::SaxVector<T> & XmlLib::SaxVector<T>::operator=(const std::vector<T> & v)
{
    // overwrite the vector value (string is no longer valid)

    m_vector = v;
    m_validVector = true;
    m_validString = false;
    return *this;
}

template <typename T>
XmlLib::SaxVector<T>::operator const std::vector<T> & () const
{
    // ensure the vector value is valid and return it

    return RealiseVector();
}

template <typename T>
bool XmlLib::SaxVector<T>::operator==(const XmlLib::SaxVector<T> & other) const 
{ 
    return RealiseVector() == other.RealiseVector(); 
}

template <typename T>
bool XmlLib::SaxVector<T>::operator!=(const XmlLib::SaxVector<T> & other) const
{ 
    return !operator==(other); 
}

template <typename T>
void XmlLib::SaxVector<T>::Write(
        const XmlLib::SaxString & saxElementName,
        XmlLib::SaxWriter * writer,
        size_t numDecimalPlaces) const
{
    SaxAttributes attributes;

    const XmlLib::SaxString saxSizeAttribute = L"size";
    std::wstringstream wssSize;
    if (m_validVector)
    {
        wssSize << m_vector.size();
    }
    else
    {
        wssSize << m_saxSize;
    }
    std::wstring wSize = wssSize.str();
    attributes[saxSizeAttribute] = XmlLib::SaxString(wSize);
    
    writer->StartElement(saxElementName, attributes, false);
    
    writer->Characters(XmlLib::SaxString(RealiseString(numDecimalPlaces)));

    writer->EndElement(false);
}

template <typename T>
std::wstring * XmlLib::SaxVector<T>::OverwriteString(
        const SaxAttributes & attributes)
{
    // return a pointer to the string - allowing the client to overwrite it
    // (vector is no longer valid)

    // store size
    {
        const XmlLib::SaxString saxSizeAttribute = L"size";
        if (!attributes.HasAttribute(saxSizeAttribute))
        {
            THROW("vector attribute 'size' missing");
        }
        else
        {
            std::wstringstream wssSize(attributes[saxSizeAttribute]);
            wssSize >> m_saxSize;
            if (wssSize.fail())
            {
                THROW("vector attribute 'size' invalid");
            }
        }
    }

    m_vector.clear();
    m_string.erase();
    m_validVector = false;
    m_validString = true;
    return &m_string;
}

template <typename T>
void XmlLib::SaxVector<T>::Clear()
{
    m_vector.clear();
    m_string.erase();
    m_validVector = true;
    m_validString = true;
}

template <typename T>
const std::vector<T> & XmlLib::SaxVector<T>::RealiseVector() const
{
    if (!m_validVector)
    {
        StringToVector(m_string, &m_vector);

        if (m_vector.size() != m_saxSize)
        {
            THROW("incorrect size when realising vector");
        }
        else
        {
            m_validVector = true;
        }
    }
    return m_vector;
}

template <typename T>
const std::wstring & XmlLib::SaxVector<T>::RealiseString(size_t numDecimalPlaces) const
{
    if (!m_validString)
    {
        VectorToString(m_vector, numDecimalPlaces, &m_string);
        m_validString = true;
    }
    return m_string;
}

// stream out a vector to a stream - vector contents must be streamable
template<class T>
std::ostream & operator << (std::ostream & os, const std::vector<T> & v)
{
    for (size_t i=0; i<v.size(); ++i)
    {
        if (i>0)
        {
            os << " ";
        }
        os << v[i];
    }

    return os;
}

// stream in a vector from a stream - vector contents must be streamable
template<class T>
std::istream & operator >> (std::istream & is, std::vector<T> & v)
{
    v.clear();
    while (!is.eof())
    {
        T t;
        is >> t;
        if (is.fail())
        {
            if (is.eof()) is.clear();
            break;
        }
        v.push_back(t);
    }
    return is;
}


// stream out a vector to a stream - vector contents must be streamable
template<class T>
std::wostream & operator << (std::wostream & os, const std::vector<T> & v)
{
    for (size_t i=0; i<v.size(); ++i)
    {
        if (i>0)
        {
            os << L" ";
        }
        os << v[i];
    }
    return os;
}

// stream in a vector from a stream - vector contents must be streamable
template<class T>
std::wistream & operator >> (std::wistream & is, std::vector<T> & v)
{
    v.clear();
    while (!is.eof())
    {
        T t;
        is >> t;
        if (is.fail())
        {
            if (is.eof()) is.clear();
            break;
        }
        v.push_back(t);
    }
    return is;
}
