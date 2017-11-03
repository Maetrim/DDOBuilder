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
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
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
        std::vector<bool> * met) const
{
    m_RequirementsToTrain.CreateRequirementStrings(charData, requirements, met);
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
    // check the spell effects also
    std::list<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&lss);
        ++it;
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
        const std::string & treeName) const
{
    bool met = true;
    // must also meet the requirements of the item
    if (met)
    {
        const TrainedEnhancement * pTE = charData.IsTrained(Name(), "");
        met = m_RequirementsToTrain.CanTrainEnhancement(
                charData,
                (pTE == NULL) ? 0 : pTE->Ranks());
    }
    // cannot train this enhancement if its tier5 and not from the same tier 5
    // tree if one has already need trained
    if (HasTier5()                  // are we a tier 5 enhancement?
            && charData.HasTier5Tree()
            && treeName != charData.Tier5Tree())
    {
        // not allowed this tier 5 enhancement
        met = false;
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
    const TrainedEnhancement * te = charData.IsTrained(InternalName(), "");
    size_t trainedRanks = (te != NULL) ? te->Ranks() : 0;
    bool canTrain = (trainedRanks < Ranks());
    canTrain &= (spentInTree >= MinSpent());
    // verify and enhancements we are dependent on have enough trained ranks also
    canTrain &= m_RequirementsToTrain.CanTrainEnhancement(charData, trainedRanks);
    // must have enough action points to buy it
    size_t availableAP = charData.AvailableActionPoints(treeName, type);
    canTrain &= (availableAP >= Cost());
    return canTrain;
}

std::string EnhancementTreeItem::ActiveIcon(
        const Character & charData,
        bool * isActive) const
{
    std::string icon = "NoImage";
    // assume its the items root icon
    icon = Icon();
    // check to see if the enhancement is already trained
    const TrainedEnhancement * te = charData.IsTrained(InternalName(), "");
    *isActive = (te != NULL);
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

