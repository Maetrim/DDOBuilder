// VectorConversion.cpp
//
#include "stdafx.h"
#include "XmlLib\VectorConversion.h"
#include <math.h>
#include <limits>

///////////////////////////////////////////////////////////////////////////////
// VectorToString
namespace
{
    const std::wstring f_infinity = L"Infinity";
    const std::wstring f_notANumber = L"Nan";
}

template <>
bool XmlLib::VectorToString(const std::vector<int> & v, std::wstring * out)
{
    bool ok = true;
    const int maxAddSize = 20;
    size_t size = 0;
    out->resize(size);
    out->reserve(v.size() * 3);
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (out->size() < size + maxAddSize)
        {
            out->resize(size * 2 + maxAddSize);
        }
        wchar_t * p = &*out->begin() + size;
        if (i > 0)
        {
            *p = ' ';
            ++size;
            ++p;
        }
        _itow_s(v[i], p, out->size() - size, 10);
        size += wcslen(p);
    }
    out->resize(size);
    return ok;
}

template <>
bool XmlLib::VectorToString(const std::vector<double> & v, std::wstring * out)
{
    bool ok = true;
    const int maxAddSize = 30;
    size_t size = 0;
    out->resize(size);
    out->reserve(v.size() * 3);
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (out->size() < size + maxAddSize)
        {
            out->resize(size * 2 + maxAddSize);
        }
        wchar_t * p = &*out->begin() + size;
        if (i > 0)
        {
            *p = ' ';
            ++size;
            ++p;
        }
        double value = v[i];
        if (_isnan(value))
        {
            wcscpy_s(p, out->size() - size, f_notANumber.c_str());
        }
        else if (!_finite(value))
        {
            wcscpy_s(p, out->size() - size, f_infinity.c_str());
        }
        else
        {
            swprintf(p, out->size() - size, L"%g", value);
        }
        size += wcslen(p);
    }
    out->resize(size);
    return ok;
}

// overload to allow control of precision (num decimal places)
template <>
bool XmlLib::VectorToString(const std::vector<double> & v, size_t precision, std::wstring * out)
{
    bool ok = true;
    const int maxAddSize = 30;
    wchar_t format[10];
    swprintf(format, sizeof(format), L"%%.%ug", precision);
    size_t size = 0;
    out->resize(size);
    out->reserve(v.size() * 3);
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (out->size() < size + maxAddSize)
        {
            out->resize(size * 2 + maxAddSize);
        }
        wchar_t * p = &*out->begin() + size;
        if (i > 0)
        {
            *p = ' ';
            ++size;
            ++p;
        }
        double value = v[i];
        if (_isnan(value))
        {
            wcscpy_s(p, out->size() - size, f_notANumber.c_str());
        }
        else if (!_finite(value))
        {
            wcscpy_s(p, out->size() - size, f_infinity.c_str());
        }
        else
        {
            swprintf(p, out->size() - size, format, value);
        }
        size += wcslen(p);
    }
    out->resize(size);
    return ok;
}

template <>
bool XmlLib::VectorToString(const std::vector<BYTE> & v, std::wstring * out)
{
    bool ok = true;
    const int maxAddSize = 20;
    size_t size = 0;
    out->resize(size);
    out->reserve(v.size() * 3);
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (out->size() < size + maxAddSize)
        {
            out->resize(size * 2 + maxAddSize);
        }
        wchar_t * p = &*out->begin() + size;
        if (i > 0)
        {
            *p = ' ';
            ++size;
            ++p;
        }
        _itow_s(v[i], p, out->size() - size, 10);
        size += wcslen(p);
    }
    out->resize(size);
    return ok;
}

template <>
bool XmlLib::VectorToString(const std::vector<bool> & v, std::wstring * out)
{
    bool ok = true;
    size_t size = v.size();
    out->resize(size);
    if (size > 0)
    {
        wchar_t * p = &*out->begin();
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (v[i])
            {
                *p = '1';
            }
            else
            {
                *p = '0';
            }
            ++p;
        }
    }
    return ok;
}

///////////////////////////////////////////////////////////////////////////////
// StringToVector

template <>
bool XmlLib::StringToVector(const std::wstring & s, std::vector<int> * out)
{
    bool ok = true;

    int val = 0;
    bool started = false;
    bool negative = false;
    bool hadSign = false;

    for (const wchar_t * p = s.c_str(); *p != '\0' && ok; ++p)
    {
        if (!started && !hadSign && *p == '-')
        {
            hadSign = true;
            negative = true;
        }
        else if (!started && !hadSign && *p == '+')
        {
            hadSign = true;
        }
        else if (*p == ' ' || *p == '\n' || *p == '\t')
        {
            // whitespace
            if (started)
            {
                out->push_back(negative?-val:val);
                val = 0;
                hadSign = false;
                negative = false;
                started = false;
            }
        }
        else if (*p >= '0' && *p <= '9')
        {
            if (started)
            {
                val *= 10;
                val += *p - '0';
            }
            else
            {
                val = *p - '0';
                started = true;
            }
        }
        else
        {
            ok = false;
        }
    }
    if (started && ok)
    {
        out->push_back(negative?-val:val);
    }

    return ok;
}

template <>
bool XmlLib::StringToVector(const std::wstring & s, std::vector<double> * out)
{
    bool ok = true;
    out->clear();

    __int64 valInt = 0;
    __int64 valDec = 0;
    __int64 divDec = 1;
    __int64 valExp = 0;

    bool negVal = false;
    bool negExp = false;
    size_t specialIndex = 0;
    enum
    {
        unstarted,
        sign,
        integer,
        point,
        decimal,
        expchar,
        expsign,
        expinteger,
        infinity,
        notAnumber
    } stage = unstarted;

    for (const wchar_t * p = s.c_str(); *p != '\0' && ok; ++p)
    {
        switch (stage)
        {
            case unstarted:
                if (*p == ' ' || *p == '\n' || *p == '\t')
                {
                    // ignore blanks, new lines and tabs
                    break;
                }
                else if (*p == '-')
                {
                    negVal = true;
                    stage = sign;
                    break;
                }
                else if (*p == '+')
                {
                    stage = sign;
                    break;
                }
                else if (*p == f_infinity.at(0))
                {
                    stage = infinity;
                    ++specialIndex;
                    break;
                }
                else if (*p == f_notANumber.at(0))
                {
                    stage = notAnumber;
                    ++specialIndex;
                    break;
                }
                // FALL THROUGH
            case sign:
                if (*p >= '0' && *p <= '9')
                {
                    valInt = *p - '0';
                    stage = integer;
                    break;
                }
                else if (*p == '.')
                {
                    valInt = 0;
                    stage = point;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            case integer:
            {
                if (*p >= '0' && *p <= '9')
                {
                    valInt = valInt * 10 + *p - '0';
                    break;
                }
                else if (*p == '.')
                {
                    stage = point;
                    break;
                }
                else if (*p == 'e' || *p == 'E')
                {
                    valDec = 0;
                    divDec = 1;
                    stage = expchar;
                    break;
                }
                else if (*p == ' ' || *p == '\n' || *p == '\t')
                {
                    double val = double(negVal ? -valInt : valInt);
                    out->push_back(val);
                    stage = unstarted;
                    negVal = false;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
            case point:
            {
                if (*p >= '0' && *p <= '9')
                {
                    valDec = *p - '0';
                    divDec = 10;
                    stage = decimal;
                    break;
                }
                else if (*p == 'e' || *p == 'E')
                {
                    stage = expchar;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
            case decimal:
            {
                if (*p >= '0' && *p <= '9')
                {
                    valDec = valDec * 10 + *p - '0';
                    divDec *= 10;
                    break;
                }
                else if (*p == 'e' || *p == 'E')
                {
                    stage = expchar;
                    break;
                }
                else if (*p == ' ' || *p == '\n' || *p == '\t')
                {
                    double val = valInt + valDec / double(divDec);
                    out->push_back(negVal ? -val : val);
                    stage = unstarted;
                    negVal = false;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
            case expchar:
            {
                if (*p == '-')
                {
                    stage = expsign;
                    negExp = true;
                    break;
                }
                else if (*p == '+')
                {
                    stage = expsign;
                    break;
                }
                // FALL THROUGH
            }
            case expsign:
            {
                if (*p >= '0' && *p <= '9')
                {
                    valExp = *p - '0';
                    stage = expinteger;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
            case expinteger:
            {
                if (*p >= '0' && *p <= '9')
                {
                    valExp = valExp * 10 + *p - '0';
                    break;
                }
                else if (*p == ' ' || *p == '\n' || *p == '\t')
                {
                    double val = valInt + valDec / double(divDec);
                    double exp = pow(10.0, double(negExp ? -valExp : valExp));
                    out->push_back(negVal ? -val*exp : val*exp);
                    stage = unstarted;
                    negVal = negExp = false;
                    break;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
            break;
            case infinity:
            {
                if (specialIndex == f_infinity.length())
                {
                    out->push_back(std::numeric_limits<double>::infinity());
                    stage = unstarted;
                    specialIndex = 0;
                }
                else if (*p == f_infinity.at(specialIndex))
                {
                    ++specialIndex;
                }
                else
                {
                    ok = false;
                }
                break;
            }
            case notAnumber:
            {
                if (specialIndex == f_notANumber.length())
                {
                    out->push_back(std::numeric_limits<double>::signaling_NaN());
                    stage = unstarted;
                    specialIndex = 0;
                }
                else if (*p == f_notANumber.at(specialIndex))
                {
                    ++specialIndex;
                }
                else
                {
                    ok = false;
                }
                break;
            }
        }
    }

    if (ok)
    {
        switch (stage)
        {
            case unstarted:
                break;
            case sign:
            {
                ok = false;
                break;
            }
            case integer:
            {
                double val = double(negVal ? -valInt : valInt);
                out->push_back(val);
                break;
            }
            case point:
            {
                ok = false;
                break;
            }
            case decimal:
            {
                double val = valInt + valDec / double(divDec);
                out->push_back(negVal ? -val : val);
                break;
            }
            case expchar:
            {
                ok = false;
                break;
            }
            case expsign:
            {
                ok = false;
                break;
            }
            case expinteger:
            {
                double val = valInt + valDec / double(divDec);
                double exp = pow(10.0, double(negExp ? -valExp : valExp));
                out->push_back(negVal ? -val*exp : val*exp);
                break;
            }
            case infinity:
            {
                if (specialIndex == f_infinity.length())
                {
                    out->push_back(std::numeric_limits<double>::infinity());
                }
                else
                {
                    ok = false;
                }
                break;
            }
            case notAnumber:
            {
                if (specialIndex == f_notANumber.length())
                {
                    out->push_back(std::numeric_limits<double>::signaling_NaN());
                }
                else
                {
                    ok = false;
                }
                break;
            }
        }
    }

    return ok;
}

template <>
bool XmlLib::StringToVector(const std::wstring & s, std::vector<BYTE> * out)
{
    bool ok = true;

    int val = 0;
    bool started = false;

    for (const wchar_t * p = s.c_str(); *p != '\0' && ok; ++p)
    {
        if (*p == ' ' || *p == '\n' || *p == '\t')
        {
            // whitespace
            if (started)
            {
                out->push_back(val);
                val = 0;
                started = false;
            }
        }
        else if (*p >= '0' && *p <= '9')
        {
            if (started)
            {
                val *= 10;
                val += *p - '0';
            }
            else
            {
                val = *p - '0';
                started = true;
            }
        }
        else
        {
            ok = false;
        }
    }
    if (started && ok)
    {
        out->push_back(val);
    }

    return ok;
}

template <>
bool XmlLib::StringToVector(const std::wstring & s, std::vector<bool> * out)
{
    bool ok = true;

    for (const wchar_t * p = s.c_str(); *p != '\0' && ok; ++p)
    {
        if (*p == '1')
        {
            out->push_back(true);
        }
        else if (*p == '0')
        {
            out->push_back(false);
        }
        else if (*p == ' ')
        {
            // skip whitespace (old files save as integers of 0/1) "0 1 1 0"
        }
        else
        {
            ok = false;
        }
    }

    return ok;
}
///////////////////////////////////////////////////////////////////////////////
