// FeatGroup.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class FeatGroup :
    public XmlLib::SaxContentElement
{
    public:
        FeatGroup(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(std::stringstream * ss) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define FeatGroup_PROPERTIES(_) \
                DL_FLAG(_, IsAasimarBond) \
                DL_FLAG(_, IsAlchemistBonus) \
                DL_FLAG(_, IsArtificerBonus) \
                DL_FLAG(_, IsBattle) \
                DL_FLAG(_, IsBelovedOf) \
                DL_FLAG(_, IsChildOf) \
                DL_FLAG(_, IsDamageReduction) \
                DL_FLAG(_, IsDilettante) \
                DL_FLAG(_, IsDeity) \
                DL_FLAG(_, IsDomain) \
                DL_FLAG(_, IsDomainFeat) \
                DL_FLAG(_, IsDragonbornRacial) \
                DL_FLAG(_, IsDruidWildShape) \
                DL_FLAG(_, IsEnergyResistance) \
                DL_FLAG(_, IsEpicDestinyFeat) \
                DL_FLAG(_, IsEpicFeat) \
                DL_FLAG(_, IsFavoredEnemy) \
                DL_FLAG(_, IsFighterBonus) \
                DL_FLAG(_, IsFollowerOf) \
                DL_FLAG(_, IsHeart) \
                DL_FLAG(_, IsRogueSpecialAbility) \
                DL_FLAG(_, IsStandardFeat) \
                DL_FLAG(_, IsLegendaryFeat) \
                DL_FLAG(_, IsMonkBonus) \
                DL_FLAG(_, IsMonkBonus6) \
                DL_FLAG(_, IsMonkPhilosphy) \
                DL_FLAG(_, IsMetamagic) \
                DL_FLAG(_, IsWarlockPact) \
                DL_FLAG(_, IsWarlockPactAbility) \
                DL_FLAG(_, IsWarlockResistance) \
                DL_FLAG(_, IsWarlockSaveBonus) \
                DL_FLAG(_, IsWarlockPactSpell)

        DL_DECLARE_ACCESS(FeatGroup_PROPERTIES)
        DL_DECLARE_VARIABLES(FeatGroup_PROPERTIES)
};
