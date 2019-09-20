// Spell.cpp
//
#include "StdAfx.h"
#include "Spell.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "BreakdownItemAbility.h"
#include "BreakdownItemSpellSchool.h"
#include "BreakdownItemEnergyCasterLevel.h"
#include "BreakdownItemSchoolCasterLevel.h"

#define DL_ELEMENT Spell

namespace
{
    const wchar_t f_saxElementName[] = L"Spell";
    DL_DEFINE_NAMES(Spell_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Spell::Spell() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Spell_PROPERTIES)
}

DL_DEFINE_ACCESS(Spell_PROPERTIES)

XmlLib::SaxContentElementInterface * Spell::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Spell_PROPERTIES)

    return subHandler;
}

void Spell::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Spell_PROPERTIES)
}

void Spell::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Spell_PROPERTIES)
    writer->EndElement();
}

void Spell::AddImage(CImageList * pIL) const
{
    CImage image;
    HRESULT result = LoadImageFile(
            IT_spell,
            m_Icon,
            &image);
    if (result == S_OK)
    {
        CBitmap bitmap;
        bitmap.Attach(image.Detach());
        pIL->Add(&bitmap, c_transparentColour);  // standard mask color (purple)
    }
}

bool Spell::operator<(const Spell & other) const
{
    // (assumes all spell names are unique)
    // sort by name
    return (Name() < other.Name());
}

size_t Spell::SpellLevel(ClassType ct) const
{
    size_t level = 0;
    switch (ct)
    {
    case Class_Artificer:
        level = Artificer();
        break;
    case Class_Bard:
        level = Bard();
        break;
    case Class_Cleric:
        level = Cleric();
        break;
    case Class_Druid:
        level = Druid();
        break;
    case Class_FavoredSoul:
        level = FavoredSoul();
        break;
    case Class_Paladin:
        level = Paladin();
        break;
    case Class_Ranger:
        level = Ranger();
        break;
    case Class_Sorcerer:
        level = Sorcerer();
        break;
    case Class_Warlock:
        level = Warlock();
        break;
    case Class_Wizard:
        level = Wizard();
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    return level;
}

std::vector<std::string> Spell::Metamagics() const
{
    std::vector<std::string> metas;
    if (HasEmbolden())
    {
        metas.push_back("Embolden");
    }
    if (HasEmpower())
    {
        metas.push_back("Empower");
    }
    if (HasEmpowerHealing())
    {
        metas.push_back("Empower Healing");
    }
    if (HasEnlarge())
    {
        metas.push_back("Enlarge");
    }
    if (HasExtend())
    {
        metas.push_back("Extend");
    }
    if (HasHeighten())
    {
        metas.push_back("Heighten");
    }
    if (HasIntensify())
    {
        metas.push_back("Intensify");
    }
    if (HasMaximize())
    {
        metas.push_back("Maximize");
    }
    if (HasQuicken())
    {
        metas.push_back("Quicken");
    }
    return metas;
}

void Spell::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (!ImageFileExists(IT_spell, Icon()))
    {
        ss << "Spell is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    // check the spell effects also
    std::vector<Effect>::const_iterator eit = m_Effects.begin();
    while (eit != m_Effects.end())
    {
        ok &= (*eit).VerifyObject(&ss);
        ++eit;
    }
    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}

size_t Spell::SpellDC(
        const Character & charData,
        ClassType ct,
        size_t spellLevel,
        size_t maxSpellLevel) const
{
    size_t dc = 0;
    BreakdownType bt = Breakdown_Unknown;
    switch (m_School)
    {
    case SpellSchool_Abjuration:
        bt = Breakdown_SpellSchoolAbjuration;
        break;
    case SpellSchool_Conjuration:
        bt = Breakdown_SpellSchoolConjuration;
        break;
    case SpellSchool_Divination:
        bt = Breakdown_SpellSchoolDivination;
        break;
    case SpellSchool_Enchantment:
        bt = Breakdown_SpellSchoolEnchantment;
        break;
    case SpellSchool_Evocation:
        bt = Breakdown_SpellSchoolEvocation;
        break;
    case SpellSchool_Illusion:
        bt = Breakdown_SpellSchoolIllusion;
        break;
    case SpellSchool_Necromancy:
        bt = Breakdown_SpellSchoolNecromancy;
        break;
    case SpellSchool_Transmutation:
        bt = Breakdown_SpellSchoolTransmutation;
        break;
    }
    // DC = breakdown amount + class ability amount + spell level
    // + maxSpellLevel - spellLevel) if heightened
    BreakdownItem * pBI = FindBreakdown(bt);
    BreakdownItemSpellSchool * pBISpellSchool = dynamic_cast<BreakdownItemSpellSchool *>(pBI);
    if (pBI != NULL)
    {
        AbilityType at = ClassCastingStat(ct);
        BreakdownItem * pCS = FindBreakdown(StatToBreakdown(at));
        BreakdownItemAbility * pBIAbility = dynamic_cast<BreakdownItemAbility *>(pCS);
        if (pBIAbility != NULL)
        {
            // we have all the information we need now
            dc = (size_t)pBISpellSchool->Total()        // lose fractions
                    + BaseStatToBonus(pBIAbility->Total())
                    + spellLevel;
            if (charData.IsStanceActive("Heighten"))
            {
                dc += (maxSpellLevel - spellLevel);
            }
        }
    }
    return dc;
}

std::vector<Effect> Spell::UpdatedEffects(size_t castingLevel) const
{
    // create a copy of the spell effects with the amount field dependent on the
    // the caster level in use.
    std::vector<Effect> effects = m_Effects;
    // look at each effect and update if required
    for (size_t i = 0; i < effects.size(); ++i)
    {
        if (effects[i].HasAmountVector())
        {
            // look up the amount to use for this casting level
            // note that its ok for a caster level to be larger than the
            // vector supplied, in such cases we just use the value from the last element
            std::vector<double> amounts = effects[i].AmountVector();
            castingLevel = min(castingLevel, amounts.size() - 1);   // limit range
            effects[i].Set_Amount(amounts[castingLevel]);
            effects[i].Clear_AmountVector();    // no longer has amount vector
        }
    }
    return effects;
}

int Spell::DynamicCasterLevel(const Character & charData, ClassType ct) const
{
    // caster level is:
    //  actual class level
    //  + any bonus class caster levels
    //  + any energy spell type caster levels
    //  + any school type caster levels
    int casterLevel = 0;
    BreakdownItem * pCLB = FindBreakdown(ClassToCasterLevelsBreakdown(ct)); // includes class levels
    if (pCLB != NULL)
    {
        casterLevel += (int)pCLB->Total();
    }
    if (HasEnergy())
    {
        BreakdownItem * pCLE = FindBreakdown(EnergyToCasterLevelsBreakdown(Energy()));
        if (pCLE != NULL)
        {
            casterLevel += (int)pCLE->Total();
        }
    }
    if (HasSchool())
    {
        BreakdownItem * pCLS = FindBreakdown(SchoolToCasterLevelsBreakdown(School()));
        if (pCLS != NULL)
        {
            casterLevel += (int)pCLS->Total();
        }
    }
    return casterLevel;
}

int Spell::DynamicMaxCasterLevel(const Character & charData, ClassType ct) const
{
    // Max caster level is defined by the spell plus:
    //   + any energy spell max caster types
    //   + any school type max caster levels
    int maxCasterLevel = 0;
    if (HasMaxCasterLevel())
    {
        maxCasterLevel = MaxCasterLevel();
    }
    if (HasEnergy())
    {
        BreakdownItem * pCLE = FindBreakdown(EnergyToCasterLevelsBreakdown(Energy()));
        BreakdownItemEnergyCasterLevel * pCLEB = dynamic_cast<BreakdownItemEnergyCasterLevel*>(pCLE);
        if (pCLEB != NULL)
        {
            maxCasterLevel += (int)pCLEB->MaxTotal();
        }
    }
    if (HasSchool())
    {
        BreakdownItem * pCLS = FindBreakdown(SchoolToCasterLevelsBreakdown(School()));
        BreakdownItemSchoolCasterLevel * pCLSB = dynamic_cast<BreakdownItemSchoolCasterLevel*>(pCLS);
        if (pCLSB != NULL)
        {
            maxCasterLevel += (int)pCLSB->MaxTotal();
        }
    }
    return maxCasterLevel;
}

