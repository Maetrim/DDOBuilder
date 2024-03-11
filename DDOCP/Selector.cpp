// Selector.cpp
//
#include "StdAfx.h"
#include "Selector.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Selector

namespace
{
    const wchar_t f_saxElementName[] = L"Selector";
    DL_DEFINE_NAMES(Selector_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Selector::Selector() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Selector_PROPERTIES)
}

DL_DEFINE_ACCESS(Selector_PROPERTIES)

XmlLib::SaxContentElementInterface * Selector::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Selector_PROPERTIES)

    return subHandler;
}

void Selector::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Selector_PROPERTIES)
}

void Selector::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Selector_PROPERTIES)
    writer->EndElement();
}

std::string Selector::SelectedIcon(const std::string & selectionName) const
{
    // iterate the selections and return the icon of the one that matches
    std::string icon = "NoImage";
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selectionName)
        {
            icon = (*it).Icon();
            break;
        }
        ++it;
    }
    return icon;
}

void Selector::RenderIcon(
        const std::string & selection,
        CDC * pDC,
        const CRect & itemRect) const
{
    // iterate the selections and get correct item to render its image
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            (*it).RenderIcon(pDC, itemRect);
            break;
        }
        ++it;
    }
}

bool Selector::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        ok &= (*it).VerifyObject(ss, trees, feats);
        ++it;
    }
    return ok;
}

std::string Selector::DisplayName(const std::string & selection) const
{
    std::string name;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            name = (*it).Name();
            break;          // done
        }
        ++it;
    }
    return name;
}

std::list<Effect> Selector::Effects(const std::string & selection) const
{
    std::list<Effect> effects;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            effects = (*it).Effects();
            break;          // done
        }
        ++it;
    }
    return effects;
}

std::list<DC> Selector::EffectDCs(const std::string & selection) const
{
    std::list<DC> dcs;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            dcs = (*it).EffectDC();
            break;          // done
        }
        ++it;
    }
    return dcs;
}

std::list<Stance> Selector::Stances(const std::string & selection) const
{
    std::list<Stance> stances;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            stances = (*it).Stances();
            break;          // done
        }
        ++it;
    }
    return stances;
}

size_t Selector::MinSpent(
        const std::string& selection,
        size_t defaultMinSpent) const
{
    size_t min = defaultMinSpent;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            if ((*it).HasMinSpent())
            {
                min = (*it).MinSpent();
            }
            break;          // done
        }
        ++it;
    }
    return min;
}

size_t Selector::Ranks(
        const std::string& selection,
        size_t defaultRanks) const
{
    size_t ranks = defaultRanks;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            if ((*it).HasRanks())
            {
                ranks = (*it).Ranks();
            }
            break;          // done
        }
        ++it;
    }
    return ranks;
}

bool Selector::CostVaries(const std::string& selection) const
{
    bool varies = false;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            varies = (*it).CostVaries();
            break;          // done
        }
        ++it;
    }
    return varies;
}

size_t Selector::Cost(const std::string& selection, size_t rank) const
{
    size_t cost = 0;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            cost = (*it).Cost(rank);
            break;          // done
        }
        ++it;
    }
    return cost;
}

const std::vector<size_t>& Selector::ItemCosts(const std::string& selection) const
{
    static std::vector<size_t> defaultCost(1, 1);
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            return (*it).CostPerRank();
        }
        ++it;
    }
    return defaultCost;
}

bool Selector::IsSelectionClickie(const std::string & selection) const
{
    bool isClickie = false;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            isClickie = (*it).HasClickie();
            break;          // done
        }
        ++it;
    }
    return isClickie;
}

bool Selector::RequiresEnhancement(
        const std::string& name,
        const std::string& selection,
        const std::string& subSelection) const
{
    bool bRequiresIt = false;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == subSelection)
        {
            if ((*it).HasRequirementsToTrain())
            {
                bRequiresIt = (*it).RequirementsToTrain().RequiresEnhancement(name, selection);
            }
            break;          // done
        }
        ++it;
    }
    return bRequiresIt;
}

bool Selector::HasTrainableOption(
        const Character & charData,
        size_t spentInTree) const
{
    bool bHasTrainableOption = false;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        bool optionTrainable = false;
        if ((*it).HasRequirementsToTrain())
        {
            optionTrainable = (*it).RequirementsToTrain().CanTrainEnhancement(charData, 0);
        }
        else
        {
            // if no requirements, then it is selectable
            optionTrainable = true;
        }
        if ((*it).HasMinSpent())
        {
            optionTrainable &= (spentInTree >= (*it).MinSpent());
        }
        bHasTrainableOption |= optionTrainable;
        ++it;
    }
    return bHasTrainableOption;
}

