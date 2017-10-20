// EpicDestinySpendInTree.cpp
//
#include "StdAfx.h"
#include "EpicDestinySpendInTree.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EpicDestinySpendInTree

namespace
{
    const wchar_t f_saxElementName[] = L"EpicDestinySpendInTree";
    DL_DEFINE_NAMES(EpicDestinySpendInTree_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EpicDestinySpendInTree::EpicDestinySpendInTree() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pointsSpent(0)
{
    DL_INIT(EpicDestinySpendInTree_PROPERTIES)
}

DL_DEFINE_ACCESS(EpicDestinySpendInTree_PROPERTIES)

XmlLib::SaxContentElementInterface * EpicDestinySpendInTree::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EpicDestinySpendInTree_PROPERTIES)

    return subHandler;
}

void EpicDestinySpendInTree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EpicDestinySpendInTree_PROPERTIES)
    // fix old files with incorrect typo in tree name
    if (m_TreeName == "Gransmaster of Flowers")
    {
        m_TreeName = "Grandmaster of Flowers";
    }
}

void EpicDestinySpendInTree::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EpicDestinySpendInTree_PROPERTIES)
    writer->EndElement();
}

size_t EpicDestinySpendInTree::TrainEnhancement(
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

int EpicDestinySpendInTree::RevokeEnhancement(
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
            if ((*it).IsLastBuyIndex(buyIndex))
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

size_t EpicDestinySpendInTree::NextBuyIndex() const
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

size_t EpicDestinySpendInTree::Spent() const
{
    return m_pointsSpent;
}

void EpicDestinySpendInTree::SetSpent(size_t apsSpent)
{
    m_pointsSpent = apsSpent;
}

