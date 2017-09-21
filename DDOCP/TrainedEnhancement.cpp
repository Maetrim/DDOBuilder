// TrainedEnhancement.cpp
//
#include "StdAfx.h"
#include "TrainedEnhancement.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT TrainedEnhancement

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedEnhancement";
    DL_DEFINE_NAMES(TrainedEnhancement_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TrainedEnhancement::TrainedEnhancement() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_cost(0)
{
    DL_INIT(TrainedEnhancement_PROPERTIES)
}

DL_DEFINE_ACCESS(TrainedEnhancement_PROPERTIES)

XmlLib::SaxContentElementInterface * TrainedEnhancement::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(TrainedEnhancement_PROPERTIES)

    return subHandler;
}

void TrainedEnhancement::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(TrainedEnhancement_PROPERTIES)
}

void TrainedEnhancement::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(TrainedEnhancement_PROPERTIES)
    writer->EndElement();
}

void TrainedEnhancement::AddRank(size_t buyIndex, bool isTier5)
{
    ++m_Ranks;
    m_BuyIndex.push_back(buyIndex);
    if (isTier5)
    {
        Set_IsTier5();
    }
    else
    {
        Clear_IsTier5();
    }
}

void TrainedEnhancement::RevokeRank()
{
    --m_Ranks;
    m_BuyIndex.pop_back();
}

bool TrainedEnhancement::IsLastBuyIndex(size_t buyIndex) const
{
    // return true if buyIndex is the last element in our m_BuyIndex vector
    bool isUs = false;
    if (m_BuyIndex.size() > 0)
    {
        if (m_BuyIndex.at(m_BuyIndex.size() - 1) == buyIndex)
        {
            isUs = true;
        }
    }
    return isUs;
}

bool TrainedEnhancement::HasBuyIndex(size_t buyIndex) const
{
    // return true if the given buyIndex is present in m_BuyIndex
    BOOL isUs = false;
    for (size_t i = 0; i < m_BuyIndex.size(); ++i)
    {
        if (m_BuyIndex[i] == buyIndex)
        {
            isUs = true;
        }
    }
    return isUs;
}

void TrainedEnhancement::SetCost(size_t cost)
{
    m_cost = cost;
}

size_t TrainedEnhancement::Cost() const
{
    return m_cost;
}
