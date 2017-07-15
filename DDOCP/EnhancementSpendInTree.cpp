// EnhancementSpendInTree.cpp
//
#include "StdAfx.h"
#include "EnhancementSpendInTree.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EnhancementSpendInTree

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementSpendInTree";
    DL_DEFINE_NAMES(EnhancementSpendInTree_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EnhancementSpendInTree::EnhancementSpendInTree() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pointsSpent(0)
{
    DL_INIT(EnhancementSpendInTree_PROPERTIES)
}

DL_DEFINE_ACCESS(EnhancementSpendInTree_PROPERTIES)

XmlLib::SaxContentElementInterface * EnhancementSpendInTree::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EnhancementSpendInTree_PROPERTIES)

    return subHandler;
}

void EnhancementSpendInTree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EnhancementSpendInTree_PROPERTIES)
}

void EnhancementSpendInTree::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EnhancementSpendInTree_PROPERTIES)
    writer->EndElement();
}

size_t EnhancementSpendInTree::TrainEnhancement(
        const std::string & enhancementName,
        const std::string & selection,
        size_t cost,
        bool isTier5,
        size_t * ranks)
{
    // determine the buy index of this enhancement training action so
    // that it can be revoked in the correct order if required
    size_t buyIndex = NextBuyIndex();
    // increment the ranks of this enhancement if already present
    TrainedEnhancement * item = NULL;
    // iterate the list to see if its present
    std::list<TrainedEnhancement>::iterator it = m_Enhancements.begin();
    while (item == NULL && it != m_Enhancements.end())
    {
        if ((*it).EnhancementName() == enhancementName)
        {
            item = &(*it);
            break;
        }
        ++it;
    }
    if (item != NULL)
    {
        item->AddRank(buyIndex, isTier5);
        m_pointsSpent += item->Cost();
    }
    else
    {
        TrainedEnhancement te;
        te.Set_EnhancementName(enhancementName);
        if (!selection.empty())
        {
            te.Set_Selection(selection);
        }
        te.AddRank(buyIndex, isTier5);  // create the first rank trained
        te.SetCost(cost);               // cached value
        m_Enhancements.push_back(te);
        item = &m_Enhancements.back();
        m_pointsSpent += cost;
    }
    *ranks = item->Ranks();
    return item->Cost();
}

int EnhancementSpendInTree::RevokeEnhancement(
        std::string * revokedEnhancement,
        std::string * revokedEnhancementSelection,
        size_t * ranks)
{
    int ap = 0;     // return number of AP recovered
    // look for the correct enhancement to revoke
    size_t buyIndex = NextBuyIndex();
    if (buyIndex > 0)
    {
        --buyIndex;     // we need the last one not the next one
        // find the enhancement to revoke a rank on
        TrainedEnhancement * item = NULL;
        // iterate the list to see if its present
        std::list<TrainedEnhancement>::iterator it = m_Enhancements.begin();
        while (item == NULL && it != m_Enhancements.end())
        {
            if ((*it).IsYourBuyIndex(buyIndex))
            {
                item = &(*it);
                break;
            }
            ++it;
        }
        // decrement the ranks of this enhancement if already present, remove if ranks == 0
        if (item != NULL)
        {
            *revokedEnhancement = item->EnhancementName();
            if (item->HasSelection())
            {
                *revokedEnhancementSelection = item->Selection();
            }
            else
            {
                *revokedEnhancementSelection = "";
            }
            *ranks = item->Ranks();     // return the rank being revoked
            ap = item->Cost();
            m_pointsSpent -= ap;    // return AP for use
            item->RevokeRank();
            if (item->Ranks() == 0)
            {
                // fully revokes, remove item from list
                m_Enhancements.erase(it);
            }
        }
    }
    return ap;
}

size_t EnhancementSpendInTree::NextBuyIndex() const
{
    size_t buyIndex = 0;
    // iterate all previous buys and count ranks in each
    std::list<TrainedEnhancement>::const_iterator it = m_Enhancements.begin();
    while (it != m_Enhancements.end())
    {
        buyIndex += (*it).Ranks();
        ++it;
    }
    return buyIndex;
}

bool EnhancementSpendInTree::HasTier5() const
{
    // return true if any of the trained enhancements are tier 5
    bool hasTier5 = false;
    std::list<TrainedEnhancement>::const_iterator it = m_Enhancements.begin();
    while (it != m_Enhancements.end())
    {
        if ((*it).HasIsTier5())
        {
            hasTier5 = true;
            break;  // no need to check the rest
        }
        ++it;
    }
    return hasTier5;
}

size_t EnhancementSpendInTree::Spent() const
{
    return m_pointsSpent;
}

void EnhancementSpendInTree::SetSpent(size_t apsSpent)
{
    m_pointsSpent = apsSpent;
}

