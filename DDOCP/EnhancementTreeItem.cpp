// EnhancementTreeItem.cpp
//
#include "StdAfx.h"
#include "EnhancementTreeItem.h"
#include "XmlLib\SaxWriter.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT EnhancementTreeItem

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementTreeItem";
    DL_DEFINE_NAMES(EnhancementTreeItem_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EnhancementTreeItem::EnhancementTreeItem() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_bImageLoaded(false),
    m_bDisabledImageLoaded(false),
    m_type(TT_unknown)
{
    DL_INIT(EnhancementTreeItem_PROPERTIES)
}

DL_DEFINE_ACCESS(EnhancementTreeItem_PROPERTIES)

XmlLib::SaxContentElementInterface * EnhancementTreeItem::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EnhancementTreeItem_PROPERTIES)
    return subHandler;
}

void EnhancementTreeItem::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EnhancementTreeItem_PROPERTIES)
}

void EnhancementTreeItem::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EnhancementTreeItem_PROPERTIES)
    writer->EndElement();
}

void EnhancementTreeItem::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    m_RequirementsToTrain.CreateRequirementStrings(charData, requirements, met, level);
}

bool EnhancementTreeItem::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    std::stringstream lss;
    if (!ImageFileExists(IT_enhancement, Icon()))
    {
        lss << "EnhancementTreeItem is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    ok &= m_RequirementsToTrain.VerifyObject(&lss, trees, feats);
    if (HasSelections())
    {
        // check each of the selections also
        ok &= m_Selections.VerifyObject(&lss, trees, feats);
    }
    // check the effects also
    std::list<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&lss);
        ++it;
    }
    // verify its DC objects
    std::vector<DC>::const_iterator edcit = m_EffectDC.begin();
    while (edcit != m_EffectDC.end())
    {
        ok &= (*edcit).VerifyObject(ss);
        ++edcit;
    }
    if (!ok)
    {
        (*ss) << "---" << m_Name << "\n";
        (*ss) << lss.str();
    }
    return ok;
}

bool EnhancementTreeItem::MeetRequirements(
        const Character & charData,
        const std::string & selection,
        const std::string & treeName,
        size_t spentInTree) const
{
    bool met = true;
    // must also meet the requirements of the item
    if (met)
    {
        const TrainedEnhancement * pTE = charData.IsTrained(Name(), "", m_type);
        met = m_RequirementsToTrain.CanTrainEnhancement(
                charData,
                (pTE == NULL) ? 0 : pTE->Ranks());
    }
    // cannot train this enhancement if its tier5 and not from the same tier 5
    // tree if one has already been trained
    if (m_type == TT_enhancement
            || m_type == TT_universal)
    {
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasTier5Tree()
                && treeName != charData.Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            met = false;
        }
    }
    if (m_type == TT_epicDestiny)
    {
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasU51Destiny_Tier5Tree()
                && treeName != charData.U51Destiny_Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            met = false;
        }
    }
    if (met && selection != "")
    {
        // check if we can train this selection
        std::list<EnhancementSelection> selections = m_Selections.Selections();
        std::list<EnhancementSelection>::const_iterator it = selections.begin();
        while (it != selections.end())
        {
            if ((*it).Name() == selection)
            {
                // this is the one we need to check
                if ((*it).HasMinSpent())
                {
                    met &= ((*it).MinSpent() <= spentInTree);
                }
                if ((*it).HasRequirementsToTrain())
                {
                    met &= (*it).RequirementsToTrain().CanTrainEnhancement(charData, 0);
                }
                // and were done
                break;
            }
            ++it;
        }
    }
    return met;
}

bool EnhancementTreeItem::IsTier5Blocked(
    const Character & charData,
    const std::string& treeName) const
{
    bool bTier5Blocked = false;
    switch (m_type)
    {
    case TT_enhancement:
    case TT_universal:
        // cannot train this enhancement if its tier5 and not from the same tier 5
        // tree if one has already been trained
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasTier5Tree()
                && treeName != charData.Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            bTier5Blocked = true;
        }
        break;
    case TT_epicDestiny:
        // cannot train this enhancement if its tier5 and not from the same tier 5
        // tree if one has already been trained
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasU51Destiny_Tier5Tree()
                && treeName != charData.U51Destiny_Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            bTier5Blocked = true;
        }
        break;
    }
    return bTier5Blocked;
}

bool EnhancementTreeItem::IsAllowed(
        const Character & charData,
        const std::string & selection,
        const std::string & treeName,
        size_t spentInTree) const
{
    bool met = true;
    // must also meet a subset of the requirements of the item
    if (met)
    {
        const TrainedEnhancement * pTE = charData.IsTrained(Name(), "", m_type);
        met = m_RequirementsToTrain.IsAllowed(
                charData,
                (pTE == NULL) ? 0 : pTE->Ranks());
    }
    // cannot train this enhancement if its tier5 and not from the same tier 5
    if (m_type == TT_enhancement
            || m_type == TT_universal)
    {
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasTier5Tree()
                && treeName != charData.Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            met = false;
        }
    }
    if (m_type == TT_epicDestiny)
    {
        if (HasTier5()                  // are we a tier 5 enhancement?
                && charData.HasU51Destiny_Tier5Tree()
                && treeName != charData.U51Destiny_Tier5Tree())
        {
            // not allowed this tier 5 enhancement
            met = false;
        }
    }
    if (met && selection != "")
    {
        // check if we can train this selection
        std::list<EnhancementSelection> selections = m_Selections.Selections();
        std::list<EnhancementSelection>::const_iterator it = selections.begin();
        while (it != selections.end())
        {
            if ((*it).Name() == selection)
            {
                // this is the one we need to check
                if ((*it).HasMinSpent())
                {
                    met &= ((*it).MinSpent() <= spentInTree);
                }
                if ((*it).HasRequirementsToTrain())
                {
                    met = (*it).RequirementsToTrain().IsAllowed(charData, 0);
                }
                // and were done
                break;
            }
            ++it;
        }
    }
    return met;
}

bool EnhancementTreeItem::CanTrain(
        const Character & charData,
        const std::string & treeName,
        size_t spentInTree,
        TreeType type) const
{
    // must not be trained to max ranks
    const TrainedEnhancement * te = charData.IsTrained(InternalName(), "", m_type);
    size_t trainedRanks = (te != NULL) ? te->Ranks() : 0;
    std::string selection = (te != NULL && te->HasSelection()) ? te->Selection() : "";
    bool canTrain = (trainedRanks < Ranks(selection));
    canTrain &= (spentInTree >= MinSpent(selection));
    // if we have no selection, and we have a selector, only enabled if at least
    // one of the sub selector items can be trained
    if (selection == ""
            && HasSelections())
    {
        canTrain &= Selections().HasTrainableOption(charData, spentInTree);
    }
    // verify and enhancements we are dependent on have enough trained ranks also
    canTrain &= m_RequirementsToTrain.CanTrainEnhancement(charData, trainedRanks);
    // must have enough action points to buy it
    int availableAP = charData.AvailableActionPoints(treeName, type);
    canTrain &= (availableAP >= (int)Cost(selection, trainedRanks));
    return canTrain;
}

bool EnhancementTreeItem::CanRevoke(
        const SpendInTree* pTreeSpend) const
{
    bool canRevoke = false;
    // This enhancement cannot be revoked if:
    if (pTreeSpend != NULL)
    {
        // 1. No ranks have been spent in this enhancement
        std::string selection;
        size_t ranks = pTreeSpend->TrainedRanks(InternalName(), &selection);
        if (ranks != 0)
        {
            // 2. Revoking a rank of this enhancement would reduce the APs spent in the tree
            // below those required for a higher tier enhancement to be trained

            // sum how many Aps have been spent in this min Ap tier
            canRevoke = pTreeSpend->CanRevokeAtTier(MinSpent(selection), Cost(selection, ranks));

            if (canRevoke)
            {
                // 3. Another enhancement that is dependent on this enhancement being trained has
                // equal or more ranks than this enhancement
                canRevoke = !pTreeSpend->HasTrainedDependants(InternalName(), selection, ranks);
            }
        }
    }
    return canRevoke;
}

void EnhancementTreeItem::RenderIcon(
        const Character & charData,
        CDC * pDC,
        const CRect & itemRect) const
{
    std::string icon = "NoImage";
    // assume its the items root icon
    icon = Icon();
    // check to see if the enhancement is already trained
    const TrainedEnhancement * te = charData.IsTrained(InternalName(), "", m_type);
    if (te != NULL)
    {
        if (te->HasSelection())
        {
            std::string sel = te->Selection();
            m_Selections.RenderIcon(sel, pDC, itemRect);
        }
        else
        {
            if (!m_bImageLoaded)
            {
                // load the display image for this item
                LoadImageFile(IT_enhancement, m_Icon, &m_image);
                m_image.SetTransparentColor(c_transparentColour);
                m_bImageLoaded = true;
            }
            m_image.TransparentBlt(
                    pDC->GetSafeHdc(),
                    itemRect.left,
                    itemRect.top,
                    itemRect.Width(),
                    itemRect.Height());
        }
    }
    else
    {
        if (!m_bDisabledImageLoaded)
        {
            // load the display image for this item
            LoadImageFile(IT_enhancement, m_Icon, &m_disabledImage);
            // its disabled, so grey scale
            MakeGrayScale(&m_disabledImage, c_transparentColour);
            m_bDisabledImageLoaded = true;
        }
        m_disabledImage.TransparentBlt(
                pDC->GetSafeHdc(),
                itemRect.left,
                itemRect.top,
                itemRect.Width(),
                itemRect.Height());
    }
}

std::string EnhancementTreeItem::DisplayName(
        const std::string & selection) const
{
    std::string name;
    name = Name();
    if (HasSelections())
    {
        // we need to look up the effects for a selection
        name += ": ";
        name += m_Selections.DisplayName(selection);
    }
    return name;
}

std::string EnhancementTreeItem::ActiveIcon(
        const Character & charData) const
{
    std::string icon = "NoImage";
    // assume its the items root icon
    icon = Icon();
    // check to see if the enhancement is already trained
    const TrainedEnhancement * te = charData.IsTrained(InternalName(), "", m_type);
    if (te != NULL)
    {
        if (te->HasSelection())
        {
            // get the selector icon instead
            std::string sel = te->Selection();
            icon = m_Selections.SelectedIcon(sel);
        }
    }
    return icon;
}

std::list<Effect> EnhancementTreeItem::ActiveEffects(
        const std::string & selection) const
{
    // an enhancement may have specific sub-selection effects
    std::list<Effect> effects;
    if (HasSelections())
    {
        // we need to look up the effects for a selection
        effects = m_Selections.Effects(selection);
    }
    // even if it had a sub-selection it may still have effects that
    // always apply regardless of the sub-selection
    effects.insert(effects.end(), m_Effects.begin(), m_Effects.end());
    return effects;
}

std::list<DC> EnhancementTreeItem::ActiveDCs(const std::string & selection) const
{
    // an enhancement may have specific sub-selection DCs
    std::list<DC> dcs;
    if (HasSelections())
    {
        // we need to look up the DCs for a selection
        dcs = m_Selections.EffectDCs(selection);
    }
    // even if it had a sub-selection it may still have DCs that
    // always apply regardless of the sub-selection
    dcs.insert(dcs.end(), m_EffectDC.begin(), m_EffectDC.end());
    return dcs;
}

std::list<Stance> EnhancementTreeItem::Stances(const std::string & selection) const
{
    // an enhancement may have specific sub-selection stances
    std::list<Stance> stances;
    if (HasSelections())
    {
        // we need to look up the effects for a selection
        stances = m_Selections.Stances(selection);
    }
    // even if it had a sub-selection it may still have stances that
    // always apply regardless of the sub-selection
    stances.insert(stances.end(), m_Stances.begin(), m_Stances.end());
    return stances;
}

bool EnhancementTreeItem::CostVaries(const std::string& selection) const
{
    bool varies = false;
    if (selection != ""
            && HasSelections())
    {
        varies = m_Selections.CostVaries(selection);
    }
    else
    {
        size_t cost = m_CostPerRank[0];
        for (size_t i = 1; i < m_CostPerRank.size(); ++i)
        {
            varies |= (m_CostPerRank[i] != cost);
        }
    }
    return varies;
}

size_t EnhancementTreeItem::Cost(
        const std::string& selection,
        size_t rank) const
{
    size_t cost = 0;
    if (rank < m_CostPerRank.size())
    {
        cost = m_CostPerRank[rank];
    }
    else
    {
        cost = m_CostPerRank[0];
    }
    if (selection != ""
            && HasSelections())
    {
        cost = m_Selections.Cost(selection, rank);
    }
    return cost;
}

const std::vector<size_t>& EnhancementTreeItem::ItemCosts(
        const std::string& selection) const
{
    if (selection != ""
            && HasSelections())
    {
        return m_Selections.ItemCosts(selection);
    }
    else
    {
        return m_CostPerRank;
    }
}

bool EnhancementTreeItem::IsSelectionClickie(const std::string & selection) const
{
    return m_Selections.IsSelectionClickie(selection);
}

bool EnhancementTreeItem::RequiresEnhancement(
        const std::string& name,
        const std::string& selection,
        const std::string& subSelection) const
{
    bool bRequiresIt = m_RequirementsToTrain.RequiresEnhancement(name, selection);
    // need to also check any selection if that has different requirements
    if (selection != "")
    {
        bRequiresIt |= m_Selections.RequiresEnhancement(name, selection, subSelection);
    }
    return bRequiresIt;
}

size_t EnhancementTreeItem::MinSpent(const std::string& selection) const
{
    size_t min = MinSpent();    // assume default
    if (selection != "")
    {
        // find the selection and use its MinSepnt if it has one
        min = m_Selections.MinSpent(selection, min);    // returns min if no MinSpent on item or not found
    }
    return min;
}

size_t EnhancementTreeItem::Ranks(const std::string& selection) const
{
    size_t ranks = Ranks();    // assume default
    if (selection != "")
    {
        // find the selection and use its MinSepnt if it has one
        ranks = m_Selections.Ranks(selection, ranks);
    }
    return ranks;
}
