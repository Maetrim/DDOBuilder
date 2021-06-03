// FeatGroup.cpp
//
#include "StdAfx.h"
#include "FeatGroup.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT FeatGroup

namespace
{
    const wchar_t f_saxElementName[] = L"FeatGroup";
    DL_DEFINE_NAMES(FeatGroup_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

FeatGroup::FeatGroup() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(FeatGroup_PROPERTIES)
}

DL_DEFINE_ACCESS(FeatGroup_PROPERTIES)

XmlLib::SaxContentElementInterface * FeatGroup::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(FeatGroup_PROPERTIES)

    return subHandler;
}

void FeatGroup::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(FeatGroup_PROPERTIES)
}

void FeatGroup::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(FeatGroup_PROPERTIES)
    writer->EndElement();
}

bool FeatGroup::VerifyObject(std::stringstream * ss) const
{
    // if a feat has a grouping object, at least one of the flags must be set for it
    bool ok = HasIsAasimarBond()
            || HasIsAlchemistBonus()
            || HasIsAnimalisticAspect()
            || HasIsArtificerBonus()
            || HasIsBattle()
            || HasIsBelovedOf()
            || HasIsChildOf()
            || HasIsDamageReduction()
            || HasIsDilettante()
            || HasIsDeity()
            || HasIsDomain()
            || HasIsDomainFeat()
            || HasIsDragonbornRacial()
            || HasIsDruidWildShape()
            || HasIsEnergyResistance()
            || HasIsEpicDestinyFeat()
            || HasIsEpicFeat()
            || HasIsFavoredEnemy()
            || HasIsFighterBonus()
            || HasIsFollowerOf()
            || HasIsHeart()
            || HasIsRogueSpecialAbility()
            || HasIsStandardFeat()
            || HasIsLegendaryFeat()
            || HasIsMonkBonus()
            || HasIsMonkBonus6()
            || HasIsMonkPhilosphy()
            || HasIsMetamagic()
            || HasIsWarlockPact()
            || HasIsWarlockPactAbility()
            || HasIsWarlockResistance()
            || HasIsWarlockSaveBonus()
            || HasIsWarlockPactSpell()
            || HasIsKoboldTribe();
    if (!ok)
    {
        (*ss) << "FeatGroup has no grouping elements\n";
    }
    return ok;
}

