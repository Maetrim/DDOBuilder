// VectorConversion.h
//
#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace XmlLib
{
    template <typename T>
    bool VectorToString(const std::vector<T> & v, std::wstring * out)
    {
        std::wstringstream ss;
        ss << v;
        *out = ss.str();
        return !ss.fail();
    }

    template <>
    bool VectorToString(const std::vector<int> & v, std::wstring * out);

    template <>
    bool VectorToString(const std::vector<double> & v, std::wstring * out);
    
    template <>
    bool VectorToString(const std::vector<BYTE> & v, std::wstring * out);

    template <>
    bool VectorToString(const std::vector<bool> & v, std::wstring * out);

    // overload to allow control of precision (num decimal places)
    template <typename T>
    bool VectorToString(const std::vector<T> & v, size_t precision, std::wstring * out)
    {
        return VectorToString(v, out);
    }
    template <>
    bool VectorToString(const std::vector<double> & v, size_t precision, std::wstring * out);

    template <typename T>
    bool StringToVector(const std::wstring & s, std::vector<T> * out)
    {
        std::wstringstream ss(s);
        ss >> *out;
        return !ss.fail();
    }

    template <>
    bool StringToVector(const std::wstring & s, std::vector<int> * out);

    template <>
    bool StringToVector(const std::wstring & s, std::vector<double> * out);

    template <>
    bool StringToVector(const std::wstring & s, std::vector<BYTE> * out);

    template <>
    bool StringToVector(const std::wstring & s, std::vector<bool> * out);
}


