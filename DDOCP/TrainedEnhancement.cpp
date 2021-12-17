// TrainedEnhancement.cpp
//
#include "StdAfx.h"
#include "TrainedEnhancement.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "EnhancementTreeItem.h"

#define DL_ELEMENT TrainedEnhancement

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedEnhancement";
    DL_DEFINE_NAMES(TrainedEnhancement_PROPERTIES)

    const unsigned f_verCurrent = 2;
}

TrainedEnhancement::TrainedEnhancement() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_requiredAps(0)
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

void TrainedEnhancement::AddRank(bool isTier5)
{
    ++m_Ranks;
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
}

void TrainedEnhancement::SetCost(const std::vector<size_t>& cost)
{
    m_cost = cost;
}

size_t TrainedEnhancement::Cost(size_t rank) const
{
    size_t cost = m_cost[0];
    if (rank < m_cost.size())
    {
        cost = m_cost[rank];
    }
    return cost;
}

size_t TrainedEnhancement::TotalCost() const
{
    size_t totalCost = 0;
    for (size_t i = 0; i < m_Ranks; ++i)
    {
        totalCost += Cost(i);
    }
    return totalCost;
}

void TrainedEnhancement::SetRequiredAps(size_t requiredAps)
{
    m_requiredAps = requiredAps;
}

size_t TrainedEnhancement::RequiredAps() const
{
    return m_requiredAps;
}

bool TrainedEnhancement::HasRequirementOf(const std::string& dependentOnEnhancementName) const
{
    bool bRequiresIt = false;
    // find the this trained enhancement to look at its requirements
    const EnhancementTreeItem *pItem = FindEnhancement(EnhancementName());
    if (pItem != NULL)
    {
        bRequiresIt = pItem->RequiresEnhancement(dependentOnEnhancementName);
    }

    return bRequiresIt;
}

bool TrainedEnhancement::operator<(const TrainedEnhancement & other) const
{
    return (RequiredAps() < other.RequiredAps());
}
