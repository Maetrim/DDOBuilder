// Character.cpp
//
#include "StdAfx.h"
#include "Character.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "DDOCPDoc.h"
#include <algorithm>
#include "BreakdownItemWeaponEffects.h"
#include "MainFrm.h"
#include "StancesView.h"
#include "SpellsControl.h"

#define DL_ELEMENT Character

namespace
{
    const wchar_t f_saxElementName[] = L"Character";
    DL_DEFINE_NAMES(Character_PROPERTIES)

    const unsigned f_verCurrent = 1;


    struct ClassEntry
    {
        ClassType type;
        size_t levels;
    };
    bool SortClassEntry(const ClassEntry & a, const ClassEntry & b)
    {
        if (a.levels == b.levels)
        {
            // same level, sort on class name
            return (a.type < b.type);
        }
        else
        {
            // sort on number of class levels
            return (a.levels > b.levels);
        }
    }
    std::string ReplaceAll(
            std::string str,
            const std::string & from,
            const std::string & to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }
}

Character::Character() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pDocument(NULL),
    m_SpecialFeats(L"SpecialFeats"),
    m_bonusRacialActionPoints(0),
    m_bonusUniversalActionPoints(0),
    m_racialTreeSpend(0),
    m_universalTreeSpend(0),
    m_destinyTreeSpend(0),
    m_classTreeSpend(0),
    m_previousGuildLevel(0)
{
    DL_INIT(Character_PROPERTIES)
    // make sure we have MaxLevel() default LevelTraining objects in the list
    size_t count = m_Levels.size();
    for (size_t index = count; index < MaxLevel(); ++index)
    {
        LevelTraining lt;
        if (index >= MAX_CLASS_LEVELS && index < MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
        {
            // all levels above level MAX_CLASS_LEVELS are epic levels by default
            lt.Set_Class(Class_Epic);
        }
        else if (index >= MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
        {
            // all levels above level MAX_CLASS_LEVELS + MAX_EPIC_LEVELS are legendary
            // levels by default
            lt.Set_Class(Class_Legendary);
        }
        m_Levels.push_back(lt);
    }
    UpdateSkillPoints();
}

Character::Character(CDDOCPDoc * pDoc) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pDocument(pDoc),
    m_SpecialFeats(L"SpecialFeats"),
    m_bonusRacialActionPoints(0),
    m_bonusUniversalActionPoints(0),
    m_bonusDestinyActionPoints(0),
    m_destinyTreeSpend(0),
    m_racialTreeSpend(0),
    m_universalTreeSpend(0),
    m_classTreeSpend(0),
    m_previousGuildLevel(0),
    m_bShowEpicOnly(false),
    m_bShowUnavailableFeats(false),
    m_bShowIgnoredItems(false)
{
    DL_INIT(Character_PROPERTIES)
    // make sure we have MAX_LEVEL default LevelTraining objects in the list
    size_t count = m_Levels.size();
    for (size_t index = count; index < MaxLevel(); ++index)
    {
        LevelTraining lt;
        if (index >= MAX_CLASS_LEVELS && index < MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
        {
            // all levels above level MAX_CLASS_LEVELS are epic levels by default
            lt.Set_Class(Class_Epic);
        }
        else if (index >= MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
        {
            // all levels above level MAX_CLASS_LEVELS + MAX_EPIC_LEVELS are legendary
            // levels by default
            lt.Set_Class(Class_Legendary);
        }
        m_Levels.push_back(lt);
    }
    UpdateFeats();
    // by default they start with 1 gear layout called "Standard"
    EquippedGear gear("Standard");  // with no items yet selected
    m_GearSetups.push_back(gear);
    m_ActiveGear = "Standard";

    AttachObserver(this);       // we observe ourselves
    UpdateSkillPoints();
}

DL_DEFINE_ACCESS(Character_PROPERTIES)

XmlLib::SaxContentElementInterface * Character::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Character_PROPERTIES)

    return subHandler;
}

void Character::EndElement()
{
    // when we load in we end up with MAX_LEVEL blank levels followed by what was loaded
    // clear the first MAX_CLASS_LEVELS
    for (size_t i = 0 ; i < MAX_CLASS_LEVELS; ++i)
    {
        m_Levels.erase(m_Levels.begin());
    }
    // also clear any epic/legendary levels if the number of total levels has been increased
    // these epic levels will be at the start of m_Levels
    while (m_Levels.size() > 0
            && m_Levels.begin()->HasClass()
            && (m_Levels.begin()->Class() == Class_Epic
                || m_Levels.begin()->Class() == Class_Legendary))
    {
        m_Levels.erase(m_Levels.begin());
    }
    // old files may need to have levels padded by adding Epic/Legendary class levels
    while (m_Levels.size() < MaxLevel())
    {
        LevelTraining lt;
        if (m_Levels.size() < MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
        {
            // need to add epic
            lt.Set_Class(Class_Epic);
        }
        else
        {
            // need to add legendary
            lt.Set_Class(Class_Legendary);
        }
        m_Levels.push_back(lt);
    }

    // keep older files working
    m_hasGuildLevel = true; // default guild level is 0
    m_hasNotes = true;
    m_hasDestinyTrees = true;

    // if there has been a level cap increase older files may not hav ethe Level32(), Level36() and Level40()
    // fields. Add these automatically to the same value as that for previous
    // level up Level28()for 32, 32 for 36 and 36 for 40.
    if (!m_hasLevel32)
    {
        m_hasLevel32 = true;
        Set_Level32(Level28());
    }
    if (!m_hasLevel36)
    {
        m_hasLevel36 = true;
        Set_Level36(Level32());
    }
    if (!m_hasLevel40)
    {
        m_hasLevel40 = true;
        Set_Level40(Level36());
    }

    // notes text is not saved as \r\n's replace all text
    m_Notes = ReplaceAll(m_Notes, "\n", "\r\n");

    SaxContentElement::EndElement();
    DL_END(Character_PROPERTIES)
    ASSERT(m_Levels.size() == MaxLevel());

    // also remove the default "Standard" gear layout which we get by default
    m_GearSetups.pop_front();
    m_hasActiveGear = true;

    // backwards compatibility for old files without special feats for enhancement
    // trees trained.
    if (Enhancement_IsTreeTrained("Falconry")
            && !IsFeatTrained("Falconry Tree"))
    {
        TrainSpecialFeat("Falconry Tree", TFT_SpecialFeat);
    }
    if (Enhancement_IsTreeTrained("Harper Agent")
            && !IsFeatTrained("Harper Agent Tree"))
    {
        TrainSpecialFeat("Harper Agent Tree", TFT_SpecialFeat);
    }
    if (Enhancement_IsTreeTrained("Vistani Knife Fighter")
            && !IsFeatTrained("Vistani Knife Fighter Tree"))
    {
        TrainSpecialFeat("Vistani Knife Fighter Tree", TFT_SpecialFeat);
    }
    if (HasTier5Tree())
    {
        // check that this tree still exists, if not, clear it
        const EnhancementTree& t = FindTree(Tier5Tree());
        const SpendInTree* pSpendInTree = FindSpendInTree(Tier5Tree());
        if (t.Name() != Tier5Tree()
                || pSpendInTree == NULL)
        {
            // tree no longer exists or no points spent in it, just clear it so a new tier 5 can be selected
            Clear_Tier5Tree();
        }
    }
    if (HasU51Destiny_Tier5Tree())
    {
        // check that this tree still exists, if not, clear it
        const EnhancementTree& t = FindTree(U51Destiny_Tier5Tree());
        const SpendInTree* pSpendInTree = FindSpendInTree(U51Destiny_Tier5Tree());
        if (t.Name() != U51Destiny_Tier5Tree()
                || pSpendInTree == NULL)
        {
            // tree no longer exists or no points spent in it, just clear it so a new tier 5 can be selected
            Clear_U51Destiny_Tier5Tree();
        }
    }
}

void Character::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Character_PROPERTIES)
    writer->EndElement();
}

size_t Character::GetSpecialFeatTrainedCount(
        const std::string & featName) const
{
    // return the count of how many times this particular feat has
    // been trained.
    size_t count = 0;
    const std::list<TrainedFeat> & feats = SpecialFeats().Feats();
    std::list<TrainedFeat>::const_iterator it = feats.begin();
    while (it != feats.end())
    {
        if ((*it).FeatName() == featName)
        {
            ++count;    // it is present, count it
        }
        ++it;
    }
    return count;
}

size_t Character::AbilityTomeValue(AbilityType ability) const
{
    size_t value = 0;
    switch (ability)
    {
    case Ability_Strength:
        value = StrTome();
        break;
    case Ability_Dexterity:
        value = DexTome();
        break;
    case Ability_Constitution:
        value = ConTome();
        break;
    case Ability_Intelligence:
        value = IntTome();
        break;
    case Ability_Wisdom:
        value = WisTome();
        break;
    case Ability_Charisma:
        value = ChaTome();
        break;
    }
    return value;
}

size_t Character::SkillTomeValue(SkillType skill) const
{
    size_t value = 0;
    switch (skill)
    {
    case Skill_Balance:
        value = m_Tomes.Balance();
        break;
    case Skill_Bluff:
        value = m_Tomes.Bluff();
        break;
    case Skill_Concentration:
        value = m_Tomes.Concentration();
        break;
    case Skill_Diplomacy:
        value = m_Tomes.Diplomacy();
        break;
    case Skill_DisableDevice:
        value = m_Tomes.DisableDevice();
        break;
    case Skill_Haggle:
        value = m_Tomes.Haggle();
        break;
    case Skill_Heal:
        value = m_Tomes.Heal();
        break;
    case Skill_Hide:
        value = m_Tomes.Hide();
        break;
    case Skill_Intimidate:
        value = m_Tomes.Intimidate();
        break;
    case Skill_Jump:
        value = m_Tomes.Jump();
        break;
    case Skill_Listen:
        value = m_Tomes.Listen();
        break;
    case Skill_MoveSilently:
        value = m_Tomes.MoveSilently();
        break;
    case Skill_OpenLock:
        value = m_Tomes.OpenLock();
        break;
    case Skill_Perform:
        value = m_Tomes.Perform();
        break;
    case Skill_Repair:
        value = m_Tomes.Repair();
        break;
    case Skill_Search:
        value = m_Tomes.Search();
        break;
    case Skill_SpellCraft:
        value = m_Tomes.SpellCraft();
        break;
    case Skill_Spot:
        value = m_Tomes.Spot();
        break;
    case Skill_Swim:
        value = m_Tomes.Swim();
        break;
    case Skill_Tumble:
        value = m_Tomes.Tumble();
        break;
    case Skill_UMD:
        value = m_Tomes.UMD();
        break;
    }
    return value;
}

bool Character::IsFeatTrained(const std::string & featName, bool includeGrantedFeats) const
{
    // return true if the given feat is trained
    bool bTrained = false;
    std::list<TrainedFeat> currentFeats = CurrentFeats(MaxLevel());

    std::list<TrainedFeat>::const_iterator it = currentFeats.begin();
    while (it != currentFeats.end())
    {
        if ((*it).FeatName() == featName)
        {
            bTrained = true;
            break;
        }
        ++it;
    }
    if (includeGrantedFeats)
    {
        std::list<TrainedFeat>::const_iterator it = m_grantedFeats.begin();
        while (it != m_grantedFeats.end())
        {
            if ((*it).FeatName() == featName)
            {
                bTrained = true;
                break;
            }
            ++it;
        }
    }
    return bTrained;
}

TrainedFeat Character::GetTrainedFeat(
        size_t level,
        TrainableFeatTypes type) const
{
    // find the specific trained feat at level by type
    TrainedFeat feat;       // blank if no feat found

    const LevelTraining & lt = LevelData(level);
    const std::list<TrainedFeat> & tfs = lt.TrainedFeats().Feats();
    std::list<TrainedFeat>::const_iterator it = tfs.begin();
    bool found = false;
    while (!found && it != tfs.end())
    {
        if ((*it).Type() == type)
        {
            feat = (*it);
            found = true;
        }
        ++it;
    }
    return feat;
}

const LevelTraining & Character::LevelData(size_t level) const
{
    ASSERT(m_Levels.size() == MaxLevel());
    ASSERT(level < MaxLevel());
    std::list<LevelTraining>::const_iterator it = m_Levels.begin();
    std::advance(it, level);  // point to level of interest
    const LevelTraining & data = (*it);
    return data;
}

bool Character::RevokeClass(ClassType type)
{
    bool hadRevoke = false;
    size_t level = 0;
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    while (it != m_Levels.end())
    {
        if ((*it).HasClass()
                && (*it).Class() == type)
        {
            // class selection no longer available
            (*it).Set_Class(Class_Unknown);
            UpdateSkillPoints(level);
            hadRevoke = true;
        }
        ++it;
        ++level;
    }
    if (hadRevoke)
    {
        UpdateFeats();
        VerifyTrainedFeats();
        VerifyGear();
    }
    return hadRevoke;
}

void Character::UpdateSpells()
{
    bool showMsg = false;
    std::stringstream ss;
    ss << "The following spells were revoked due to class level or enhancement changes:\r\n"
        "\r\n";
    // check that we do not have more spells trained at a given spell
    // level than the current class assignments allow. If we do,
    // revoke the spells over the maximum size
    std::vector<size_t> classLevels = ClassLevels(MaxLevel());
    for (size_t ci = Class_Unknown + 1; ci < Class_Count; ++ci)
    {
        // get the number of spells available for this class at this level
        std::vector<size_t> spellSlots = SpellSlotsForClass((ClassType)ci, classLevels[ci]);
        for (int spellLevel = 0; spellLevel < (int)spellSlots.size(); ++spellLevel)
        {
            // get the current trained spells at this level
            std::list<TrainedSpell> trainedSpells = TrainedSpells((ClassType)ci, spellLevel + 1); // 1 based
            while (trainedSpells.size() > spellSlots[spellLevel])
            {
                TrainedSpell ts = trainedSpells.back();
                trainedSpells.pop_back(); // remove oldest
                // need to revoke this spell
                ss << EnumEntryText((ClassType)ci, classTypeMap) << ": ";
                ss << ts.SpellName() << " Spell Level " << ts.Level();
                ss << "\r\n";
                showMsg = true;
                RevokeSpell((ClassType)ci, ts.Level(), ts.SpellName());
            }
            // the spell must also still be in the available list for this class and level
            std::vector<Spell> availableSpells = FilterSpells(this, (ClassType)ci, spellLevel+1);
            std::list<TrainedSpell>::iterator tsit = trainedSpells.begin();
            while (tsit != trainedSpells.end())
            {
                std::string spellName = (*tsit).SpellName();
                bool bFound = false;
                for (size_t asi = 0; asi < availableSpells.size(); ++asi)
                {
                    if (availableSpells[asi].Name() == spellName)
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    // need to revoke this spell
                    ss << EnumEntryText((ClassType)ci, classTypeMap) << ": ";
                    ss << spellName << " Spell Level " << (*tsit).Level();
                    ss << "\r\n";
                    showMsg = true;
                    RevokeSpell((ClassType)ci, (*tsit).Level(), spellName);
                    tsit = trainedSpells.erase(tsit);
                }
                else
                {
                    ++tsit;
                }
            }
        }
    }
    if (showMsg)
    {
        AfxMessageBox(ss.str().c_str(), MB_ICONWARNING);
    }
}

void Character::UpdateFeats()
{
    // we start with any special feats (past lives etc)
    std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
    // for each level, determine what automatic feats are trained
    // also, update each level with the trainable feat types
    for (size_t level = 0; level < MaxLevel(); ++level)
    {
        UpdateFeats(level, &allFeats);
    }
    KeepGrantedFeatsUpToDate();
}

void Character::UpdateFeats(size_t level, std::list<TrainedFeat> * allFeats)
{
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    std::list<TrainedFeat> oldFeats = (*it).AutomaticFeats().Feats();

    // add the trained feats for this level as they can affect the available automatic feats
    const std::list<TrainedFeat> & trainedFeats = (*it).TrainedFeats().Feats();
    allFeats->insert(allFeats->end(), trainedFeats.begin(), trainedFeats.end());

    FeatsListObject flo(L"AutomaticFeats");
    std::list<TrainedFeat> automaticFeats = AutomaticFeats(level, *allFeats);
    // have the automatic feats at this level changed?
    if (automaticFeats != oldFeats)
    {
        // first revoke the feats at this level then apply the new ones
        std::list<TrainedFeat>::iterator ofit = oldFeats.begin();
        while (ofit != oldFeats.end())
        {
            const Feat & feat = FindFeat((*ofit).FeatName());
            RevokeFeatEffects(feat);
            ++ofit;
        }

        flo.Set_Feats(automaticFeats);
        allFeats->insert(allFeats->end(), automaticFeats.begin(), automaticFeats.end());
        // now apply the new automatic feats
        std::list<TrainedFeat>::iterator afit = automaticFeats.begin();
        while (afit != automaticFeats.end())
        {
            const Feat & feat = FindFeat((*afit).FeatName());
            ApplyFeatEffects(feat);
            ++afit;
        }
        (*it).Set_AutomaticFeats(flo);
    }
    else
    {
        // still need them in the list even if we do not revoke/train
        allFeats->insert(allFeats->end(), automaticFeats.begin(), automaticFeats.end());
    }
}

void Character::NotifyAvailableBuildPointsChanged()
{
    NotifyAll(&CharacterObserver::UpdateAvailableBuildPointsChanged, this);
}

void Character::NotifyAlignmentChanged(AlignmentType alignment)
{
    NotifyAll(&CharacterObserver::UpdateAlignmentChanged, this, alignment);
}

void Character::NotifySkillSpendChanged(size_t level, SkillType skill)
{
    NotifyAll(&CharacterObserver::UpdateSkillSpendChanged, this, level, skill);
}

void Character::NotifySkillTomeChanged(SkillType skill)
{
    NotifyAll(&CharacterObserver::UpdateSkillTomeChanged, this, skill);
}

void Character::NotifyClassChoiceChanged()
{
    NotifyAll(&CharacterObserver::UpdateClassChoiceChanged, this);
}

void Character::NotifyClassChanged(ClassType classFrom, ClassType classTo, size_t level)
{
    NotifyAll(&CharacterObserver::UpdateClassChanged, this, classFrom, classTo, level);
}

void Character::NotifyAbilityValueChanged(AbilityType ability)
{
    NotifyAll(&CharacterObserver::UpdateAbilityValueChanged, this, ability);
}

void Character::NotifyAbilityTomeChanged(AbilityType ability)
{
    NotifyAll(&CharacterObserver::UpdateAbilityTomeChanged, this, ability);
}

void Character::NotifyRaceChanged(RaceType race)
{
    NotifyAll(&CharacterObserver::UpdateRaceChanged, this, race);
}

void Character::NotifyFeatTrained(const std::string & featName)
{
    NotifyAll(&CharacterObserver::UpdateFeatTrained, this, featName);
}

void Character::NotifyFeatRevoked(const std::string & featName)
{
    NotifyAll(&CharacterObserver::UpdateFeatRevoked, this, featName);
}

void Character::NotifyFeatEffect(const std::string & featName, const Effect & effect)
{
    NotifyAll(&CharacterObserver::UpdateFeatEffect, this, featName, effect);
}

void Character::NotifyFeatEffectRevoked(const std::string & featName, const Effect & effect)
{
    NotifyAll(&CharacterObserver::UpdateFeatEffectRevoked, this, featName, effect);
}

void Character::NotifyItemEffect(const std::string & itemName, const Effect & effect)
{
    NotifyAll(&CharacterObserver::UpdateItemEffect, this, itemName, effect);
}

void Character::NotifyItemEffectRevoked(const std::string & itemName, const Effect & effect)
{
    NotifyAll(&CharacterObserver::UpdateItemEffectRevoked, this, itemName, effect);
}

void Character::NotifyGearChanged(InventorySlotType slot)
{
    NotifyAll(&CharacterObserver::UpdateGearChanged, this, slot);
}

void Character::NotifyNewStance(const Stance & stance)
{
    NotifyAll(&CharacterObserver::UpdateNewStance, this, stance);
}

void Character::NotifyRevokeStance(const Stance & stance)
{
    NotifyAll(&CharacterObserver::UpdateRevokeStance, this, stance);
}

void Character::NotifyStanceActivated(const std::string & name)
{
    NotifyAll(&CharacterObserver::UpdateStanceActivated, this, name);
}

void Character::NotifyStanceDeactivated(const std::string & name)
{
    NotifyAll(&CharacterObserver::UpdateStanceDeactivated, this, name);
}

void Character::NotifyFatePointsChanged()
{
    NotifyAll(&CharacterObserver::UpdateFatePointsChanged, this);
}

void Character::NotifyEnhancementEffect(
        const std::string & enhancementName,
        const Effect & effect,
        size_t ranks)
{
    EffectTier et(effect, ranks);
    NotifyAll(&CharacterObserver::UpdateEnhancementEffect, this, enhancementName, et);
    if (effect.Type() == Effect_Unique)
    {
        std::pair<std::string, std::string> item;
        item.first = effect.Unique();
        item.second = effect.Selection();
        m_uniqueSelections.push_back(item);
    }
}

void Character::NotifyEnhancementEffectRevoked(
        const std::string & name,
        const Effect & effect,
        size_t tier)
{
    EffectTier et(effect, tier);
    NotifyAll(&CharacterObserver::UpdateEnhancementEffectRevoked, this, name, et);
    if (effect.Type() == Effect_Unique)
    {
        auto it = m_uniqueSelections.begin();
        while (it != m_uniqueSelections.end())
        {
            if ((*it).first == effect.Unique())
            {
                m_uniqueSelections.erase(it);
                break;
            }
            ++it;
        }
    }
}

void Character::NotifyEnhancementTrained(
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5,
        bool bActiveTree)
{
    NotifyAll(
            &CharacterObserver::UpdateEnhancementTrained,
            this,
            enhancementName,
            selection,
            isTier5);
    if (bActiveTree)
    {
        const EnhancementTreeItem * pItem = FindEnhancement(enhancementName);
        if (pItem != NULL)
        {
            std::list<Stance> stances = pItem->Stances(selection);
            // enhancements may give multiple stances
            std::list<Stance>::const_iterator sit = stances.begin();
            while (sit != stances.end())
            {
                NotifyNewStance((*sit));
                ++sit;
            }
        }
    }
}

void Character::NotifyEnhancementRevoked(
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5,
        bool bActiveTree)
{
    NotifyAll(
            &CharacterObserver::UpdateEnhancementRevoked,
            this,
            enhancementName,
            selection,
            isTier5);
    if (bActiveTree)
    {
        const EnhancementTreeItem * pItem = FindEnhancement(enhancementName);
        if (pItem != NULL)
        {
            std::list<Stance> stances = pItem->Stances(selection);
            // enhancements may give multiple stances
            std::list<Stance>::const_iterator sit = stances.begin();
            while (sit != stances.end())
            {
                NotifyRevokeStance((*sit));
                ++sit;
            }
        }
    }
}

void Character::NotifyEnhancementTreeReset()
{
    NotifyAll(&CharacterObserver::UpdateEnhancementTreeReset, this);
}

void Character::NotifyEnhancementTreeOrderChanged()
{
    NotifyAll(&CharacterObserver::UpdateEnhancementTreeOrderChanged, this);
}


void Character::NotifyActionPointsChanged()
{
    NotifyAll(&CharacterObserver::UpdateActionPointsChanged, this);
}

void Character::NotifyAPSpentInTreeChanged(const std::string & treeName)
{
    NotifyAll(&CharacterObserver::UpdateAPSpentInTreeChanged, this, treeName);
}

void Character::NotifySpellTrained(const TrainedSpell & spell)
{
    NotifyAll(&CharacterObserver::UpdateSpellTrained, this, spell);
}

void Character::NotifySpellRevoked(const TrainedSpell & spell)
{
    NotifyAll(&CharacterObserver::UpdateSpellRevoked, this, spell);
}

void Character::NotifyGrantedFeatsChanged()
{
    NotifyAll(&CharacterObserver::UpdateGrantedFeatsChanged, this);
}

void Character::NotifyNewDC(const DC & dc)
{
    NotifyAll(&CharacterObserver::UpdateNewDC, this, dc);
}

void Character::NotifyRevokeDC(const DC & dc)
{
    NotifyAll(&CharacterObserver::UpdateRevokeDC, this, dc);
}

void Character::NotifyAllLoadedEffects()
{
    // get a list of all feats and notify for each feat effect
    std::list<TrainedFeat> feats = CurrentFeats(MaxLevel());
    std::list<TrainedFeat>::iterator it = feats.begin();
    while (it != feats.end())
    {
        // get the list of effects this feat has
        const Feat & feat = FindFeat((*it).FeatName());
        ApplyFeatEffects(feat);
        ++it;
    }
}

void Character::NotifyAllEnhancementEffects()
{
    // for each tree that has a trained item
    std::list<EnhancementSpendInTree>::const_iterator it = m_EnhancementTreeSpend.begin();
    while (it != m_EnhancementTreeSpend.end())
    {
        const EnhancementSpendInTree & tree = (*it);
        ApplyAllEffects(tree.TreeName(), tree.Enhancements());
        ++it;
    }
}

void Character::NotifyAllReaperEnhancementEffects()
{
    // for each tree that has a trained item
    std::list<ReaperSpendInTree>::const_iterator it = m_ReaperTreeSpend.begin();
    while (it != m_ReaperTreeSpend.end())
    {
        const ReaperSpendInTree & tree = (*it);
        ApplyAllEffects(tree.TreeName(), tree.Enhancements());
        ++it;
    }
}

void Character::NotifyAllU51DestinyEffects()
{
    std::list<DestinySpendInTree>::const_iterator it = m_DestinyTreeSpend.begin();
    while (it != m_DestinyTreeSpend.end())
    {
        const DestinySpendInTree & tree = (*it);
        ApplyAllEffects(tree.TreeName(), tree.Enhancements());
        ++it;
    }
}

void Character::SetCharacterName(const CString & name)
{
    Set_Name((LPCTSTR)name);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::SetRace(RaceType race)
{
    // revoke any race feats trained here also
    std::vector<TrainableFeatTypes> typesToRevoke = RaceSpecificFeatTypes(Race());
    std::vector<TrainableFeatTypes>::const_iterator ttrIt = typesToRevoke.begin();
    while (ttrIt != typesToRevoke.end())
    {
        // special racial feats are only trained at level 1
        TrainedFeat tf = GetTrainedFeat(0, (*ttrIt));
        const Feat & feat = FindFeat(tf.FeatName());
        RevokeFeatEffects(feat);
        ++ttrIt;
    }
    Set_Race(race);
    UpdateFeats();
    NotifyRaceChanged(race);
    DetermineBuildPoints();     // switching too/from Drow can affect the number of build points
    UpdateSkillPoints();        // race can effect number of skill points (int + human)
    m_pDocument->SetModifiedFlag(TRUE);
    // revoking a racial feat can invalidate a feat selection in other levels (e.g. loss of Dodge)
    VerifyTrainedFeats();
    VerifyGear();               // changing too/from forged/kobold can affect equipped gear
    UpdateWeaponStances();
}

void Character::SetAlignment(AlignmentType alignment)
{
    bool classChoiceChange = false;
    Set_Alignment(alignment);
    // an alignment change may invalidate current class selections
    while (!IsClassAvailable(Class1())
            && Class1() != Class_Unknown)
    {
        // class 1 is now no longer valid, bump it from all levels
        RevokeClass(Class1());
        Set_Class1(Class2());
        Set_Class2(Class3());
        Set_Class3(Class_Unknown);
        classChoiceChange = true;
    }
    // do the same for class 2 and class 3
    while (!IsClassAvailable(Class2())
            && Class2() != Class_Unknown)
    {
        // class 2 is now no longer valid, bump it from all levels
        RevokeClass(Class2());
        Set_Class2(Class3());
        Set_Class3(Class_Unknown);
        classChoiceChange = true;
    }
    // do the same for class 3
    while (!IsClassAvailable(Class3())
            && Class3() != Class_Unknown)
    {
        // class 3 is now no longer valid, bump it from all levels
        RevokeClass(Class3());
        Set_Class3(Class_Unknown);
        classChoiceChange = true;
    }
    if (classChoiceChange)
    {
        NotifyClassChoiceChanged();
    }
    NotifyAlignmentChanged(alignment);
    m_pDocument->SetModifiedFlag(TRUE);
    VerifyTrainedFeats();
    SetAlignmentStances();
    VerifyGear();
}

void Character::SetAlignmentStances()
{
    Stance lawful("Lawful", "Lawful", "You are Lawful");
    Stance chaotic("Chaotic", "Chaotic", "You are Chaotic");
    Stance good("Good", "Good", "You are Good");
    Stance trueNeutral("True Neutral", "TrueNeutral", "You are True Neutral");
    Stance evil("Evil", "Evil", "You are Evil");

    switch (OverrideAlignment())
    {
    case Alignment_LawfulGood:
        ActivateStance(lawful);
        ActivateStance(good);
        DeactivateStance(chaotic);
        DeactivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_LawfulNeutral:
        ActivateStance(lawful);
        DeactivateStance(good);
        DeactivateStance(chaotic);
        DeactivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_NeutralGood:
        DeactivateStance(lawful);
        ActivateStance(good);
        DeactivateStance(chaotic);
        DeactivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_TrueNeutral:
        DeactivateStance(lawful);
        DeactivateStance(good);
        DeactivateStance(chaotic);
        ActivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_ChaoticNeutral:
        DeactivateStance(lawful);
        DeactivateStance(good);
        ActivateStance(chaotic);
        DeactivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_ChaoticGood:
        DeactivateStance(lawful);
        ActivateStance(good);
        ActivateStance(chaotic);
        DeactivateStance(trueNeutral);
        DeactivateStance(evil);
        break;
    case Alignment_NeutralEvil:
        DeactivateStance(lawful);
        DeactivateStance(good);
        DeactivateStance(chaotic);
        DeactivateStance(trueNeutral);
        ActivateStance(evil);
        break;
    case Alignment_ChaoticEvil:
        DeactivateStance(lawful);
        DeactivateStance(good);
        ActivateStance(chaotic);
        DeactivateStance(trueNeutral);
        ActivateStance(evil);
        break;
    }
}

void Character::SetAbilityTome(AbilityType ability, size_t value)
{
    switch (ability)
    {
    case Ability_Strength:
        Set_StrTome(value);
        break;
    case Ability_Dexterity:
        Set_DexTome(value);
        break;
    case Ability_Constitution:
        Set_ConTome(value);
        break;
    case Ability_Intelligence:
        Set_IntTome(value);
        break;
    case Ability_Wisdom:
        Set_WisTome(value);
        break;
    case Ability_Charisma:
        Set_ChaTome(value);
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    // if Intelligence has changed, update the available skill points
    if (ability == Ability_Intelligence)
    {
        UpdateSkillPoints();
    }
    NotifyAbilityTomeChanged(ability);
    m_pDocument->SetModifiedFlag(TRUE);
    // changing an inherent tome value can invalidate a feat selection (e.g Dodge)
    VerifyTrainedFeats();
}

void Character::SetSkillTome(SkillType skill, size_t value)
{
    switch (skill)
    {
    case Skill_Balance:
        m_Tomes.Set_Balance(value);
        break;
    case Skill_Bluff:
        m_Tomes.Set_Bluff(value);
        break;
    case Skill_Concentration:
        m_Tomes.Set_Concentration(value);
        break;
    case Skill_Diplomacy:
        m_Tomes.Set_Diplomacy(value);
        break;
    case Skill_DisableDevice:
        m_Tomes.Set_DisableDevice(value);
        break;
    case Skill_Haggle:
        m_Tomes.Set_Haggle(value);
        break;
    case Skill_Heal:
        m_Tomes.Set_Heal(value);
        break;
    case Skill_Hide:
        m_Tomes.Set_Hide(value);
        break;
    case Skill_Intimidate:
        m_Tomes.Set_Intimidate(value);
        break;
    case Skill_Jump:
        m_Tomes.Set_Jump(value);
        break;
    case Skill_Listen:
        m_Tomes.Set_Listen(value);
        break;
    case Skill_MoveSilently:
        m_Tomes.Set_MoveSilently(value);
        break;
    case Skill_OpenLock:
        m_Tomes.Set_OpenLock(value);
        break;
    case Skill_Perform:
        m_Tomes.Set_Perform(value);
        break;
    case Skill_Repair:
        m_Tomes.Set_Repair(value);
        break;
    case Skill_Search:
        m_Tomes.Set_Search(value);
        break;
    case Skill_SpellCraft:
        m_Tomes.Set_SpellCraft(value);
        break;
    case Skill_Spot:
        m_Tomes.Set_Spot(value);
        break;
    case Skill_Swim:
        m_Tomes.Set_Swim(value);
        break;
    case Skill_Tumble:
        m_Tomes.Set_Tumble(value);
        break;
    case Skill_UMD:
        m_Tomes.Set_UMD(value);
        break;
    }
    NotifySkillTomeChanged(skill);
    m_pDocument->SetModifiedFlag(TRUE);
    // changing an inherent tome value can invalidate a feat selection
    // (e.g Single Weapon Fighting)
    VerifyTrainedFeats();
}

void Character::SetClass1(ClassType type)
{
    ClassType classFrom = Class1();
    if (Class1() != Class_Unknown)
    {
        if (RevokeClass(Class1()))
        {
            // if a class got revoked keep the class list up to date
            NotifyClassChanged(classFrom, type, -1);
        }
    }
    Set_Class1(type);
    if (type == Class_Unknown)
    {
        // may need to bump classes down one if set to Unknown
        Set_Class1(Class2());
        Set_Class2(Class3());
        Set_Class3(Class_Unknown);
    }
    // now set all levels that were classFrom to current Class1
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        if (!LevelData(level).HasClass()
                || LevelData(level).Class() == classFrom
                || LevelData(level).Class() == Class_Unknown)
        {
            std::list<LevelTraining>::iterator it = m_Levels.begin();
            std::advance(it, level);
            (*it).Set_Class(Class1());
            UpdateSkillPoints(level);
            NotifyClassChanged(classFrom, Class1(), level);    // must be done before feat updates to keep spell lists kosher
        }
    }
    UpdateSpells();
    UpdateFeats();
    VerifyTrainedFeats();
    AutoTrainSingleSelectionFeats();
    VerifyGear();
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    NotifyStanceDeactivated("Force correct update of effects");
}

void Character::SetClass2(ClassType type)
{
    ClassType classFrom = Class2();
    if (Class2() != Class_Unknown)
    {
        ClassType classFrom = Class2();
        if (RevokeClass(Class2()))
        {
            // if a class got revoked keep the class list up to date
            NotifyClassChanged(classFrom, type, -1);
        }
    }
    Set_Class2(type);
    if (type == Class_Unknown)
    {
        // may need to bump classes down one if set to Unknown
        Set_Class2(Class3());
        Set_Class3(Class_Unknown);
    }
    // now set all levels that were classFrom to current Class1
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        if (!LevelData(level).HasClass()
                || LevelData(level).Class() == classFrom
                || LevelData(level).Class() == Class_Unknown)
        {
            std::list<LevelTraining>::iterator it = m_Levels.begin();
            std::advance(it, level);
            (*it).Set_Class(Class2());
            UpdateSkillPoints(level);
            NotifyClassChanged(classFrom, Class2(), level);    // must be done before feat updates to keep spell lists kosher
        }
    }
    UpdateSpells();
    UpdateFeats();
    VerifyTrainedFeats();
    AutoTrainSingleSelectionFeats();
    VerifyGear();
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    NotifyStanceDeactivated("Force correct update of effects");
}

void Character::SetClass3(ClassType type)
{
    ClassType classFrom = Class3();
    if (Class3() != Class_Unknown)
    {
        ClassType classFrom = Class3();
        if (RevokeClass(Class3()))
        {
            // if a class got revoked keep the class list up to date
            NotifyClassChanged(classFrom, type, -1);
        }
    }
    Set_Class3(type);
    // now set all levels that were classFrom to current Class1
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        if (!LevelData(level).HasClass()
                || LevelData(level).Class() == classFrom
                || LevelData(level).Class() == Class_Unknown)
        {
            std::list<LevelTraining>::iterator it = m_Levels.begin();
            std::advance(it, level);
            (*it).Set_Class(Class3());
            UpdateSkillPoints(level);
            NotifyClassChanged(classFrom, Class3(), level);    // must be done before feat updates to keep spell lists kosher
        }
    }
    UpdateSpells();
    UpdateFeats();
    VerifyTrainedFeats();
    AutoTrainSingleSelectionFeats();
    VerifyGear();
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    NotifyStanceDeactivated("Force correct update of effects");
}

void Character::SetClass(size_t level, ClassType type)
{
    ClassType classFrom = Class_Unknown;
    size_t oldCasterLevelTo = CasterLevel(this, type);
    size_t oldCasterLevelFrom = 0;
    if (level < MAX_CLASS_LEVELS)    // 0 based
    {
        ASSERT(m_Levels.size() == MaxLevel());
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        std::advance(it, level);
        classFrom = (*it).HasClass() ? (*it).Class() : Class_Unknown;
        oldCasterLevelFrom = CasterLevel(this, classFrom);
        (*it).Set_Class(type);
        UpdateSkillPoints(level);
        m_pDocument->SetModifiedFlag(TRUE);
    }
    NotifyClassChanged(classFrom, type, level);    // must be done before feat updates to keep spell lists kosher
    UpdateSpells();
    UpdateFeats();
    VerifyTrainedFeats();
    AutoTrainSingleSelectionFeats();
    VerifyGear();
    CSpellsControl * pSC = GetMainFrame()->GetSpellsControl(type);
    if (pSC != NULL)
    {
        pSC->UpdateSpells(oldCasterLevelTo);
    }
    pSC = GetMainFrame()->GetSpellsControl(classFrom);
    if (pSC != NULL)
    {
        pSC->UpdateSpells(oldCasterLevelFrom);
    }
    // it is possible to select multiple archetype classes before assigning
    // levels. If they do this we need to revoke the archetype/default class
    // selection at the point at which the first class level is set for the
    // archetype/default
    size_t c1Levels = ClassLevels(Class1());
    size_t c2Levels = ClassLevels(Class2());
    size_t c3Levels = ClassLevels(Class3());
    if ((c2Levels > 0 && SameArchetype(Class1(), Class2()))
            || (c3Levels > 0 && SameArchetype(Class1(), Class3())))
    {
        // need to revoke class 1 selection
        SetClass1(Class_Unknown);
    }
    if ((c1Levels > 0 && SameArchetype(Class1(), Class2()))
            || (c3Levels > 0 && SameArchetype(Class2(), Class3())))
    {
        // need to revoke class 2 selection
        SetClass2(Class_Unknown);
    }
    if ((c2Levels > 0 && SameArchetype(Class2(), Class3()))
            || (c1Levels > 0 && SameArchetype(Class1(), Class3())))
    {
        // need to revoke class 3 selection
        SetClass3(Class_Unknown);
    }
}

void Character::SwapClasses(size_t level1, size_t level2)
{
    ClassType c1 = LevelData(level1).HasClass()
            ? LevelData(level1).Class()
            : Class_Unknown;
    ClassType c2 = LevelData(level2).HasClass()
            ? LevelData(level2).Class()
            : Class_Unknown;
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level1);
    (*it).Set_Class(c2);
    UpdateSkillPoints(level1);

    it = m_Levels.begin();
    std::advance(it, level2);
    (*it).Set_Class(c1);
    UpdateSkillPoints(level2);

    m_pDocument->SetModifiedFlag(TRUE);
    UpdateSpells();
    UpdateFeats();
    VerifyTrainedFeats();
    AutoTrainSingleSelectionFeats();
    VerifyGear();
}

void Character::SpendSkillPoint(
        size_t level,
        SkillType skill,
        bool suppressUpdate)
{
    // update the skill point spend for the correct level data
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    (*it).TrainSkill(skill);
    if (!suppressUpdate)
    {
        NotifySkillSpendChanged(level, skill);
    }
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::RevokeSkillPoint(
        size_t level,
        SkillType skill,
        bool suppressUpdate)
{
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    (*it).RevokeSkill(skill);
    if (!suppressUpdate)
    {
        NotifySkillSpendChanged(level, skill);
        // revoking a skill point in theory can invalidate a feat selection
        VerifyTrainedFeats();
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

void Character::SkillsUpdated()
{
    // once for every skill at level 20
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        NotifySkillSpendChanged(MAX_CLASS_LEVELS, (SkillType)skill);
    }
    VerifyTrainedFeats();
    m_pDocument->SetModifiedFlag(TRUE);
}

// active point build affected
size_t Character::BaseAbilityValue(AbilityType ability) const
{
    const AbilitySpend & as = BuildPoints();
    size_t value = 8 + RacialModifier(Race(), ability) + as.GetAbilitySpend(ability);
    return value;
}

void Character::SpendOnAbility(AbilityType ability)
{
    m_BuildPoints.SpendOnAbility(ability);
    // if Intelligence has changed, update the available skill points
    if (ability == Ability_Intelligence)
    {
        UpdateSkillPoints();
    }
    NotifyAbilityValueChanged(ability);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::RevokeSpendOnAbility(AbilityType ability)
{
    m_BuildPoints.RevokeSpendOnAbility(ability);
    // if Intelligence has changed, update the available skill points
    if (ability == Ability_Intelligence)
    {
        UpdateSkillPoints();
    }
    NotifyAbilityValueChanged(ability);
    m_pDocument->SetModifiedFlag(TRUE);
    // revoking an ability point in theory can invalidate a feat selection
    VerifyTrainedFeats();
}

void Character::TrainSpecialFeat(
        const std::string & featName,
        TrainableFeatTypes type)
{
    // just add a copy of the feat name to the current list
    std::list<TrainedFeat> trainedFeats = SpecialFeats().Feats();
    TrainedFeat tf;
    tf.Set_FeatName(featName);
    tf.Set_Type(type);
    tf.Set_LevelTrainedAt(0);
    trainedFeats.push_back(tf);

    FeatsListObject flo(L"SpecialFeats");
    flo.Set_Feats(trainedFeats);
    Set_SpecialFeats(flo);

    // notify about the feat effects
    const Feat & feat = FindFeat(featName);
    ApplyFeatEffects(feat);

    NotifyFeatTrained(featName);
    m_pDocument->SetModifiedFlag(TRUE);

    if (feat.Acquire() == FeatAcquisition_RacialPastLife)
    {
        std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
        UpdateFeats(0, &allFeats);      // racial completionist state may have changed
    }

    if (feat.Acquire() == FeatAcquisition_HeroicPastLife)
    {
        std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
        UpdateFeats(2, &allFeats);      // completionist state may have changed
    }

    // number of past lives affects how many build points they have to spend
    DetermineBuildPoints();

    // special feats may change the number of action points available
    CountBonusRacialAP();
    CountBonusUniversalAP();
}

void Character::RevokeSpecialFeat(
        const std::string & featName,
        TrainableFeatTypes type)
{
    // just remove the first copy of the feat name from the current list
    std::list<TrainedFeat> trainedFeats = SpecialFeats().Feats();
    std::list<TrainedFeat>::iterator it = trainedFeats.begin();
    bool found = false;
    while (!found && it != trainedFeats.end())
    {
        if ((*it).FeatName() == featName)
        {
            // this is the first occurrence, remove it
            it = trainedFeats.erase(it);
            found = true;
        }
        else
        {
            ++it;
        }
    }
    if (found)
    {
        FeatsListObject flo(L"SpecialFeats");
        flo.Set_Feats(trainedFeats);
        Set_SpecialFeats(flo);
        // notify about the feat effects
        const Feat & feat = FindFeat(featName);
        RevokeFeatEffects(feat);
        NotifyFeatRevoked(featName);
        m_pDocument->SetModifiedFlag(TRUE);
        // number of past lives affects how many build points they have to spend
        DetermineBuildPoints();
        // special feats may change the number of action points available
        CountBonusRacialAP();
        CountBonusUniversalAP();
        if (feat.Acquire() == FeatAcquisition_RacialPastLife)
        {
            std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
            UpdateFeats(0, &allFeats);      // racial completionist state may have changed
        }
        if (feat.Acquire() == FeatAcquisition_HeroicPastLife)
        {
            std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
            UpdateFeats(2, &allFeats);      // completionist state may have changed
            // revoking a special feat in theory can invalidate a feat selection (e.g. completionist)
            VerifyTrainedFeats();
        }
    }
}

void Character::SetAbilityLevelUp(
        size_t level,
        AbilityType ability)
{
    AbilityType old = Ability_Unknown;
    switch (level)
    {
    case 4:
        old = Level4();
        Set_Level4(ability);
        break;
    case 8:
        old = Level8();
        Set_Level8(ability);
        break;
    case 12:
        old = Level12();
        Set_Level12(ability);
        break;
    case 16:
        old = Level16();
        Set_Level16(ability);
        break;
    case 20:
        old = Level20();
        Set_Level20(ability);
        break;
    case 24:
        old = Level24();
        Set_Level24(ability);
        break;
    case 28:
        old = Level28();
        Set_Level28(ability);
        break;
    case 32:
        old = Level32();
        Set_Level32(ability);
        break;
    case 36:
        old = Level36();
        Set_Level36(ability);
        break;
    case 40:
        old = Level40();
        Set_Level40(ability);
        break;
    default:
        ASSERT(FALSE);      // invalid level up level
        break;
    }
    NotifyAbilityValueChanged(old);
    NotifyAbilityValueChanged(ability);
    if (old == Ability_Intelligence
            || ability == Ability_Intelligence)
    {
        UpdateSkillPoints();    // ensure skill points are correct
    }
    // revoking an ability point in theory can invalidate a feat selection
    VerifyTrainedFeats();
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::TrainFeat(
        const std::string & featName,
        TrainableFeatTypes type,
        size_t level,
        bool autoTrained,
        bool suppressVerify)
{
    const Feat & feat = FindFeat(featName);

    bool featSwapWarning = false;
    if (level == 0)
    {
        // we evaluate the feat requirements again but without tomes being
        // applied to the characters abilities. This allows us to determine
        // whether the feat can be selected before entering the world or
        // after via a feat swap with Fred the Mind Flayer.
        if (!IsFeatTrainable(level, type, feat, false)
                && featName != " No Selection"
                || featName == "Completionist")
        {
            // this feat although trainable with Fred is not trainable
            // during character creation.
            featSwapWarning = true;
        }
    }
    // ensure re-selection of same feat in same slot does not change anything
    // as this can cause enhancements and feats to be revoked.
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    if (featName != (*it).FeatName(type) // is it the same feat that was previously selected?
            && !(featName == " No Selection" && (*it).FeatName(type) == ""))
    {
        // first revoke any previous trained feat in this slot at level
        std::string lostFeat = (*it).RevokeFeat(type);
        if (!lostFeat.empty())
        {
            const Feat & feat = FindFeat(lostFeat);
            RevokeFeatEffects(feat);
        }
        if (featName != " No Selection")
        {
            // train new
            (*it).TrainFeat(featName, type, level, featSwapWarning);
            ApplyFeatEffects(feat);
        }

        NotifyFeatTrained(featName);
        // some automatic feats may have changed due to the trained feat
        UpdateFeats();
        // a feat change can invalidate a feat selection at at later level
        if (!suppressVerify)
        {
            VerifyTrainedFeats();
        }
        m_pDocument->SetModifiedFlag(TRUE);
        if (!autoTrained && !suppressVerify)
        {
            AutoTrainSingleSelectionFeats();
        }
        // Warforged feats can affect active stances
        UpdateArmorStances();
    }
}

void Character::TrainAlternateFeat(
        const std::string & featName,
        TrainableFeatTypes type,
        size_t level)
{
    const Feat & feat = FindFeat(featName);

    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    (*it).TrainAlternateFeat(featName, type, level);
}

void Character::NowActive()
{
    // keep all views up to date
    DetermineBuildPoints();
    AutoTrainSingleSelectionFeats();

    NotifyAllLoadedEffects();
    NotifyAllEnhancementEffects();
    NotifyAllReaperEnhancementEffects();
    NotifyAllU51DestinyEffects();
    ApplyGearEffects();     // apply effects from equipped gear
    m_previousGuildLevel = 0;   // ensure all effects apply
    ApplyGuildBuffs();
    SetAlignmentStances();
    UpdateWeaponStances();
    UpdateArmorStances();
    UpdateShieldStances();
    UpdateCenteredStance();
    UpdateGreensteelStances();
    NotifyAllSelfAndPartyBuffs();
    NotifyGearChanged(Inventory_Weapon1);   // updates both in breakdowns
}

std::list<TrainedFeat> Character::AutomaticFeats(
        size_t level,
        const std::list<TrainedFeat> & currentFeats) const
{
    // determine which feats are automatically gained at this level/class and race
    std::list<TrainedFeat> feats;
    // need to know how many and of what classes have been trained up to here
    std::vector<size_t> classLevels = ClassLevels(level);
    const LevelTraining & levelData = LevelData(level);
    const std::list<Feat> & allFeats = StandardFeats();
    level++;    // 1 based for level comparison

    std::list<Feat>::const_iterator it = allFeats.begin();
    while (it != allFeats.end())
    {
        // check every feats automatic acquisition entries (if any)
        const std::list<AutomaticAcquisition> & aa = (*it).AutomaticAcquireAt();
        bool acquire = false;
        std::list<AutomaticAcquisition>::const_iterator aait = aa.begin();
        while (!acquire && aait != aa.end())
        {
            // see if we meet the acquire criteria
            bool meetsSpecificLevel = true;
            bool meetsRace = false;
            if ((*aait).HasRace())
            {
                meetsRace = ((*aait).Race() == Race()
                        || (*aait).Race() == Race_All);
            }
            else
            {
                // no race criteria
                meetsRace = true;
            }
            bool meetsClass = false;
            bool meetsLevel = false;
            if ((*aait).HasClass())
            {
                if (levelData.HasClass())
                {
                    meetsClass = ((*aait).Class() == levelData.Class()
                            || (*aait).Class() == Class_All
                            || ((*aait).Class() == Class_NotEpic && levelData.Class() != Class_Epic));
                    // must meet level requirement also
                    if ((*aait).HasLevel())
                    {
                        meetsLevel = (classLevels[levelData.Class()] == (*aait).Level());
                    }
                    else
                    {
                        // no class level criteria
                        meetsLevel = true;
                    }
                }
                else
                {
                    // fails to meet class requirements (no selection at this level)
                }
            }
            else
            {
                // no class criteria
                meetsClass = true;
                // must match overall level if we have one
                if ((*aait).HasLevel())
                {
                    meetsLevel = (level == (*aait).Level());
                }
                else
                {
                    // no min level criteria
                    meetsLevel = true;
                }
            }
            if ((*aait).HasSpecificLevel())
            {
                meetsSpecificLevel = ((*aait).SpecificLevel() == level);
            }
            bool meetsRequirementsToGain = true;
            if ((*aait).HasRequirementsToGain())
            {
                meetsRequirementsToGain = (*aait).RequirementsToGain().Met(
                        *this,
                        classLevels,
                        level,
                        currentFeats,
                        true);      // do include tomes
            }
            // if we meet the auto acquire criteria, make sure
            // we also meet the feats Requirements
            if (meetsRace
                    && meetsClass
                    && meetsLevel
                    && meetsSpecificLevel
                    && meetsRequirementsToGain)
            {
                if ((*aait).HasIgnoreRequirements())
                {
                    // don't have to meet feat requirements
                    acquire = true;
                }
                else
                {
                    // let the feat determine whether its acquired
                    acquire = (*it).RequirementsToTrain().Met(
                            *this,
                            classLevels,
                            level,
                            currentFeats,
                            true);      // do include tomes
                }
            }
            ++aait;
        }
        if (acquire)
        {
            // first count how many times this feat has been acquired already
            size_t count = TrainedCount(
                    currentFeats,
                    (*it).Name());
            // by default unless specified by the feat otherwise, any feat
            // can only be acquired once
            if (count < (*it).MaxTimesAcquire()) // MaxTimesAcquire defaults to 1 if not specified in the file
            {
                // this feat is automatic at this level
                // train it!
                TrainedFeat feat;
                feat.Set_FeatName((*it).Name());
                feat.Set_Type(TFT_Automatic);
                feat.Set_LevelTrainedAt(level);
                feats.push_back(feat);
            }
        }
        ++it;
    }
    if (level == 1 && m_hasLamanniaMode)
    {
        // add the special Lamannia feat
        TrainedFeat feat;
        feat.Set_FeatName("Lamannia");
        feat.Set_Type(TFT_Automatic);
        feat.Set_LevelTrainedAt(level);
        feats.push_back(feat);
    }
    return feats;
}

void Character::ActivateStance(const Stance & stance)
{
    // first activate the stance if not already active
    if (!IsStanceActive(stance.Name()))
    {
        m_Stances.AddActiveStance(stance.Name());
        NotifyStanceActivated(stance.Name());
        // now revoke any stances that cannot be active at the same time as this stance
        const std::list<std::string> & incompatibles = stance.IncompatibleStance();
        std::list<std::string>::const_iterator isit = incompatibles.begin();
        while (isit != incompatibles.end())
        {
            m_Stances.RevokeStance((*isit));
            NotifyStanceDeactivated((*isit));
            ++isit;
        }
        // animal forms can affect weapon auto stances
        if (stance.Name() == "Wolf"
                || stance.Name() == "Winter Wolf"
                || stance.Name() == "Bear"
                || stance.Name() == "Dire Bear"
                || stance.Name() == "Fire Elemental"
                || stance.Name() == "Water Elemental")
        {
            UpdateWeaponStances();
        }
    }
}

void Character::DeactivateStance(const Stance & stance)
{
    // deactivate the stance if active
    if (IsStanceActive(stance.Name()))
    {
        // deactivation of a stance only affects that stance
        m_Stances.RevokeStance(stance.Name());
        NotifyStanceDeactivated(stance.Name());
        // animal forms can affect weapon auto stances
        if (stance.Name() == "Wolf"
                || stance.Name() == "Winter Wolf"
                || stance.Name() == "Bear"
                || stance.Name() == "Dire Bear"
                || stance.Name() == "Fire Elemental"
                || stance.Name() == "Water Elemental")
        {
            UpdateWeaponStances();
        }
    }
}

bool Character::IsStanceActive(const std::string & name, WeaponType wt) const
{
    bool ret = false;
    // some special stances are based on a slider position
    // all these stances start with a numeric with the format of
    // "<number>% <stance name>"
    CWnd * pWnd = AfxGetMainWnd();
    CMainFrame * pMainWnd = dynamic_cast<CMainFrame*>(pWnd);
    const CStancesView * pStancesView = pMainWnd->GetStancesView();
    if (name.find("%") != std::string::npos)
    {
        ret = pStancesView->IsStanceActive(name, wt);
    }
    else
    {
        ret = m_Stances.IsStanceActive(name);
        ret |= pStancesView->IsStanceActive(name, wt);
    }
    if (name == "FavoredWeapon")
    {
        ret = pStancesView->IsStanceActive("Favored Weapon", wt);
        //ret = m_Stances.IsStanceActive("Favored Weapon");   // check the weapon group
        // must also have at least 10 favored soul levels for this to apply
        // for Grace/Knowledge of battle
        if (ClassLevels(Class_FavoredSoul) < 10)
        {
            // not enough heroic levels for this to apply
            ret = false;
        }
    }
    return ret;
}

void Character::StanceSliderChanged()
{
    NotifyStanceActivated("");
}

bool Character::IsClassSkill(
        SkillType skill,
        size_t level) const
{
    bool isClassSkill = false;  // assume not a class skill

    // check the class at each previous trained level to see if its a class skill
    ASSERT(m_Levels.size() == MaxLevel());
    std::list<LevelTraining>::const_iterator it = m_Levels.begin();
    for (size_t li = 0; li <= level; ++li)
    {
        if ((*it).HasClass())
        {
            ClassType type = (*it).Class();
            if (::IsClassSkill(type, skill))
            {
                // ok, it is a class skill
                isClassSkill = true;
                break;  // no need to check the rest
            }
        }
        ++it;
    }
    return isClassSkill;
}

size_t Character::AbilityAtLevel(
        AbilityType ability,
        size_t level,
        bool includeTomes) const  // level is 0 based
{
    size_t abilityValue = 8
            + RacialModifier(Race(), ability)
            + m_BuildPoints.GetAbilitySpend(ability);

    abilityValue += LevelUpsAtLevel(ability, level);

    // Update 37 Patch 1 changed the way tomes apply
    // they now apply at:
    // +1-+2 Level 1
    // +3 Level 3
    // +4 Level 7
    // +5 Level 11
    // +6 Level 15
    // +7 Level 19
    // +8 Level 22
    if (includeTomes || level > 0) // level is 0 based
    {
        abilityValue += TomeAtLevel(ability, level);
    }

    return abilityValue;
}

int Character::LevelUpsAtLevel(
        AbilityType ability,
        size_t level) const
{
    size_t levelUps = 0;
    // add on level ups on 4, 8, 12, 16, 20, 24, 28, 32, 36 and 40
    if (Level4() == ability && level >= 3)
    {
        ++levelUps;
    }
    if (Level8() == ability && level >= 7)
    {
        ++levelUps;
    }
    if (Level12() == ability && level >= 11)
    {
        ++levelUps;
    }
    if (Level16() == ability && level >= 15)
    {
        ++levelUps;
    }
    if (Level20() == ability && level >= 19)
    {
        ++levelUps;
    }
    if (Level24() == ability && level >= 23)
    {
        ++levelUps;
    }
    if (Level28() == ability && level >= 27)
    {
        ++levelUps;
    }
    if (Level32() == ability && level >= 31)
    {
        ++levelUps;
    }
    if (Level36() == ability && level >= 35)
    {
        ++levelUps;
    }
    if (Level40() == ability && level >= 39)
    {
        ++levelUps;
    }
    return levelUps;
}

int Character::TomeAtLevel(
        AbilityType ability,
        size_t level) const
{
    // Update 37 Patch 1 changed the way tomes apply
    // they now apply at:
    // +1-+2 Level 1
    // +3 Level 3
    // +4 Level 7
    // +5 Level 11
    // +6 Level 15
    // +7 Level 19
    // +8 or higher Level 22
    // level is 1 based for this calculation
    ++level;
    size_t maxTome = AbilityTomeValue(ability);
    size_t maxAtLevel = 0;
    switch (level)
    {
    case 1:
    case 2:
        maxAtLevel = 2;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        maxAtLevel = 3;
        break;
    case 7:
    case 8:
    case 9:
    case 10:
        maxAtLevel = 4;
        break;
    case 11:
    case 12:
    case 13:
    case 14:
        maxAtLevel = 5;
        break;
    case 15:
    case 16:
    case 17:
    case 18:
        maxAtLevel = 6;
        break;
    case 19:
    case 20:
    case 21:
        maxAtLevel = 7;
        break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
        maxAtLevel = 999;   // no upper limit
        break;

    }
    size_t tv = min(maxTome, maxAtLevel);
    return tv;
}

AbilityType Character::AbilityLevelUp(size_t level) const
{
    switch (level)
    {
        case 4:     return Level4();
        case 8:     return Level8();
        case 12:    return Level12();
        case 16:    return Level16();
        case 20:    return Level20();
        case 24:    return Level24();
        case 28:    return Level28();
        case 32:    return Level32();
        case 36:    return Level36();
        case 40:    return Level40();
    }
    return Ability_Unknown;
}

double Character::MaxSkillForLevel(
        SkillType skill,
        size_t level) const       // 0 based
{
    double maxSkill = 0;
    // max skill depends on whether this skill is or has been a previous level
    // class skill.
    bool isClassSkill = IsClassSkill(
            skill,
            level);

    ++level;        // 1 based for calculation
    level = min(MAX_CLASS_LEVELS, level); // only valid for heroic levels
    if (isClassSkill)
    {
        // max skill is 3 + character level
        maxSkill = 3.0 + level;
    }
    else
    {
        // some skills must be a class skill to be trained
        if (skill == Skill_DisableDevice
                || skill == Skill_OpenLock)
        {
            // this skill cannot be trained unless its a class skill for
            // any of the classes in a multi-class character
            maxSkill = 0;
        }
        else
        {
            // max skill for cross class is (3 + level) / 2
            maxSkill = (3.0 + level) / 2.0;
        }
    }
    return maxSkill;
}

ClassType Character::Class(size_t index) const
{
    ClassType ct = Class_Unknown;
    switch (index)
    {
    case 0: ct = Class1(); break;
    case 1: ct = Class2(); break;
    case 2: ct = Class3(); break;
    }
    return ct;
}

bool Character::IsClassAvailable(
        ClassType type) const
{
    bool bAvailable = false;
    // return true if the class type is available to be trained
    // some classes are alignment dependent
    if (CanTrainClass(type, Alignment()))
    {
        // they can train this class
        bAvailable = true;
    }
    return bAvailable;
}

bool Character::IsClassRestricted(
        ClassType type) const
{
    bool bRestricted = false;
    // archetype/default classes of the same type are mutually exclusive
    std::vector<size_t> classLevels(ClassLevels(MaxLevel()));
    switch (type)
    {
    case Class_Bard:                bRestricted = (classLevels[Class_BardStormsinger] != 0); break;
    case Class_BardStormsinger:     bRestricted = (classLevels[Class_Bard] != 0); break;
    case Class_Cleric:              bRestricted = (classLevels[Class_ClericDarkApostate] != 0); break;
    case Class_ClericDarkApostate:  bRestricted = (classLevels[Class_Cleric] != 0); break;
    case Class_Druid:               bRestricted = (classLevels[Class_DruidBlightCaster] != 0); break;
    case Class_DruidBlightCaster:   bRestricted = (classLevels[Class_Druid] != 0); break;
    case Class_Fighter:             bRestricted = (classLevels[Class_FighterDragonLord] != 0); break;
    case Class_FighterDragonLord:   bRestricted = (classLevels[Class_Fighter] != 0); break;
    case Class_Paladin:             bRestricted = (classLevels[Class_PaladinSacredFist] != 0); break;
    case Class_PaladinSacredFist:   bRestricted = (classLevels[Class_Paladin] != 0); break;
    case Class_Ranger:              bRestricted = (classLevels[Class_RangerDarkHunter] != 0); break;
    case Class_RangerDarkHunter:    bRestricted = (classLevels[Class_Ranger] != 0); break;
    case Class_Warlock:             bRestricted = (classLevels[Class_WarlockAcolyteOfTheSkin] != 0); break;
    case Class_WarlockAcolyteOfTheSkin: bRestricted = (classLevels[Class_Warlock] != 0); break;
    }
    return bRestricted;
}

size_t Character::SpentAtLevel(
        SkillType skill,
        size_t level) const
{
    size_t spent = 0;
    const LevelTraining & levelData = LevelData(level);
    // count points spent in this skill at this level
    const std::list<TrainedSkill> & trainedSkills = levelData.TrainedSkills();
    std::list<TrainedSkill>::const_iterator tsit = trainedSkills.begin();
    while (tsit != trainedSkills.end())
    {
        if ((*tsit).Skill() == skill)
        {
            ++spent;
        }
        ++tsit;
    }
    return spent;
}

double Character::SkillAtLevel(
        SkillType skill,
        size_t level,
        bool includeTome) const
{
    double skillLevel = 0;  // assume untrained

    ASSERT(m_Levels.size() == MaxLevel());
    std::list<LevelTraining>::const_iterator it = m_Levels.begin();
    for (size_t li = 0; li <= level; ++li)
    {
        // full point per spend if its a class skill at this level
        // half point per spend if its a cross class skill at this level
        const LevelTraining & ld = (*it);
        bool isClassSkill = ::IsClassSkill(
                ld.HasClass() ? ld.Class() : Class_Unknown,
                skill);
        size_t spent = SpentAtLevel(skill, li);
        if (isClassSkill)
        {
            // full ranks for each train
            skillLevel += spent;
        }
        else
        {
            // half rank for each train
            skillLevel += (spent / 2.0);
        }
        ++it;
    }
    // add in any tome bonus if required
    if (includeTome)
    {
        // Skill tomes apply at levels 1 (+1), 1 (+2), 3 (+3), 7 (+4), 11 (+5)
        size_t maxTome = 2; // always at least +2 applies
        if (level >= 2) maxTome += 1;
        if (level >= 6) maxTome += 1;
        if (level >= 10) maxTome += 1;
        size_t tomeValue = SkillTomeValue(skill);
        skillLevel += min(tomeValue, maxTome);
    }
    return skillLevel;
}

std::vector<size_t> Character::ClassLevels(
        size_t level) const
{
    // return a vector of the number of class levels trained in each class
    // at the specified level
    std::vector<size_t> classLevels(Class_Count + 3, 0);    // 0 for each class at start
    std::list<LevelTraining>::const_iterator it = m_Levels.begin();
    size_t currentLevel = 0;
    while (it != m_Levels.end() && currentLevel <= level)
    {
        // check this level to see what the user class is
        ClassType type = (*it).HasClass() ? (*it).Class() : Class_Unknown;
        classLevels[type] += 1;        // count a class level, works even for Class_Unknown
        ++it;
        ++currentLevel;
    }
    classLevels[Class_All] = level;
    classLevels[Class_NotEpic] = MAX_CLASS_LEVELS;
    return classLevels;
}

size_t Character::ClassLevels(ClassType ct) const
{
    std::vector<size_t> classLevels(ClassLevels(MaxLevel()));
    return classLevels[ct];
}

std::vector<TrainableFeatTypes> Character::TrainableFeatTypeAtLevel(
        size_t level) const
{
    // determine what feat types can be trained at this level up
    // it should never be more than 3 possible feat types
    std::vector<TrainableFeatTypes> trainable;

    // see if we are at a level we can train a general standard feat choice
    if (level == 0
            || ((level + 1) % 3) == 0)
    {
        // we can train a standard feat selection at this level
        if (level < MAX_CLASS_LEVELS)
        {
            // TFT_Epic handles for level 21+
            trainable.push_back(TFT_Standard);
        }
    }
    if (Race() == Race_Kobold
            && level == 0)
    {
        // Kobolds get a tribe feat at level 1
        trainable.push_back(TFT_KoboldTribe);
    }
    if (Race() == Race_KoboldShamen
            && level == 0)
    {
        // Kobolds get a tribe feat at level 1
        trainable.push_back(TFT_KoboldTribe);
    }
    if (Race() == Race_Aasimar
            && level == 0)
    {
        // Aasimars get a bond feat at level 1
        // (not Scourge Aasimar as their bond feat assigned automatically)
        trainable.push_back(TFT_AasimarBond);
    }
    if (Race() == Race_Human
            && level == 0)
    {
        // humans get a bonus feat at level 1
        trainable.push_back(TFT_HumanBonus);
    }
    if (Race() == Race_PurpleDragonKnight
            && level == 0)
    {
        // PDKs get a bonus feat at level 1
        trainable.push_back(TFT_PDKBonus);
    }
    if (Race() == Race_HalfElf
            && level == 0)
    {
        // half elves get a dilettante feat at level 1
        trainable.push_back(TFT_HalfElfDilettanteBonus);
    }
    if (Race() == Race_Dragonborn
            && level == 0)
    {
        // dragon born get a bonus feat at level 1
        trainable.push_back(TFT_DragonbornRacial);
    }
    if ((Race() == Race_Shifter)
            && level == 0)
    {
        // shifters get a animalistic aspect at level 1
        trainable.push_back(TFT_AnimalisticAspect);
    }

    // all other bonus feat types depend on class and level
    // determine how many class levels we have in each class
    std::vector<size_t> classLevels = ClassLevels(level);
    // now look at what class is being trained and see if any bonus feats
    // apply at this class level
    std::list<LevelTraining>::const_iterator it = m_Levels.begin();
    std::advance(it, level);
    ClassType type = (*it).HasClass() ? (*it).Class() : Class_Unknown;
    switch (type)
    {
    case Class_Alchemist:
        // Alchemists get bonus feats at levels 4, 8, 12, 16 and 20
        if (classLevels[Class_Alchemist] % 4 == 0)
        {
            trainable.push_back(TFT_AlchemistBonus);
        }
        break;

    case Class_Artificer:
        // artificers get bonus feats at levels 4, 8, 12, 16 and 20
        if (classLevels[Class_Artificer] % 4 == 0)
        {
            trainable.push_back(TFT_ArtificerBonus);
        }
        break;

    case Class_Cleric:
        // clerics get a Follower of (faith) selection at level 1
        // domain selection at level 2
        // domain feats at 5, 9 and 14
        // and a deity feat at level 6
        if (classLevels[Class_Cleric] == 1)
        {
            if (NotPresentInEarlierLevel(level, TFT_FollowerOf))
            {
                trainable.push_back(TFT_FollowerOf);
            }
        }
        if (classLevels[Class_Cleric] == 2)
        {
            trainable.push_back(TFT_Domain);
        }
        if (classLevels[Class_Cleric] == 5
                || classLevels[Class_Cleric] == 9
                || classLevels[Class_Cleric] == 14)
        {
            trainable.push_back(TFT_DomainFeat);
        }
        if (classLevels[Class_Cleric] == 6)
        {
            if (NotPresentInEarlierLevel(level, TFT_Deity))
            {
                trainable.push_back(TFT_Deity);
            }
        }
        break;

    case Class_ClericDarkApostate:
        // clerics get a Follower of (faith) selection at level 1
        // domain selection at level 2
        // domain feats at 5, 9 and 14
        // and a deity feat at level 6
        if (classLevels[Class_ClericDarkApostate] == 1)
        {
            if (NotPresentInEarlierLevel(level, TFT_FollowerOf))
            {
                trainable.push_back(TFT_FollowerOf);
            }
        }
        if (classLevels[Class_ClericDarkApostate] == 2)
        {
            trainable.push_back(TFT_Domain);
        }
        if (classLevels[Class_ClericDarkApostate] == 5
                || classLevels[Class_ClericDarkApostate] == 9
                || classLevels[Class_ClericDarkApostate] == 14)
        {
            trainable.push_back(TFT_DomainFeat);
        }
        if (classLevels[Class_ClericDarkApostate] == 6)
        {
            if (NotPresentInEarlierLevel(level, TFT_Deity))
            {
                trainable.push_back(TFT_Deity);
            }
        }
        break;

    case Class_Druid:
        // druids can select a wild shape at levels 2 and 5, 8, 11, 13, 17
        if (classLevels[Class_Druid] == 2
                || classLevels[Class_Druid] == 5
                || classLevels[Class_Druid] == 8
                || classLevels[Class_Druid] == 11
                || classLevels[Class_Druid] == 13
                || classLevels[Class_Druid] == 17)
        {
            trainable.push_back(TFT_DruidWildShape);
        }
        break;

    case Class_DruidBlightCaster:
        // Blight Casters can select a wild shape at levels 2, 11, 13, 17
        if (classLevels[Class_DruidBlightCaster] == 2
                || classLevels[Class_DruidBlightCaster] == 11
                || classLevels[Class_DruidBlightCaster] == 13
                || classLevels[Class_DruidBlightCaster] == 17)
        {
            trainable.push_back(TFT_DruidWildShape);
        }
        // Blight Casters can select a kin Form at levels 4, 8
        if (classLevels[Class_DruidBlightCaster] == 4
                || classLevels[Class_DruidBlightCaster] == 8)
        {
            trainable.push_back(TFT_KinForm);
        }
        // Blight Casters can select a toggle imbue at levels 2 and 11
        if (classLevels[Class_DruidBlightCaster] == 2
                || classLevels[Class_DruidBlightCaster] == 11)
        {
            trainable.push_back(TFT_BlightCasterToggleImbue);
        }
        break;

    case Class_FavoredSoul:
        // favored souls can select a follower of faith at level 1
        // battle feat at level 2
        // a child of feat at level 3
        // a deity feat at level 6
        // heart feat at level 7
        // a beloved of feat at level 12
        // and a damage reduction feat at level 20
        if (classLevels[Class_FavoredSoul] == 1)
        {
            if (NotPresentInEarlierLevel(level, TFT_FollowerOf))
            {
                trainable.push_back(TFT_FollowerOf);
            }
        }
        if (classLevels[Class_FavoredSoul] == 2)
        {
            trainable.push_back(TFT_FavoredSoulBattle);
        }
        if (classLevels[Class_FavoredSoul] == 3)
        {
            trainable.push_back(TFT_ChildOf);
        }
        if (classLevels[Class_FavoredSoul] == 6)
        {
            if (NotPresentInEarlierLevel(level, TFT_Deity))
            {
                trainable.push_back(TFT_Deity);
            }
        }
        if (classLevels[Class_FavoredSoul] == 7)
        {
            trainable.push_back(TFT_FavoredSoulHeart);
        }
        if (classLevels[Class_FavoredSoul] == 12)
        {
            trainable.push_back(TFT_BelovedOf);
        }
        if (classLevels[Class_FavoredSoul] == 20)
        {
            trainable.push_back(TFT_DamageReduction);
        }
        // favored souls also get energy resistance at levels 5, 10 and 15
        if (classLevels[Class_FavoredSoul] == 5
                || classLevels[Class_FavoredSoul] == 10
                || classLevels[Class_FavoredSoul] == 15)
        {
            trainable.push_back(TFT_EnergyResistance);
        }
        break;

    case Class_Epic:
        // epic level feats can be selected at 21, 24, 27 and 30
        if (level == 20     // level is 0 based
                || level == 23
                || level == 26
                || level == 29)
        {
            trainable.push_back(TFT_EpicFeat);
        }

        // epic destiny feats can also be trained at levels 22, 25 and 28
        if (level == 21     // level is 0 based
                || level == 24
                || level == 27)
        {
            trainable.push_back(TFT_EpicDestinyFeat);
        }

        // legendary feat can only be trained at level 30
        if (level == 29)     // level is 0 based
        {
            trainable.push_back(TFT_LegendaryFeat);
        }
        break;

    case Class_Fighter:
        // fighters gain a bonus feat at level 1 and then all even levels thereafter
        if (classLevels[Class_Fighter] == 1
                || classLevels[Class_Fighter] % 2 == 0)
        {
            trainable.push_back(TFT_FighterBonus);
        }
        break;

    case Class_FighterDragonLord:
        // Dragon lords  gain a bonus feat at all even levels except at 8/16
        if (classLevels[Class_FighterDragonLord] == 1)
        {
            trainable.push_back(TFT_MinorDragonLordAura);
        }
        if (classLevels[Class_FighterDragonLord] == 8)
        {
            trainable.push_back(TFT_MajorDragonLordAura);
        }
        if (classLevels[Class_FighterDragonLord] == 16)
        {
            trainable.push_back(TFT_SuperiorDragonLordAura);
        }
        if (classLevels[Class_FighterDragonLord] % 2 == 0
            && classLevels[Class_FighterDragonLord] != 8
            && classLevels[Class_FighterDragonLord] != 16)
        {
            trainable.push_back(TFT_FighterBonus);
        }
        break;

    case Class_Legendary:
        // epic level feats can be selected at 33, 36, 39 and 30
        if (level == 32     // level is 0 based
                || level == 35
                || level == 38)
        {
            trainable.push_back(TFT_EpicFeat);
        }

        // epic destiny feats can also be trained at levels 31, 34, 37 and 40
        if (level == 30     // level is 0 based
                || level == 33
                || level == 36
                || level == 39)
        {
            trainable.push_back(TFT_EpicDestinyFeat);
        }
        break;

    case Class_Monk:
        // monks gain a bonus martial arts feat at levels 1, 2, 6
        if (classLevels[Class_Monk] == 1
                || classLevels[Class_Monk] == 2)
        {
            trainable.push_back(TFT_MonkBonus);
        }
        if (classLevels[Class_Monk] == 6)
        {
            // slightly different feat list available
            trainable.push_back(TFT_MonkBonus6);
        }
        // monks must choose a path at level 3
        if (classLevels[Class_Monk] == 3)
        {
            trainable.push_back(TFT_MonkPhilosphy);
        }
        break;

    case Class_Paladin:
        // paladins gain a follower of feat at level 1
        // and a deity feat at level 6
        if (classLevels[Class_Paladin] == 1)
        {
            if (NotPresentInEarlierLevel(level, TFT_FollowerOf))
            {
                trainable.push_back(TFT_FollowerOf);
            }
        }
        if (classLevels[Class_Paladin] == 6)
        {
            if (NotPresentInEarlierLevel(level, TFT_Deity))
            {
                trainable.push_back(TFT_Deity);
            }
        }
        break;

    case Class_PaladinSacredFist:
        // paladins gain a follower of feat at level 1
        // and a deity feat at level 6
        // For a Sacred Fist these are automatically assigned
        // if if they multiclass its possible this way to get
        // multiple FollowerOf and Deity feats
        //if (classLevels[Class_PaladinSacredFist] == 1)
        //{
        //    if (NotPresentInEarlierLevel(level, TFT_FollowerOf))
        //    {
        //        trainable.push_back(TFT_FollowerOf);
        //    }
        //}
        //if (classLevels[Class_PaladinSacredFist] == 6)
        //{
        //    if (NotPresentInEarlierLevel(level, TFT_Deity))
        //    {
        //        trainable.push_back(TFT_Deity);
        //    }
        //}
        break;

    case Class_Ranger:
        // rangers gain a favored enemy feat at levels 1, 5, 10, 15 and 20
        if (classLevels[Class_Ranger] == 1
                || classLevels[Class_Ranger] == 5
                || classLevels[Class_Ranger] == 10
                || classLevels[Class_Ranger] == 15
                || classLevels[Class_Ranger] == 20)
        {
            trainable.push_back(TFT_RangerFavoredEnemy);
        }
        break;

    case Class_RangerDarkHunter:
        // rangers gain a favored enemy feat at levels 1, 5, 10, 15 and 20
        if (classLevels[Class_RangerDarkHunter] == 1
                || classLevels[Class_RangerDarkHunter] == 5
                || classLevels[Class_RangerDarkHunter] == 10
                || classLevels[Class_RangerDarkHunter] == 15
                || classLevels[Class_RangerDarkHunter] == 20)
        {
            trainable.push_back(TFT_DarkHunterFavoredEnemy);
        }
        break;

    case Class_Rogue:
        // rogues gain special abilities at levels 10, 13, 16 and 19
        if (classLevels[Class_Rogue] == 10
                || classLevels[Class_Rogue] == 13
                || classLevels[Class_Rogue] == 16
                || classLevels[Class_Rogue] == 19)
        {
            trainable.push_back(TFT_RogueSpecialAbility);
        }
        break;

    case Class_Warlock:
        // warlocks can choose their pact feat at level 1
        if (classLevels[Class_Warlock] == 1)
        {
            trainable.push_back(TFT_WarlockPact);
        }
        // warlocks can select special pact abilities at levels 6 and 15
        if (classLevels[Class_Warlock] == 6
                || classLevels[Class_Warlock] == 15)
        {
            trainable.push_back(TFT_WarlockPactAbility);
        }
        // warlocks can select pact spells at levels 1, 5, 9, 14, 17 and 19
        // note that the pact spell at level 1 is handled by the pact feat
        if (classLevels[Class_Warlock] == 5
                || classLevels[Class_Warlock] == 9
                || classLevels[Class_Warlock] == 14
                || classLevels[Class_Warlock] == 17
                || classLevels[Class_Warlock] == 19)
        {
            trainable.push_back(TFT_WarlockPactSpell);
        }
        // warlocks can select save abilities at levels 4, 8, 12, 16 and 20
        if (classLevels[Class_Warlock] == 4
                || classLevels[Class_Warlock] == 8
                || classLevels[Class_Warlock] == 12
                || classLevels[Class_Warlock] == 16
                || classLevels[Class_Warlock] == 20)
        {
            trainable.push_back(TFT_WarlockPactSaveBonus);
        }
        // warlocks can train a resistance feat at level 10
        if (classLevels[Class_Warlock] == 10)
        {
            trainable.push_back(TFT_WarlockResistance);
        }
        break;

    case Class_WarlockAcolyteOfTheSkin:
        // warlocks can choose their pact feat at level 1
        if (classLevels[Class_WarlockAcolyteOfTheSkin] == 1)
        {
            trainable.push_back(TFT_TruePact);
        }
        // warlocks can select special pact abilities at level 6
        if (classLevels[Class_WarlockAcolyteOfTheSkin] == 6)
        {
            trainable.push_back(TFT_WarlockPactAbility);
        }
        // warlocks can select save abilities at levels 4, 8, 12, 16 and 20
        if (classLevels[Class_WarlockAcolyteOfTheSkin] == 4
                || classLevels[Class_WarlockAcolyteOfTheSkin] == 8
                || classLevels[Class_WarlockAcolyteOfTheSkin] == 12
                || classLevels[Class_WarlockAcolyteOfTheSkin] == 16
                || classLevels[Class_WarlockAcolyteOfTheSkin] == 20)
        {
            trainable.push_back(TFT_WarlockPactSaveBonus);
        }
        break;

    case Class_Wizard:
        // wizards gain a meta-magic feat at levels 1, 5, 10, 15 and 20
        if (classLevels[Class_Wizard] == 1
                || classLevels[Class_Wizard] == 5
                || classLevels[Class_Wizard] == 10
                || classLevels[Class_Wizard] == 15
                || classLevels[Class_Wizard] == 20)
        {
            trainable.push_back(TFT_WizardMetamagic);
        }
        break;
    }

    return trainable;
}

std::vector<Feat> Character::TrainableFeats(
        TrainableFeatTypes type,
        size_t level,
        const std::string & includeThisFeat) const
{
    std::vector<Feat> trainable;
    // iterate the list of all feats and see if the character qualifies
    // to train it
    CDDOCPApp * pApp = dynamic_cast<CDDOCPApp*>(AfxGetApp());
    if (pApp != NULL)
    {
        // need to know which feats have already been trained by this point
        // include any feats also trained at the current level
        std::list<TrainedFeat> currentFeats = CurrentFeats(level);

        const std::list<Feat> & allFeats = pApp->StandardFeats();
        std::list<Feat>::const_iterator it = allFeats.begin();
        while (it != allFeats.end())
        {
            if (IsFeatTrainable(level, type, (*it), true, false)
                    || (*it).Name() == includeThisFeat)
            {
                // they can select this one, add it to the available list
                // unless it is in the ignore list
                if ((*it).Name() == includeThisFeat
                        || m_bShowIgnoredItems
                        || !IsInIgnoreList((*it).Name()))
                {
                    trainable.push_back((*it));
                }
            }
            ++it;
        }
    }

    return trainable;
}

size_t Character::BaseAttackBonus(
        size_t level) const
{
    size_t bab = 0;
    // determine how many levels of each class have been trained up to this point
    std::vector<size_t> classLevels = ClassLevels(level);
    for (size_t ci = Class_Unknown + 1; ci < Class_Count; ++ci)
    {
        // have they trained any levels in this class?
        if (classLevels[ci] > 0)
        {
           double classBab = BAB((ClassType)ci);        // 0.5, 0.75 or 1.0
           bab += (size_t)(classBab * classLevels[ci]); // fractions dropped
        }
    }
    // epic levels award 1 BAB for each odd epic level
    int epicLevels = (int)classLevels[Class_Epic];
    while (epicLevels > 0) // level is 0 based
    {
        bab += 1;
        epicLevels -= 2;
    }
    return bab;
}

std::list<TrainedFeat> Character::CurrentFeats(
        size_t level) const
{
    // return a list of all the feats trained at the current level
    std::list<TrainedFeat> currentFeats;

    // first add any special feats (Past lives etc)
    const std::list<TrainedFeat> & specialFeats = SpecialFeats().Feats();
    currentFeats.insert(currentFeats.end(), specialFeats.begin(), specialFeats.end());

    // now add the automatic and the trained feats at each level up to the level wanted
    size_t currentLevel = 0;
    std::list<LevelTraining>::const_iterator ldit = m_Levels.begin();
    while (currentLevel <= level && ldit != m_Levels.end())
    {
        const LevelTraining & levelData = (*ldit);
        // add the automatic feats for this level
        const std::list<TrainedFeat> & autoFeats = levelData.AutomaticFeats().Feats();
        currentFeats.insert(currentFeats.end(), autoFeats.begin(), autoFeats.end());

        // add the trained feats for this level
        const std::list<TrainedFeat> & trainedFeats = levelData.TrainedFeats().Feats();
        currentFeats.insert(currentFeats.end(), trainedFeats.begin(), trainedFeats.end());

        ++currentLevel;
        ++ldit;
    }
    return currentFeats;
}

void Character::ApplyFeatEffects(const Feat & feat)
{
    // if we have just trained a feat that is also a stance
    // add a stance selection button for each
    const std::list<Stance> & stances = feat.StanceData();
    std::list<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyNewStance((*sit));
        ++sit;
    }
    // if we have any DC objects notify about them
    const std::list<DC> & dcs = feat.EffectDC();
    std::list<DC>::const_iterator dcit = dcs.begin();
    while (dcit != dcs.end())
    {
        NotifyNewDC((*dcit));
        ++dcit;
    }
    // get the list of effects this feat has
    const std::list<Effect> & effects = feat.Effects();
    std::list<Effect>::const_iterator feit = effects.begin();
    while (feit != effects.end())
    {
        NotifyFeatEffect(feat.Name(), (*feit));
        ++feit;
    }
    NotifyFeatTrained(feat.Name());
}

void Character::RevokeFeatEffects(const Feat & feat)
{
    // get the list of effects this feat has
    const std::list<Effect> & effects = feat.Effects();
    std::list<Effect>::const_iterator feit = effects.begin();
    while (feit != effects.end())
    {
        NotifyFeatEffectRevoked(feat.Name(), (*feit));
        ++feit;
    }
    NotifyFeatRevoked(feat.Name());
    // revoke any stances this feat has
    const std::list<Stance> & stances = feat.StanceData();
    std::list<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyRevokeStance((*sit));
        ++sit;
    }
    // if we have any DC objects notify about them
    const std::list<DC> & dcs = feat.EffectDC();
    std::list<DC>::const_iterator dcit = dcs.begin();
    while (dcit != dcs.end())
    {
        NotifyRevokeDC((*dcit));
        ++dcit;
    }
}

// enhancement support
bool Character::IsEnhancementTrained(
        const std::string & enhancementName,
        const std::string & selection,
        TreeType type) const
{
    // return true if this enhancement and selection is trained
    bool isTrained = (IsTrained(enhancementName, selection, type) != NULL);
    return isTrained;
}

std::list<TrainedEnhancement> Character::CurrentEnhancements() const
{
    // return a list of all current enhancements trained
    std::list<TrainedEnhancement> all;
    // iterate the list to see if its present
    std::list<EnhancementSpendInTree>::const_iterator it = m_EnhancementTreeSpend.begin();
    while (it != m_EnhancementTreeSpend.end())
    {
        const std::list<TrainedEnhancement> & te = (*it).Enhancements();
        all.insert(all.end(), te.begin(), te.end());
        ++it;
    }
    return all;
}

int Character::TotalPointsAvailable(const std::string & treeName, TreeType type) const
{
    int aps = 0;
    switch (type)
    {
    case TT_racial:
        aps = MAX_ACTION_POINTS + m_bonusRacialActionPoints;  // bonus APs only apply to racial tree
        break;
    case TT_enhancement:
        aps = MAX_ACTION_POINTS;
        break;
    case TT_universal:
        aps = MAX_ACTION_POINTS + m_bonusUniversalActionPoints;  // bonus APs only apply to universal tree(s)
        break;
    case TT_reaper:
        aps = 1000;  // no upper limit on reaper APs (i.e. you can buy all)
        break;
    case TT_epicDestiny:
        aps = (MaxLevel() - MAX_CLASS_LEVELS) * 4
                + static_cast<int>(FindBreakdown(Breakdown_DestinyPoints)->Total());
        break;
    }
    return aps;
}

int Character::AvailableActionPoints(const std::string & treeName, TreeType type) const
{
    // for actions points we need to track:
    // how many bonus action points the character has (m_bonusActionPoints)
    // how many action points have been spent in the racial tree (m_racialTreeSpend)
    // how many action points have been spent in the universal tree(s) (m_universalTreeSpend)
    // how many action points have been spent in other trees (m_classTreeSpend)
    int aps = 0;
    switch (type)
    {
    case TT_racial:
        aps = MAX_ACTION_POINTS
                + m_bonusRacialActionPoints - m_racialTreeSpend
                - max(m_universalTreeSpend - m_bonusUniversalActionPoints, 0)
                - m_classTreeSpend;
        break;
    case TT_enhancement:
        aps = MAX_ACTION_POINTS
                - max(m_racialTreeSpend - m_bonusRacialActionPoints, 0)
                - max(m_universalTreeSpend - m_bonusUniversalActionPoints, 0)
                - m_classTreeSpend;
        break;
    case TT_universal:
        aps = MAX_ACTION_POINTS
                - max(m_racialTreeSpend - m_bonusRacialActionPoints, 0)
                + m_bonusUniversalActionPoints - m_universalTreeSpend
                - m_classTreeSpend;
        break;
    case TT_reaper:
        aps = 1000;  // no upper limit on reaper APs (i.e. you can buy all)
        break;
    case TT_epicDestiny:
        aps = (MaxLevel() - MAX_CLASS_LEVELS) * 4
                + static_cast<int>(FindBreakdown(Breakdown_DestinyPoints)->Total())
                - m_destinyTreeSpend;
        break;
    }
    return aps;
}

int Character::APSpentInTree(const std::string & treeName)
{
    int spent = 0;
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    if (pItem != NULL)  // NULL if no items spent in tree or reaper/destiny tree
    {
        spent = pItem->Spent();
    }
    ReaperSpendInTree * pReaperItem = Reaper_FindTree(treeName);
    if (pReaperItem != NULL) // NULL if no items spent in tree or enhancement/destiny tree
    {
        spent = pReaperItem->Spent();
    }
    DestinySpendInTree * pDestinyItem = U51Destiny_FindTree(treeName);
    if (pDestinyItem != NULL) // NULL if no items spent in tree or enhancement/reaper tree
    {
        spent = pDestinyItem->Spent();
    }
    return spent;
}

const TrainedEnhancement * Character::IsTrained(
        const std::string & enhancementName,
        const std::string & selection,
        TreeType type) const
{
    // return NULL if item is not in the list
    const TrainedEnhancement * pItem = NULL;

    if (type == TT_enhancement
            || type == TT_universal
            || type == TT_racial
            || type == TT_unknown)
    {
        // iterate the list to see if its present
        std::list<EnhancementSpendInTree>::const_iterator it = m_EnhancementTreeSpend.begin();
        while (pItem == NULL
                && it != m_EnhancementTreeSpend.end())
        {
            const std::list<TrainedEnhancement> & te = (*it).Enhancements();
            std::list<TrainedEnhancement>::const_iterator teit = te.begin();
            while (pItem == NULL
                    && teit != te.end())
            {
                if ((*teit).EnhancementName() == enhancementName)
                {
                    // may have a required sub-selection for the enhancement
                    if (selection.empty()
                            || selection == (*teit).Selection())
                    {
                        pItem = &(*teit);
                        break;
                    }
                }
                ++teit;
            }
            ++it;
        }
    }

    if (type == TT_reaper
            || (type == TT_unknown && pItem == NULL))
    {
        // iterate the reaper list to see if its present
        std::list<ReaperSpendInTree>::const_iterator it = m_ReaperTreeSpend.begin();
        while (pItem == NULL
                && it != m_ReaperTreeSpend.end())
        {
            const std::list<TrainedEnhancement> & te = (*it).Enhancements();
            std::list<TrainedEnhancement>::const_iterator teit = te.begin();
            while (pItem == NULL
                    && teit != te.end())
            {
                if ((*teit).EnhancementName() == enhancementName)
                {
                    // may have a required sub-selection for the enhancement
                    if (selection.empty()
                            || selection == (*teit).Selection())
                    {
                        pItem = &(*teit);
                        break;
                    }
                }
                ++teit;
            }
            ++it;
        }
    }

    if (type == TT_epicDestiny
            || (type == TT_unknown && pItem == NULL))
    {
        // iterate the U51 epic destiny list to see if its present
        std::list<DestinySpendInTree>::const_iterator it = m_DestinyTreeSpend.begin();
        while (pItem == NULL
                && it != m_DestinyTreeSpend.end())
        {
            const std::list<TrainedEnhancement> & te = (*it).Enhancements();
            std::list<TrainedEnhancement>::const_iterator teit = te.begin();
            while (pItem == NULL
                    && teit != te.end())
            {
                if ((*teit).EnhancementName() == enhancementName)
                {
                    // may have a required sub-selection for the enhancement
                    if (selection.empty()
                            || selection == (*teit).Selection())
                    {
                        pItem = &(*teit);
                        break;
                    }
                }
                ++teit;
            }
            ++it;
        }
    }
    return pItem;
}

bool Character::Enhancement_IsTreeTrained(const std::string & tree) const
{
    // return tree if this is one of the selected trees
    return m_SelectedTrees.IsTreePresent(tree);
}

void Character::Enhancement_SwapTrees(const std::string & tree1, const std::string & tree2)
{
    m_SelectedTrees.SwapTrees(tree1, tree2);
    NotifyEnhancementTreeOrderChanged();
}

EnhancementSpendInTree * Character::Enhancement_FindTree(const std::string & treeName)
{
    // Find the tree tracking amount spent and enhancements trained
    EnhancementSpendInTree * pItem = NULL;
    std::list<EnhancementSpendInTree>::iterator it = m_EnhancementTreeSpend.begin();
    while (pItem == NULL
            && it != m_EnhancementTreeSpend.end())
    {
        if ((*it).TreeName() == treeName)
        {
            pItem = &(*it);
            break;
        }
        ++it;
    }
    return pItem;
}

void Character::Enhancement_TrainEnhancement(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
    const std::vector<size_t>& cost)
{
    // Find the tree tracking amount spent and enhancements trained
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    const EnhancementTree & eTree = GetEnhancementTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        EnhancementSpendInTree tree(treeName, eTree.Version());
        m_EnhancementTreeSpend.push_back(tree);
        pItem = &m_EnhancementTreeSpend.back();
    }
    size_t ranks = 0;
    const EnhancementTreeItem * pTreeItem = FindEnhancement(enhancementName);
    int spent = pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            pTreeItem->MinSpent(selection),
            pTreeItem->HasTier5(),
            &ranks);
    if (eTree.HasIsRacialTree())
    {
        m_racialTreeSpend += spent;
    }
    else
    {
        if (eTree.HasIsUniversalTree())
        {
            m_universalTreeSpend += spent;
        }
        else
        {
            m_classTreeSpend += spent;
        }
    }
    ASSERT(pTreeItem != NULL);
    if (HasTier5Tree() && pTreeItem->HasTier5())
    {
        ASSERT(Tier5Tree() == treeName);    // should only be able to train other tier 5's in the same tree
    }
    else
    {
        // no tier 5 enhancement yet selected
        if (pTreeItem->HasTier5())
        {
            // this is a tier 5, lockout all other tree's tier 5 enhancements
            Set_Tier5Tree(treeName);
        }
    }
    // track whether this is a tier 5 enhancement
    // now notify all and sundry about the enhancement effects
    ApplyEnhancementEffects(treeName, enhancementName, selection, 1);
    NotifyEnhancementTrained(enhancementName, selection, pTreeItem->HasTier5(), true);
    NotifyActionPointsChanged();
    NotifyAPSpentInTreeChanged(treeName);
    UpdateWeaponStances();
    SetAlignmentStances();
    UpdateCenteredStance();
}

void Character::Enhancement_RevokeEnhancement(
        const std::string& treeName,
        const std::string& revokedEnhancement)
{
    std::string revokedEnhancementCopy(revokedEnhancement);
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    if (pItem != NULL
            && pItem->Enhancements().size() > 0)
    {
        bool wasTier5 = pItem->HasTier5();
        // return points available to spend also
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        size_t spent = pItem->RevokeEnhancement(
                revokedEnhancementCopy,
                &revokedEnhancementSelection,
                &ranks);
        const EnhancementTree & eTree = GetEnhancementTree(treeName);
        if (eTree.HasIsRacialTree())
        {
            m_racialTreeSpend -= spent;
        }
        else
        {
            if (eTree.HasIsUniversalTree())
            {
                m_universalTreeSpend -= spent;
            }
            else
            {
                m_classTreeSpend -= spent;
            }
        }
        // now notify all and sundry about the enhancement effects
        // get the list of effects this enhancement has
        RevokeEnhancementEffects(treeName, revokedEnhancementCopy, revokedEnhancementSelection, 1);
        // determine whether we still have a tier 5 enhancement trained if the tree just had one
        // revoked in it
        if (HasTier5Tree() && Tier5Tree() == treeName)
        {
            // may have lost the tier 5 status, check the tree to see if any tier 5 are still trained
            if (!pItem->HasTier5())
            {
                Clear_Tier5Tree();  // no longer a tier 5 trained
            }
        }
        NotifyEnhancementRevoked(revokedEnhancementCopy, revokedEnhancementSelection, wasTier5, true);
        NotifyActionPointsChanged();
        NotifyAPSpentInTreeChanged(treeName);
        UpdateWeaponStances();
        SetAlignmentStances();
        UpdateCenteredStance();
        UpdateSpells();     // some enhancements can control what spells are available
    }
}

void Character::Enhancement_ResetEnhancementTree(std::string treeName)
{
    // a whole tree is being reset
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            Enhancement_RevokeEnhancement(treeName, pItem->Enhancements().back().EnhancementName());
            pItem = Enhancement_FindTree(treeName);
        }
        // now remove the tree entry from the list (not present if no spend in tree)
        std::list<EnhancementSpendInTree>::iterator it = m_EnhancementTreeSpend.begin();
        while (it != m_EnhancementTreeSpend.end())
        {
            if ((*it).TreeName() == treeName)
            {
                // done once we find it
                m_EnhancementTreeSpend.erase(it);
                break;
            }
            ++it;
        }
        NotifyEnhancementTreeReset();
        NotifyActionPointsChanged();
        NotifyAPSpentInTreeChanged(treeName);
        UpdateWeaponStances();
        UpdateCenteredStance();
    }
}

void Character::Enhancement_SetSelectedTrees(const SelectedEnhancementTrees & trees)
{
    Set_SelectedTrees(trees);
}

const EnhancementTree & Character::FindTree(const std::string & treeName) const
{
    const std::list<EnhancementTree> & trees = EnhancementTrees();
    // first find the tree we want
    std::list<EnhancementTree>::const_iterator tit = trees.begin();
    while (tit != trees.end())
    {
        if ((*tit).Name() == treeName)
        {
            // this is the one we want
            break;
        }
        ++tit;
    }
    return (*tit);
}

std::string Character::GetEnhancementName(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection)
{
    std::string name;
    const std::list<EnhancementTree> & trees = EnhancementTrees();
    // first find the tree we want
    std::list<EnhancementTree>::const_iterator tit = trees.begin();
    while (tit != trees.end())
    {
        if ((*tit).Name() == treeName)
        {
            // this is the one we want
            break;
        }
        ++tit;
    }
    if (tit != trees.end())
    {
        // we have the tree, now find the enhancement
        const EnhancementTreeItem * pItem = (*tit).FindEnhancementItem(enhancementName);
        if (pItem != NULL)
        {
            // we found the tree item
            name = pItem->DisplayName(selection);
        }
    }
    return name;
}

std::list<Effect> Character::GetEnhancementEffects(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection)
{
    std::list<Effect> effects;
    const std::list<EnhancementTree> & trees = EnhancementTrees();
    // first find the tree we want
    std::list<EnhancementTree>::const_iterator tit = trees.begin();
    while (tit != trees.end())
    {
        if ((*tit).Name() == treeName)
        {
            // this is the one we want
            break;
        }
        ++tit;
    }
    if (tit != trees.end())
    {
        // we have the tree, now find the enhancement
        const EnhancementTreeItem * pItem = (*tit).FindEnhancementItem(enhancementName);
        if (pItem != NULL)
        {
            // we found the tree item
            effects = pItem->ActiveEffects(selection);
        }
    }
    return effects;
}

std::list<DC> Character::GetEnhancementDCs(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection)
{
    std::list<DC> dcs;
    const std::list<EnhancementTree> & trees = EnhancementTrees();
    // first find the tree we want
    std::list<EnhancementTree>::const_iterator tit = trees.begin();
    while (tit != trees.end())
    {
        if ((*tit).Name() == treeName)
        {
            // this is the one we want
            break;
        }
        ++tit;
    }
    if (tit != trees.end())
    {
        // we have the tree, now find the enhancement
        const EnhancementTreeItem * pItem = (*tit).FindEnhancementItem(enhancementName);
        if (pItem != NULL)
        {
            // we found the tree item
            dcs = pItem->ActiveDCs(selection);
        }
    }
    return dcs;
}

void Character::SetBuildPoints(size_t buildPoints)
{
    m_BuildPoints.Set_UserSelectedSpend(buildPoints);
    m_BuildPoints.Set_AvailableSpend(buildPoints);
    m_pDocument->SetModifiedFlag(TRUE);
    NotifyAvailableBuildPointsChanged();
}

size_t Character::DetermineBuildPoints()
{
    // determine how many build points the current past life count
    // allows this character to have
    // note that we only consider heroic and racial past lives for
    // the past lives count to determine build points because:
    //   Epic past lives do not add to build points
    //   Iconic past lives indirectly do, as you also get a heroic past life
    size_t numPastLifeFeats = 0;
    const std::list<TrainedFeat> & feats = SpecialFeats().Feats();
    std::list<TrainedFeat>::const_iterator it = feats.begin();
    while (it != feats.end())
    {
        const Feat & feat = FindFeat((*it).FeatName());
        if (feat.Acquire() == FeatAcquisition_HeroicPastLife
                || feat.Acquire() == FeatAcquisition_RacialPastLife)
        {
            // it is a trained feat that affects number of available
            // build points
            ++numPastLifeFeats;
        }
        ++it;
    }
    // we do support 28/32 (adventurer vs Champion) build point selection
    size_t buildPoints = 28;    // assume
    if (Race() == Race_Drow)
    {
        // Drow go 28->30->32
        switch (numPastLifeFeats)
        {
            case 0: buildPoints = 28; break;
            case 1: buildPoints = 30; break;
            // all other past life counts is the maximum build points
            default: buildPoints = 32; break;
        }
    }
    else
    {
        // all other races go 28/32->34->36
        switch (numPastLifeFeats)
        {
            case 0:
                if (m_BuildPoints.HasUserSelectedSpend())
                {
                    buildPoints = m_BuildPoints.UserSelectedSpend(); break; // 28 or 32 depending on user selection
                }
                else
                {
                    buildPoints = 28;
                }
                break;
            case 1: buildPoints = 34; break;
            // all other past life counts is the maximum build points
            default: buildPoints = 36; break;
        }
    }
    if (m_BuildPoints.AvailableSpend() != buildPoints)
    {
        // we have had a change in the number of available build points
        m_BuildPoints.Set_AvailableSpend(buildPoints);
        NotifyAvailableBuildPointsChanged();
    }
    return buildPoints;
}

void Character::CountBonusRacialAP()
{
    // look through the list of all special feats and count how many bonus AP there are
    // if the number has changed from what we currently have, change it and notify
    CWinApp * pApp = AfxGetApp();
    CDDOCPApp * pDDOApp = dynamic_cast<CDDOCPApp*>(pApp);
    if (pDDOApp != NULL)
    {
        const std::list<Feat> & racialFeats = pDDOApp->RacialPastLifeFeats();
        size_t APcount = 0;
        std::list<Feat>::const_iterator fi = racialFeats.begin();
        while (fi != racialFeats.end())
        {
            size_t count = GetSpecialFeatTrainedCount(fi->Name());
            if (count > 0)
            {
                // look at all the feat effects and see if any affect our AP count
                const std::list<Effect> & effects = (*fi).Effects();
                std::list<Effect>::const_iterator ei = effects.begin();
                while (ei != effects.end())
                {
                    if (ei->Type() == Effect_APBonus)
                    {
                        // APs are always whole numbers
                        if (ei->HasAmount())
                        {
                            APcount += (size_t)ei->Amount() * count;
                        }
                        else if (ei->HasAmountVector())
                        {
                            APcount += (size_t)ei->AmountVector()[count-1];
                        }
                    }
                    ei++;
                }
            }
            fi++;
        }
        const std::list<Feat> & specialFeats = pDDOApp->SpecialFeats();
        fi = specialFeats.begin();
        while (fi != specialFeats.end())
        {
            size_t count = GetSpecialFeatTrainedCount(fi->Name());
            if (count > 0)
            {
                // look at all the feat effects and see if any affect our AP count
                const std::list<Effect> & effects = (*fi).Effects();
                std::list<Effect>::const_iterator ei = effects.begin();
                while (ei != effects.end())
                {
                    if (ei->Type() == Effect_APBonus)
                    {
                        // APs are always whole numbers
                        if (ei->HasAmount())
                        {
                            APcount += (size_t)ei->Amount() * count;
                        }
                        else if (ei->HasAmountVector())
                        {
                            APcount += (size_t)ei->AmountVector()[count-1];
                        }
                    }
                    ei++;
                }
            }
            fi++;
        }
        if (APcount != m_bonusRacialActionPoints)
        {
            m_bonusRacialActionPoints = APcount;
            NotifyActionPointsChanged();
        }
    }
}

void Character::CountBonusUniversalAP()
{
    // look through the list of all special feats and count how many bonus AP there are
    // if the number has changed from what we currently have, change it and notify
    CWinApp * pApp = AfxGetApp();
    CDDOCPApp * pDDOApp = dynamic_cast<CDDOCPApp*>(pApp);
    if (pDDOApp != NULL)
    {
        size_t UAPcount = 0;
        size_t DAPcount = 0;
        const std::list<Feat> & specialFeats = pDDOApp->SpecialFeats();
        std::list<Feat>::const_iterator fi = specialFeats.begin();
        while (fi != specialFeats.end())
        {
            size_t count = GetSpecialFeatTrainedCount(fi->Name());
            if (count > 0)
            {
                // look at all the feat effects and see if any affect our AP count
                const std::list<Effect> & effects = (*fi).Effects();
                std::list<Effect>::const_iterator ei = effects.begin();
                while (ei != effects.end())
                {
                    if (ei->Type() == Effect_UAPBonus)
                    {
                        // APs are always whole numbers
                        if (ei->HasAmount())
                        {
                            UAPcount += (size_t)ei->Amount() * count;
                        }
                        else if (ei->HasAmountVector())
                        {
                            UAPcount += (size_t)ei->AmountVector()[count-1];
                        }
                    }
                    if (ei->Type() == Effect_DestinyAPBonus)
                    {
                        // APs are always whole numbers
                        if (ei->HasAmount())
                        {
                            DAPcount += (size_t)ei->Amount() * count;
                        }
                        else if (ei->HasAmountVector())
                        {
                            DAPcount += (size_t)ei->AmountVector()[count-1];
                        }
                    }
                    ei++;
                }
            }
            fi++;
        }
        if (UAPcount != m_bonusUniversalActionPoints
                || DAPcount != m_bonusDestinyActionPoints)
        {
            m_bonusUniversalActionPoints = UAPcount;
            m_bonusDestinyActionPoints = DAPcount;
            NotifyActionPointsChanged();
        }
    }
}

void Character::JustLoaded()
{
    // when the user loads a previously saved file we need to do some house keeping
    // work to ensure everything is correct. This includes:
    //
    // 1. Determine if a trained tree enhancements need to be revoked due to a version
    //    number change. This can happen when the tree options are updated and
    //    the previously trained tree options are no longer compatible.
    // 2. If the tree is ok, work out how many AP are spent in each trained tree
    //    this is done dynamically like this to allow enhancement AP costs to change
    //    between updates without completely invaliding a series of enhancements
    //    spent in a tree.
    // 3. If the tree no longer exists. Remove its entry.

    // keep a message that will inform the user about changes made
    bool displayMessage = false;
    CString message;

    {
        std::list<EnhancementSpendInTree>::iterator etsit = m_EnhancementTreeSpend.begin();
        while (etsit != m_EnhancementTreeSpend.end())
        {
            const EnhancementTree & tree = GetEnhancementTree((*etsit).TreeName());
            if (tree.Name() == "")
            {
                // the tree no longer exists, just delete this entry
                CString text;
                text.Format("The tree \"%s\" no longer exists. All enhancements spent in this tree were revoked.\n",
                        (*etsit).TreeName().c_str());
                message += text;
                displayMessage = true;
                etsit = m_EnhancementTreeSpend.erase(etsit);
            }
            else
            {
                // is the recorded tree spend up to date with that loaded?
                size_t spendVersion = (*etsit).TreeVersion();
                if (spendVersion != tree.Version())
                {
                    // looks like this tree is now out of date, have to revoke all these
                    // enhancements in this tree (i.e. just delete it)
                    CString text;
                    text.Format("All enhancements in tree \"%s\" were revoked as the tree has since been superseded\n",
                            (*etsit).TreeName().c_str());
                    message += text;
                    displayMessage = true;
                    etsit = m_EnhancementTreeSpend.erase(etsit);
                }
                else
                {
                    // the tree is up to date, sum how many APs were spent in it
                    bool bBadEnhancement = false;
                    size_t apsSpent = 0;
                    std::list<TrainedEnhancement> te = (*etsit).Enhancements();
                    std::list<TrainedEnhancement>::iterator teit = te.begin();
                    while (teit != te.end())
                    {
                        const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                        if (pTreeItem != NULL)
                        {
                            std::string selection;
                            selection = (*teit).HasSelection() ? (*teit).Selection() : "";
                            for (size_t i = 0; i < (*teit).Ranks(); ++i)
                            {
                                apsSpent += pTreeItem->Cost(selection, i);
                            }
                            // cost also updated so revoke of items will work
                            (*teit).SetCost(pTreeItem->ItemCosts(selection));
                        }
                        else
                        {
                            bBadEnhancement = true;
                        }
                        ++teit;
                    }
                    if (!bBadEnhancement)
                    {
                        // now set it on the tree so it knows how much has been spent in it
                        (*etsit).Set_Enhancements(te);
                        (*etsit).SetSpent(apsSpent);
                        // done
                        ++etsit;
                        // we have to track action points spent in tree types
                        if (tree.HasIsRacialTree())
                        {
                            m_racialTreeSpend += apsSpent;
                        }
                        else
                        {
                            if (tree.HasIsUniversalTree())
                            {
                                m_universalTreeSpend += apsSpent;
                            }
                            else
                            {
                                m_classTreeSpend += apsSpent;
                            }
                        }
                    }
                    else
                    {
                        // looks like this tree is now out of date, have to revoke all these
                        // enhancements in this tree (i.e. just delete it)
                        CString text;
                        text.Format("All enhancements in tree \"%s\" were revoked as bad enhancement was encountered\n",
                                (*etsit).TreeName().c_str());
                        message += text;
                        displayMessage = true;
                        etsit = m_EnhancementTreeSpend.erase(etsit);
                    }
                }
            }
        }
    }

    {
        std::list<ReaperSpendInTree>::iterator rtsit = m_ReaperTreeSpend.begin();
        while (rtsit != m_ReaperTreeSpend.end())
        {
            const EnhancementTree & tree = GetEnhancementTree((*rtsit).TreeName());
            // is the recorded tree spend up to date with that loaded?
            size_t spendVersion = (*rtsit).TreeVersion();
            if (spendVersion != tree.Version())
            {
                // looks like this tree is now out of date, have to revoke all these
                // enhancements in this tree (i.e. just delete it)
                CString text;
                text.Format("All enhancements in tree \"%s\" were revoked as the tree has since been superseded\n",
                        (*rtsit).TreeName().c_str());
                message += text;
                displayMessage = true;
                rtsit = m_ReaperTreeSpend.erase(rtsit);
            }
            else
            {
                // the tree is up to date, sum how many APs were spent in it
                bool bBadEnhancement = false;
                size_t apsSpent = 0;
                std::list<TrainedEnhancement> te = (*rtsit).Enhancements();
                std::list<TrainedEnhancement>::iterator teit = te.begin();
                while (teit != te.end())
                {
                    const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                    if (pTreeItem != NULL)
                    {
                        std::string selection;
                        selection = (*teit).HasSelection() ? (*teit).Selection() : "";
                        for (size_t i = 0; i < (*teit).Ranks(); ++i)
                        {
                            apsSpent += pTreeItem->Cost(selection, i);
                        }
                        // cost also updated so revoke of items will work
                        (*teit).SetCost(pTreeItem->ItemCosts(selection));
                    }
                    else
                    {
                        bBadEnhancement = true;
                    }
                    ++teit;
                }
                if (!bBadEnhancement)
                {
                    // now set it on the tree so it knows how much has been spent in it
                    (*rtsit).Set_Enhancements(te);
                    (*rtsit).SetSpent(apsSpent);
                    // done
                    ++rtsit;
                }
                else
                    {
                    // looks like this tree is now out of date, have to revoke all these
                    // enhancements in this tree (i.e. just delete it)
                    CString text;
                    text.Format("All enhancements in tree \"%s\" were revoked as bad enhancement was encountered\n",
                            (*rtsit).TreeName().c_str());
                    message += text;
                    displayMessage = true;
                    rtsit = m_ReaperTreeSpend.erase(rtsit);
                }
            }
        }
    }

    {
        std::list<DestinySpendInTree>::iterator dsit = m_DestinyTreeSpend.begin();
        while (dsit != m_DestinyTreeSpend.end())
        {
            const EnhancementTree & tree = GetEnhancementTree((*dsit).TreeName());
            if (tree.Name() == "")
            {
                // the tree no longer exists, just delete this entry
                CString text;
                text.Format("The tree \"%s\" no longer exists. All enhancements spent in this tree were revoked.\n",
                        (*dsit).TreeName().c_str());
                message += text;
                displayMessage = true;
                dsit = m_DestinyTreeSpend.erase(dsit);
            }
            else
            {
                // is the recorded tree spend up to date with that loaded?
                size_t spendVersion = (*dsit).TreeVersion();
                if (spendVersion != tree.Version())
                {
                    // looks like this tree is now out of date, have to revoke all these
                    // enhancements in this tree (i.e. just delete it)
                    CString text;
                    text.Format("All enhancements in tree \"%s\" were revoked as the tree has since been superseded\n",
                            (*dsit).TreeName().c_str());
                    message += text;
                    displayMessage = true;
                    dsit = m_DestinyTreeSpend.erase(dsit);
                }
                else
                {
                    // the tree is up to date, sum how many APs were spent in it
                    bool bBadEnhancement = false;
                    size_t apsSpent = 0;
                    std::list<TrainedEnhancement> te = (*dsit).Enhancements();
                    std::list<TrainedEnhancement>::iterator teit = te.begin();
                    while (teit != te.end())
                    {
                        const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                        if (pTreeItem != NULL)
                        {
                            std::string selection;
                            selection = (*teit).HasSelection() ? (*teit).Selection() : "";
                            for (size_t i = 0; i < (*teit).Ranks(); ++i)
                            {
                                apsSpent += pTreeItem->Cost(selection, i);
                            }
                            // cost also updated so revoke of items will work
                            (*teit).SetCost(pTreeItem->ItemCosts(selection));
                        }
                        else
                        {
                            bBadEnhancement = true;
                        }
                        ++teit;
                    }
                    if (!bBadEnhancement)
                    {
                        // now set it on the tree so it knows how much has been spent in it
                        (*dsit).Set_Enhancements(te);
                        (*dsit).SetSpent(apsSpent);
                        // done
                        ++dsit;
                        m_destinyTreeSpend += apsSpent;
                    }
                    else
                    {
                        // looks like this tree is now out of date, have to revoke all these
                        // enhancements in this tree (i.e. just delete it)
                        CString text;
                        text.Format("All enhancements in tree \"%s\" were revoked as bad enhancement was encountered\n",
                                (*dsit).TreeName().c_str());
                        message += text;
                        displayMessage = true;
                        dsit = m_DestinyTreeSpend.erase(dsit);
                    }
                }
            }
        }
    }

    // racial completionist state may have changed
    std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
    for (size_t level = 0; level < MaxLevel(); ++level)
    {
        // automatic feats are no longer saved. regenerate them all
        UpdateFeats(level, &allFeats);
    }

    if (displayMessage)
    {
        AfxMessageBox(message, MB_ICONWARNING);
        m_pDocument->SetModifiedFlag(TRUE);
    }
    CountBonusRacialAP();
    CountBonusUniversalAP();
    UpdateSkillPoints(); // when tomes apply has changed

    VerifyTrainedFeats();   // ensure feats are still correct after an update

    // update loaded gear objects to latest versions loaded to pick up any changes
    // to fixed items.
    UpdateGearToLatestVersions();
}

void Character::VerifyTrainedFeats()
{
    // user has taken an action that could have invalidated a current feat selection
    // check them all to see if they are still valid, revoke those which are not
    // but also keep a list of them. This allows us to try and re-place them in other
    // feats slots if we can
    FeatsListObject revokedFeats(L"Revoked Feats");
 
    // keep a message that will inform the user about changes made
    bool displayMessage = false;
    CString message("The following feats were revoked as requirements no longer met:\n\n");
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    size_t level = 0;
    bool bOldState = m_bShowEpicOnly;
    m_bShowEpicOnly = false;    // stop heroic feats in epic slots being revoked
    // we have to repeat a levels revokes as we cannot guarantee the order that
    // the feats will be checked in. e.g. we check Mobility and Dodge in that order
    // The prerequisite for mobility is met as dodge is trained at the time its checked
    // but dodge is removed due to its requirements. We then have an orphaned mobility feat
    // who's requirements are no longer met. So we repeat the checks on the feats at this level
    // until such time as no feat revocations occur.
    while (it != m_Levels.end())
    {
        bool redoLevel = false;     // true if have to check feats at this level again
        FeatsListObject featList = (*it).TrainedFeats();
        const std::list<TrainedFeat> & feats = featList.Feats();
        std::list<TrainedFeat>::const_iterator fit = feats.begin();
        while (!redoLevel && fit != feats.end())
        {
            // is this feat trainable at this level?
            const Feat & feat = FindFeat((*fit).FeatName());
            if (!IsFeatTrainable(level, (*fit).Type(), feat, true, true))
            {
                // no longer trainable, remove it from the list
                revokedFeats.Add(*fit);
                // revoke from level training in this slot at level
                (*it).RevokeFeat((*fit).Type());
                // effects and feat revoked if it cannot be moved elsewhere later
                redoLevel = true;   // check the other feats at this level again
            }
            ++fit;
        }
        if (!redoLevel)
        {
            // only increment these if we are not checking the feats at this level again
            ++it;
            ++level;
        }
    }
    // if we have any revoked feats, try and re-train them in matching empty
    // feat slots which qualify to hold those feats
    std::list<TrainedFeat> feats = revokedFeats.Feats();
    if (feats.size() > 0)
    {
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        size_t level = 0;
        while (it != m_Levels.end())
        {
            // check each revoked feat to see if its trainable at this level in
            // an empty feat slot. If it is, train it and remove from the list
            std::list<TrainedFeat>::const_iterator fit = feats.begin();
            while (fit != feats.end())
            {
                // is this feat trainable at this level?
                const Feat & feat = FindFeat((*fit).FeatName());
                if (IsFeatTrainable(level, (*fit).Type(), feat, true, false))
                {
                    // slot must be empty
                    TrainedFeat tf = GetTrainedFeat(
                            level,
                            (*fit).Type());
                    // name is blank if no feat currently trained
                    if (tf.FeatName() == "")
                    {
                        // it is, train it
                        (*it).TrainFeat((*fit).FeatName(), (*fit).Type(), level, false);
                        fit = feats.erase(fit); // remove and go to next
                    }
                    else
                    {
                        ++fit;  // check the next
                    }
                }
                else
                {
                    ++fit;      // check the next
                }
            }
            ++it;
            ++level;
        }
    }
    // now generate the revoked message with those feats that could not be re-trained
    // in slots elsewhere
    std::list<TrainedFeat>::const_iterator fit = feats.begin();
    while (fit != feats.end())
    {
        const Feat & feat = FindFeat((*fit).FeatName());
        RevokeFeatEffects(feat);
        NotifyFeatRevoked(feat.Name());
        displayMessage = true;
        CString text;
        text.Format("Level %d: %s\n",
                (*fit).LevelTrainedAt() + 1,     // 0 based, 1 based for display
                (*fit).FeatName().c_str());
        message += text;
        ++fit;
    }
    m_bShowEpicOnly = bOldState;
    if (displayMessage)
    {
        AfxMessageBox(message, MB_ICONWARNING);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

bool Character::IsFeatTrainable(
        size_t level,
        TrainableFeatTypes type,
        const Feat & feat,
        bool includeTomes,
        bool alreadyTrained) const
{
    // function returns true if the given feat can be trained at this level
    // i.e. all required prerequisite feats/skills/abilities are met

    // need to know how many levels and of what classes they have trained
    std::vector<size_t> classLevels = ClassLevels(level);
    // need to know which feats have already been trained by this point
    // include any feats also trained at the current level
    std::list<TrainedFeat> currentFeats = CurrentFeats(level);

    // must be a trainable feat first
    bool canTrain = (feat.Acquire() == FeatAcquisition_Train);
    if (canTrain)
    {
        // be a member of the correct group type
        if (feat.HasGroup())
        {
            canTrain = IsInGroup(type, feat.Group());
        }
        else
        {
            // no group means its only in TFT_Standard
            canTrain = (type == TFT_Standard)
                    || (type == TFT_HumanBonus) // equivalent to a standard feat
                    || (type == TFT_PDKBonus)   // equivalent to a standard feat
                    || (type == TFT_EpicFeat);  // equivalent to a standard feat
        }
        if (!m_bShowUnavailableFeats)
        {
            if (type == TFT_EpicFeat
                    && m_bShowEpicOnly
                    && ((feat.HasGroup() && !feat.Group().HasIsEpicFeat())
                            || !feat.HasGroup()))
            {
                // user only wants to show epic feats
                canTrain = false;
            }
        }
    }
    if (canTrain && !m_bShowUnavailableFeats)
    {
        // do we meet the requirements to train this feat?
        canTrain = feat.RequirementsToTrain().Met(
                *this,
                classLevels,
                level,
                currentFeats,
                includeTomes);
    }
    if (canTrain)
    {
        // may have trained this feat the max times already
        size_t trainedCount = TrainedCount(currentFeats, feat.Name());
        if (alreadyTrained)
        {
            // were checking if an already trained feat is still trainable
            --trainedCount;
        }
        canTrain = (trainedCount < feat.MaxTimesAcquire());
    }
    // has to be one of the trainable feat types
    if (canTrain)
    {
        std::vector<TrainableFeatTypes> tfts = TrainableFeatTypeAtLevel(level);
        bool found = false;
        for (size_t i = 0; i < tfts.size(); ++i)
        {
            if (tfts[i] == type)
            {
                found = true;
            }
        }
        canTrain = found;
    }
    return canTrain;
}

bool Character::HasGrantedFeats() const
{
    return (m_grantedFeats.size() > 0);
}

const std::list<TrainedFeat> & Character::GrantedFeats() const
{
    return m_grantedFeats;
}

void Character::ApplyEnhancementEffects(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        size_t ranks)
{
    std::string displayName = GetEnhancementName(treeName, enhancementName, selection);
    std::list<Effect> effects = GetEnhancementEffects(treeName, enhancementName, selection);
    std::list<DC> dcs = GetEnhancementDCs(treeName, enhancementName, selection);
    for (size_t rank = 0; rank < ranks; ++rank)
    {
        {
            std::list<Effect>::const_iterator eit = effects.begin();
            while (eit != effects.end())
            {
                NotifyEnhancementEffect(displayName, (*eit), rank + 1);
                ++eit;
            }
        }
        {
            std::list<DC>::const_iterator dcit = dcs.begin();
            while (dcit != dcs.end())
            {
                NotifyNewDC(*dcit);
                ++dcit;
            }
        }
    }
}

void Character::RevokeEnhancementEffects(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        size_t ranks)
{
    std::string displayName = GetEnhancementName(treeName, enhancementName, selection);
    std::list<Effect> effects = GetEnhancementEffects(treeName, enhancementName, selection);
    std::list<DC> dcs = GetEnhancementDCs(treeName, enhancementName, selection);
    for (size_t rank = 0; rank < ranks; ++rank)
    {
        std::list<Effect>::const_iterator eit = effects.begin();
        while (eit != effects.end())
        {
            NotifyEnhancementEffectRevoked(displayName, (*eit), rank + 1);
            ++eit;
        }
        std::list<DC>::const_iterator dcit = dcs.begin();
        while (dcit != dcs.end())
        {
            NotifyRevokeDC(*dcit);
            ++dcit;
        }
    }
}

std::list<TrainedSpell> Character::TrainedSpells(
        ClassType classType,
        size_t level) const
{
    // return the list of trained spells for this class at this level
    std::list<TrainedSpell> spells;
    std::vector<TrainedSpell>::const_iterator it = m_TrainedSpells.begin();
    while (it != m_TrainedSpells.end())
    {
        if ((*it).Class() == classType
                && (*it).Level() == level)
        {
            spells.push_back((*it));
        }
        ++it;
    }
    return spells;
}

std::list<TrainedSpell> Character::FixedSpells(
        ClassType classType,
        size_t level) const
{
    std::list<TrainedSpell> spells;
    // this list is maintained dynamically by the SpellsControl object for this class
    // we need to go to that window to get the list of fixed spells
    const CSpellsControl * pSC = GetMainFrame()->GetSpellsControl(classType);
    if (pSC != NULL)
    {
        spells = pSC->FixedSpells(level);
    }
    return spells;
}

void Character::TrainSpell(
        ClassType classType,
        size_t level,
        const std::string & spellName)
{
    // add the spell to the list of those trained
    TrainedSpell spell;
    spell.Set_Class(classType);
    spell.Set_Level(level);
    spell.Set_SpellName(spellName);
    m_TrainedSpells.push_back(spell);
    std::sort(m_TrainedSpells.begin(), m_TrainedSpells.end());
    NotifySpellTrained(spell);
    m_pDocument->SetModifiedFlag(TRUE);
    // spell effects are handled by the SpellsControl object
}

void Character::RevokeSpell(
        ClassType classType,
        size_t level,
        const std::string & spellName)
{
    // add the spell to the list of those trained
    TrainedSpell spell;
    spell.Set_Class(classType);
    spell.Set_Level(level);
    spell.Set_SpellName(spellName);
    // remove it from the list
    bool found = false;
    std::vector<TrainedSpell>::iterator it = m_TrainedSpells.begin();
    while (!found && it != m_TrainedSpells.end())
    {
        if ((*it) == spell)
        {
            m_TrainedSpells.erase(it);
            found = true;
        }
        else
        {
            ++it;
        }
    }
    ASSERT(found);
    NotifySpellRevoked(spell);
    m_pDocument->SetModifiedFlag(TRUE);
    // spell effects are handled by the SpellsControl object
}

bool Character::IsSpellTrained(const std::string & spellName) const
{
    bool found = false;
    std::vector<TrainedSpell>::const_iterator it = m_TrainedSpells.begin();
    while (!found && it != m_TrainedSpells.end())
    {
        if ((*it).SpellName() == spellName)
        {
            found = true;
            break;
        }
        else
        {
            ++it;
        }
    }
    return found;
}

void Character::Reaper_TrainEnhancement(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        const std::vector<size_t>& cost)
{
    // Find the tree tracking amount spent and enhancements trained
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        const EnhancementTree & eTree = GetEnhancementTree(treeName);
        ReaperSpendInTree tree(treeName, eTree.Version());
        m_ReaperTreeSpend.push_back(tree);
        pItem = &m_ReaperTreeSpend.back();
    }
    size_t ranks = 0;
    std::string temp;
    const EnhancementTreeItem* pTreeItem = FindEnhancement(enhancementName, &temp);
    pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            pTreeItem->MinSpent(selection),
            pTreeItem->HasTier5(),
            &ranks);
    // now notify all and sundry about the enhancement effects
    ApplyEnhancementEffects(treeName, enhancementName, selection, 1);
    NotifyEnhancementTrained(enhancementName, selection, false, true);
    NotifyAPSpentInTreeChanged(treeName);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::Reaper_RevokeEnhancement(
        const std::string& treeName,
        const std::string& revokedEnhancement)
{
    std::string revokedEnhancementCopy(revokedEnhancement);
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem != NULL
            && pItem->Enhancements().size() > 0)
    {
        // return points available to spend also
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        pItem->RevokeEnhancement(
                revokedEnhancementCopy,
                &revokedEnhancementSelection,
                &ranks);
        // now notify all and sundry about the enhancement effects
        // get the list of effects this enhancement has
        RevokeEnhancementEffects(treeName, revokedEnhancementCopy, revokedEnhancementSelection, 1);
        NotifyEnhancementRevoked(revokedEnhancementCopy, revokedEnhancementSelection, false, true);
        NotifyAPSpentInTreeChanged(treeName);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

void Character::Reaper_ResetEnhancementTree(std::string treeName)
{
    // a whole tree is being reset
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            Reaper_RevokeEnhancement(treeName, pItem->Enhancements().back().EnhancementName());
            pItem = Reaper_FindTree(treeName);
        }
        // now remove the tree entry from the list (not present if no spend in tree)
        std::list<ReaperSpendInTree>::iterator it = m_ReaperTreeSpend.begin();
        while (it != m_ReaperTreeSpend.end())
        {
            if ((*it).TreeName() == treeName)
            {
                // done once we find it
                m_ReaperTreeSpend.erase(it);
                break;
            }
            ++it;
        }
        NotifyEnhancementTreeReset();
        NotifyAPSpentInTreeChanged(treeName);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

ReaperSpendInTree * Character::Reaper_FindTree(const std::string & treeName)
{
    // Find the tree tracking amount spent and enhancements trained
    ReaperSpendInTree * pItem = NULL;
    std::list<ReaperSpendInTree>::iterator it = m_ReaperTreeSpend.begin();
    while (pItem == NULL
            && it != m_ReaperTreeSpend.end())
    {
        if ((*it).TreeName() == treeName)
        {
            pItem = &(*it);
            break;
        }
        ++it;
    }
    return pItem;
}

void Character::ApplyAllEffects(
        const std::string & treename,
        const std::list<TrainedEnhancement> & enhancements)
{
    // for each item trained, apply its effects
    std::list<TrainedEnhancement>::const_iterator eit = enhancements.begin();
    while (eit != enhancements.end())
    {
        const EnhancementTreeItem * pTreeItem = FindEnhancement((*eit).EnhancementName());
        ApplyEnhancementEffects(
                treename,
                (*eit).EnhancementName(),
                (*eit).HasSelection() ? (*eit).Selection() : "",
                (*eit).Ranks());
        // enhancements may give multiple stances
        std::list<Stance> stances = pTreeItem->Stances((*eit).HasSelection() ? (*eit).Selection() : "");
        std::list<Stance>::const_iterator sit = stances.begin();
        while (sit != stances.end())
        {
            NotifyNewStance((*sit));
            ++sit;
        }
        ++eit;
    }
}

void Character::RevokeAllEffects(
        const std::string & treename,
        const std::list<TrainedEnhancement> & enhancements)
{
    // for each item trained, evoke its effects
    std::list<TrainedEnhancement>::const_iterator eit = enhancements.begin();
    while (eit != enhancements.end())
    {
        const EnhancementTreeItem * pTreeItem = FindEnhancement((*eit).EnhancementName());
        RevokeEnhancementEffects(
                treename,
                (*eit).EnhancementName(),
                (*eit).HasSelection() ? (*eit).Selection() : "",
                (*eit).Ranks());
        // enhancements may give multiple stances
        std::list<Stance> stances = pTreeItem->Stances((*eit).HasSelection() ? (*eit).Selection() : "");
        std::list<Stance>::const_iterator sit = stances.begin();
        while (sit != stances.end())
        {
            NotifyRevokeStance((*sit));
            ++sit;
        }
        ++eit;
    }
}

int Character::AvailableActionPoints() const
{
    return MAX_ACTION_POINTS
            + m_bonusRacialActionPoints
            - m_racialTreeSpend
            + m_bonusUniversalActionPoints
            - m_universalTreeSpend
            - m_classTreeSpend;
}

int Character::U51Destiny_AvailableDestinyPoints() const
{
    return (MaxLevel() - MAX_CLASS_LEVELS) * 4
            + static_cast<int>(FindBreakdown(Breakdown_DestinyPoints)->Total())
            - m_destinyTreeSpend;
}

int Character::BonusRacialActionPoints() const
{
    return m_bonusRacialActionPoints;
}

int Character::BonusUniversalActionPoints() const
{
    return m_bonusUniversalActionPoints;
}

// gear support
void Character::AddGearSet(const EquippedGear & gear)
{
    // add the new gear set to the end of the list
    m_GearSetups.push_back(gear);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::DeleteGearSet(const std::string & name)
{
    RevokeGearEffects();        // always for active gear (one being deleted)
    // find the gear set and delete it
    std::list<EquippedGear>::const_iterator it = m_GearSetups.begin();
    while (it != m_GearSetups.end())
    {
        if ((*it).Name() == name)
        {
            m_GearSetups.erase(it);
            break;  // were done
        }
        ++it;
    }
    // as we just deleted the active gear set, switch to the first gear set remaining (if any)
    if (m_GearSetups.size() > 0)
    {
        it = m_GearSetups.begin();
        Set_ActiveGear((*it).Name());
    }
    else
    {
        // no gear sets, no name
        Set_ActiveGear("");
    }
    ApplyGearEffects();         // always for active gear
    NotifyGearChanged(Inventory_Weapon1);   // updates both in breakdowns
    m_pDocument->SetModifiedFlag(TRUE);
}

bool Character::DoesGearSetExist(const std::string & name) const
{
    std::list<EquippedGear>::const_iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == name)
        {
            found = true;
        }
        ++it;
    }
    return found;
}

EquippedGear Character::GetGearSet(const std::string & name) const
{
    EquippedGear gear;
    std::list<EquippedGear>::const_iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == name)
        {
            gear = (*it);
            found = true;
        }
        ++it;
    }
    return gear;
}

void Character::SetActiveGearSet(const std::string & name)
{
    RevokeGearEffects();        // always for active gear
    Set_ActiveGear(name);
    ApplyGearEffects();         // always for active gear
    NotifyGearChanged(Inventory_Weapon1);   // updates both in breakdowns
    UpdateWeaponStances();
    UpdateArmorStances();
    UpdateShieldStances();
    UpdateCenteredStance();
    m_pDocument->SetModifiedFlag(TRUE);
}

EquippedGear Character::ActiveGearSet() const
{
    return GetGearSet(ActiveGear());
}

void Character::SetNumFiligrees(size_t count)
{
    // first revoke all gear effects as the gear is about to change
    RevokeGearEffects();        // always for active gear
    // update the gear
    std::list<EquippedGear>::iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == ActiveGear())
        {
            (*it).SetNumFiligrees(count);
            found = true;
        }
        ++it;
    }
    // now apply the gear effects if its the active gear set
    ApplyGearEffects();         // always for active gear
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::UpdateActiveGearSet(const EquippedGear & newGear)
{
    // first revoke all gear effects as the gear is about to change
    RevokeGearEffects();        // always for active gear
    // update the gear
    std::list<EquippedGear>::iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == ActiveGear())
        {
            (*it) = newGear;
            found = true;
        }
        ++it;
    }
    // now apply the gear effects if its the active gear set
    ApplyGearEffects();         // always for active gear
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::SetGear(
        const std::string & name,
        InventorySlotType slot,
        const Item & item)
{
    if (name == ActiveGear())
    {
        // first revoke all gear effects as the gear is about to change
        RevokeGearEffects();        // always for active gear
    }
    // update the gear entry
    std::list<EquippedGear>::iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == name)
        {
            (*it).SetItem(slot, this, item);
            found = true;
            // clear any items from restricted gear slots if required
            if (item.HasRestrictedSlots())
            {
                for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
                {
                    if (item.RestrictedSlots().HasSlot((InventorySlotType)i))
                    {
                        (*it).ClearItem((InventorySlotType)i);
                    }
                }
            }
        }
        ++it;
    }
    ASSERT(found);

    // now apply the gear effects if its the active gear set
    if (name == ActiveGear())
    {
        ApplyGearEffects();         // always for active gear
    }
    NotifyGearChanged(slot);
    UpdateWeaponStances();
    UpdateArmorStances();
    UpdateShieldStances();
    UpdateCenteredStance();
    UpdateGreensteelStances();
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::ClearGearInSlot(const std::string & name, InventorySlotType slot)
{
    if (name == ActiveGear())
    {
        // first revoke all gear effects as the gear is about to change
        RevokeGearEffects();        // always for active gear
    }
    // update the gear entry
    std::list<EquippedGear>::iterator it = m_GearSetups.begin();
    bool found = false;
    while (!found && it != m_GearSetups.end())
    {
        if ((*it).Name() == name)
        {
            (*it).ClearItem(slot);
            found = true;
        }
        ++it;
    }
    ASSERT(found);

    // now apply the gear effects if its the active gear set
    if (name == ActiveGear())
    {
        ApplyGearEffects();         // always for active gear
    }
    NotifyGearChanged(slot);
    UpdateWeaponStances();
    UpdateArmorStances();
    UpdateShieldStances();
    UpdateCenteredStance();
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::RevokeGearEffects()
{
    EquippedGear gear = ActiveGearSet(); // empty if no gear found
    // iterate the items
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (gear.HasItemInSlot((InventorySlotType)i))
        {
            bool bSuppressSetBonuses = false;
            Item item = gear.ItemInSlot((InventorySlotType)i);
            // revoke the items effects
            const std::vector<Effect> & effects = item.Effects();
            std::vector<Effect>::const_iterator it = effects.begin();
            while (it != effects.end())
            {
                if (i == Inventory_Ring2 || i == Inventory_Weapon2)
                {
                    // stop duplicate items in multiple slots effects from stacking
                    NotifyItemEffectRevoked(item.Name() + " ", (*it));
                }
                else
                {
                    NotifyItemEffectRevoked(item.Name(), (*it));
                }
                ++it;
            }
            const std::list<DC> & dcs = item.EffectDC();
            std::list<DC>::const_iterator dcit = dcs.begin();
            while (dcit != dcs.end())
            {
                NotifyRevokeDC(*dcit);
                ++dcit;
            }
            const std::list<Stance> & stances = item.Stances();
            std::list<Stance>::const_iterator sit = stances.begin();
            while (sit != stances.end())
            {
                NotifyRevokeStance((*sit));
                ++sit;
            }
            // do the same for any augment slots
            const std::vector<ItemAugment> & augments = item.Augments();
            for (size_t ai = 0; ai < augments.size(); ++ai)
            {
                if (augments[ai].HasSelectedAugment())
                {
                    // there is an augment in this position
                    const Augment & augment = FindAugmentByName(augments[ai].SelectedAugment());
                    bSuppressSetBonuses |= augment.HasSuppressSetBonus();
                    // name is:
                    // <item>:<augment type>:<Augment name>
                    std::stringstream ss;
                    ss << item.Name()
                            << " : " << augments[ai].Type()
                            << " : " << augment.Name();
                    // now revoke the augments effects
                    std::string name;
                    name = ss.str();
                    std::list<Effect> effects = augment.Effects();
                    std::list<Effect>::iterator it = effects.begin();
                    size_t effectIndex = 0;
                    while (it != effects.end())
                    {
                        if (augment.HasEnterValue())
                        {
                            if (augment.HasDualValues())
                            {
                                if (effectIndex == 0)
                                {
                                    if (augments[ai].HasValue())
                                    {
                                        (*it).Set_Amount(augments[ai].Value());
                                    }
                                }
                                else
                                {
                                    if (augments[ai].HasValue2())
                                    {
                                        (*it).Set_Amount(augments[ai].Value2());
                                    }
                                }
                            }
                            else if (augments[ai].HasValue())
                            {
                                (*it).Set_Amount(augments[ai].Value());
                            }
                        }
                        if (!(*it).HasIsItemSpecific())
                        {
                            NotifyItemEffectRevoked(name, (*it));
                        }
                        ++it;
                        ++effectIndex;
                    }
                    // clear any augment stances
                    const std::list<Stance> & stances = augment.StanceData();
                    std::list<Stance>::const_iterator sit = stances.begin();
                    while (sit != stances.end())
                    {
                        NotifyRevokeStance((*sit));
                        ++sit;
                    }
                    // revoke any augment set bonuses
                    const std::list<std::string> & setBonuses = augment.SetBonus();
                    std::list<std::string>::const_iterator sbit = setBonuses.begin();
                    while (sbit != setBonuses.end())
                    {
                        RevokeSetBonus((*sbit), name);
                        ++sbit;
                    }
                }
            }
            if (!bSuppressSetBonuses)
            {
                // revoke any item set bonuses
                const std::list<std::string> & setBonuses = item.SetBonus();
                std::list<std::string>::const_iterator sbit = setBonuses.begin();
                while (sbit != setBonuses.end())
                {
                    RevokeSetBonus((*sbit), item.Name());
                    ++sbit;
                }
            }
        }
        else if (i == Inventory_Armor)
        {
            // need to remove the no armor effects
            Item noArmor = FindItem("No Armor Effects");
            const std::vector<Effect> & effects = noArmor.Effects();
            std::vector<Effect>::const_iterator it = effects.begin();
            while (it != effects.end())
            {
                NotifyItemEffectRevoked(noArmor.Name(), (*it));
                ++it;
            }
        }
    }
    // revoke this gears sentient weapon Filigrees
    for (size_t si = 0 ; si < MAX_FILIGREE; ++si)
    {
        std::string filigree = gear.SentientIntelligence().GetFiligree(si);
        RevokeFiligree(filigree, si, false);
    }
    if (gear.HasMinorArtifact())
    {
        for (size_t si = 0 ; si < MAX_ARTIFACT_FILIGREE; ++si)
        {
            std::string filigree = gear.SentientIntelligence().GetArtifactFiligree(si);
            RevokeFiligree(filigree, si, true);
        }
    }
}

void Character::RevokeFiligree(
        const std::string & filigree,
        size_t si,
        bool bArtifact)
{
    if (filigree != "")
    {
        // there is an augment in this position
        const Augment & augment = FindAugmentByName(filigree);
        // name is:
        // <item>:<augment type>:<Augment name>
        std::stringstream ss;
        ss << "Filigree " << (si + 1)
                << ": " << augment.Name();
        // now revoke the augments effects
        std::string name;
        name = ss.str();
        std::list<Effect> effects = augment.Effects();
        std::list<Effect>::iterator it = effects.begin();
        while (it != effects.end())
        {
            NotifyItemEffectRevoked(name, (*it));
            ++it;
        }
        // now do any rare effects
        EquippedGear gear = ActiveGearSet(); // empty if no gear found
        if (bArtifact)
        {
            if (gear.SentientIntelligence().IsRareArtifactFiligree(si))
            {
                std::list<Effect> effects = augment.Rares().Effects();
                std::list<Effect>::iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffectRevoked(name, (*it));
                    ++it;
                }
            }
        }
        else
        {
            if (gear.SentientIntelligence().IsRareFiligree(si))
            {
                std::list<Effect> effects = augment.Rares().Effects();
                std::list<Effect>::iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffectRevoked(name, (*it));
                    ++it;
                }
            }
        }
        // revoke any filigree stances
        const std::list<Stance> & stances = augment.StanceData();
        std::list<Stance>::const_iterator sit = stances.begin();
        while (sit != stances.end())
        {
            NotifyRevokeStance((*sit));
            ++sit;
        }
        // revoke any filigree set bonuses
        const std::list<std::string> & setBonuses = augment.SetBonus();
        std::list<std::string>::const_iterator sbit = setBonuses.begin();
        while (sbit != setBonuses.end())
        {
            RevokeSetBonus((*sbit), name);
            ++sbit;
        }
    }
}

void Character::ApplyGearEffects()
{
    EquippedGear gear = ActiveGearSet();
    Item noArmor = FindItem("No Armor Effects");
    // iterate the items
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (gear.HasItemInSlot((InventorySlotType)i))
        {
            bool bSuppressSetBonuses = false;
            if (i == Inventory_Armor)
            {
                // need to remove the no armor effects
                const std::vector<Effect> & effects = noArmor.Effects();
                std::vector<Effect>::const_iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffectRevoked(noArmor.Name(), (*it));
                    ++it;
                }
            }
            Item item = gear.ItemInSlot((InventorySlotType)i);
            // apply the items effects
            const std::vector<Effect> & effects = item.Effects();
            std::vector<Effect>::const_iterator it = effects.begin();
            while (it != effects.end())
            {
                if (i == Inventory_Ring2 || i == Inventory_Weapon2)
                {
                    // stop duplicate items in multiple slots effects from stacking
                    NotifyItemEffect(item.Name() + " ", (*it));
                }
                else
                {
                    NotifyItemEffect(item.Name(), (*it));
                }
                ++it;
            }
            const std::list<DC> & dcs = item.EffectDC();
            std::list<DC>::const_iterator dcit = dcs.begin();
            while (dcit != dcs.end())
            {
                NotifyNewDC(*dcit);
                ++dcit;
            }
            const std::list<Stance> & stances = item.Stances();
            std::list<Stance>::const_iterator sit = stances.begin();
            while (sit != stances.end())
            {
                NotifyNewStance((*sit));
                ++sit;
            }
            // do the same for any augment slots
            const std::vector<ItemAugment> & augments = item.Augments();
            for (size_t ai = 0; ai < augments.size(); ++ai)
            {
                if (augments[ai].HasSelectedAugment())
                {
                    // there is an augment in this position
                    const Augment & augment = FindAugmentByName(augments[ai].SelectedAugment());
                    bSuppressSetBonuses |= augment.HasSuppressSetBonus();
                    // name is:
                    // <item>:<augment type>:<Augment name>
                    std::stringstream ss;
                    ss << item.Name()
                            << " : " << augments[ai].Type()
                            << " : " << augment.Name();
                    // now notify the augments effects
                    std::string name;
                    name = ss.str();
                    std::list<Effect> effects = augment.Effects();
                    std::list<Effect>::iterator it = effects.begin();
                    size_t effectIndex = 0;
                    while (it != effects.end())
                    {
                        if (augment.HasEnterValue())
                        {
                            if (augment.HasDualValues())
                            {
                                if (effectIndex == 0)
                                {
                                    if (augments[ai].HasValue())
                                    {
                                        (*it).Set_Amount(augments[ai].Value());
                                    }
                                }
                                else
                                {
                                    if (augments[ai].HasValue2())
                                    {
                                        (*it).Set_Amount(augments[ai].Value2());
                                    }
                                }
                            }
                            else if (augments[ai].HasValue())
                            {
                                (*it).Set_Amount(augments[ai].Value());
                            }
                        }
                        if (!(*it).HasIsItemSpecific())
                        {
                            NotifyItemEffect(name, (*it));
                        }
                        ++it;
                        ++effectIndex;
                    }
                    // apply any augment stances
                    const std::list<Stance> & stances = augment.StanceData();
                    std::list<Stance>::const_iterator sit = stances.begin();
                    while (sit != stances.end())
                    {
                        NotifyNewStance((*sit));
                        ++sit;
                    }
                    // apply any augment set bonuses
                    const std::list<std::string> & setBonuses = augment.SetBonus();
                    std::list<std::string>::const_iterator sbit = setBonuses.begin();
                    while (sbit != setBonuses.end())
                    {
                        ApplySetBonus((*sbit), name);
                        ++sbit;
                    }
                }
            }
            // apply any item set bonuses
            if (!bSuppressSetBonuses)
            {
                const std::list<std::string> & setBonuses = item.SetBonus();
                std::list<std::string>::const_iterator sbit = setBonuses.begin();
                while (sbit != setBonuses.end())
                {
                    ApplySetBonus((*sbit), item.Name());
                    ++sbit;
                }
            }
        }
        else
        {
            // if the armor slot is empty, notify some default effects equivalent to no armor
            if (i == Inventory_Armor)
            {
                // apply the items effects
                const std::vector<Effect> & effects = noArmor.Effects();
                std::vector<Effect>::const_iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffect(noArmor.Name(), (*it));
                    ++it;
                }
            }
        }
    }
    // apply this gears sentient weapon and artifact Filigrees
    for (size_t si = 0 ; si < MAX_FILIGREE; ++si)
    {
        std::string filigree = gear.SentientIntelligence().GetFiligree(si);
        ApplyFiligree(filigree, si, false);
    }
    if (gear.HasMinorArtifact())
    {
        for (size_t si = 0 ; si < MAX_ARTIFACT_FILIGREE; ++si)
        {
            std::string filigree = gear.SentientIntelligence().GetArtifactFiligree(si);
            ApplyFiligree(filigree, si, true);
        }
    }

    UpdateWeaponStances();
    UpdateArmorStances();
    UpdateShieldStances();
    UpdateCenteredStance();
}

void Character::ApplyFiligree(
        const std::string & filigree,
        size_t si,
        bool bArtifact)
{
    if (filigree != "")
    {
        // there is an augment in this position
        const Augment & augment = FindAugmentByName(filigree);
        // name is:
        // <item>:<augment type>:<Augment name>
        std::stringstream ss;
        ss << "Filigree " << (si + 1)
                << ": " << augment.Name();
        // now revoke the augments effects
        std::string name;
        name = ss.str();
        std::list<Effect> effects = augment.Effects();
        std::list<Effect>::iterator it = effects.begin();
        while (it != effects.end())
        {
            NotifyItemEffect(name, (*it));
            ++it;
        }
        // now do any rare effects
        EquippedGear gear = ActiveGearSet(); // empty if no gear found
        if (bArtifact)
        {
            if (gear.SentientIntelligence().IsRareArtifactFiligree(si))
            {
                std::list<Effect> effects = augment.Rares().Effects();
                std::list<Effect>::iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffect(name, (*it));
                    ++it;
                }
            }
        }
        else
        {
            if (gear.SentientIntelligence().IsRareFiligree(si))
            {
                std::list<Effect> effects = augment.Rares().Effects();
                std::list<Effect>::iterator it = effects.begin();
                while (it != effects.end())
                {
                    NotifyItemEffect(name, (*it));
                    ++it;
                }
            }
        }
        // apply any filigree stances
        const std::list<Stance> & stances = augment.StanceData();
        std::list<Stance>::const_iterator sit = stances.begin();
        while (sit != stances.end())
        {
            NotifyNewStance((*sit));
            ++sit;
        }
        // apply any filigree set bonuses
        const std::list<std::string> & setBonuses = augment.SetBonus();
        std::list<std::string>::const_iterator sbit = setBonuses.begin();
        while (sbit != setBonuses.end())
        {
            ApplySetBonus((*sbit), name);
            ++sbit;
        }
    }
}

void Character::UpdateWeaponStances()
{
    EquippedGear gear = ActiveGearSet();
    // also depending of the items equipped in Inventory_Weapon1/2 decide
    // which of the combat stances is active
    // TWF, THF, SWF, Unarmed, Axe, Sword and Board, Staff, Orb, RuneArm, Swashbuckling
    Stance twf("Two Weapon Fighting", "", "");
    Stance thf("Two Handed Fighting", "", "");
    Stance thf2("Two Handed Fighting in Bear Form", "", "");
    Stance swf("Single Weapon Fighting", "", "");
    Stance unarmed("Unarmed", "", "");
    Stance axe("Axe", "", "");
    Stance sab("Sword and Board", "", "");
    Stance staff("Staff", "", "");
    Stance orb("Orb", "", "");
    Stance ra("Rune Arm", "", "");
    Stance swashbuckling("Swashbuckling", "", "");
    Stance thrown("Thrown Weapon", "", "");
    Stance emptyOffhand("Empty Offhand", "", "");
    Stance favoredWeapon("Favored Weapon", "", "");

    if (gear.HasItemInSlot(Inventory_Weapon1)
            && gear.HasItemInSlot(Inventory_Weapon2))
    {
        bool enableSwashbuckling = false;
        bool enableSwf = false;
        // 2 items equipped
        Item item1 = gear.ItemInSlot(Inventory_Weapon1);
        Item item2 = gear.ItemInSlot(Inventory_Weapon2);

        if (IsStanceActive("Favored Weapon", item1.Weapon()))
        {
            ActivateStance(favoredWeapon);
        }
        else
        {
            DeactivateStance(favoredWeapon);
        }
        // must be TWF or Sword and Board
        switch (item2.Weapon())
        {
        case Weapon_ShieldBuckler:
            if (IsEnhancementTrained("SBSwashbucklingStyle", "Skirmisher", TT_enhancement))
            {
                enableSwashbuckling = true;
                enableSwf = true;
            }
        case Weapon_ShieldSmall:
        case Weapon_ShieldLarge:
        case Weapon_ShieldTower:
            DeactivateStance(twf);
            ActivateStance(sab);
            break;
        case Weapon_Orb:
        case Weapon_RuneArm:
            enableSwf = true;
            DeactivateStance(twf);
            DeactivateStance(sab);
            break;
        default:
            ActivateStance(twf);
            DeactivateStance(sab);
            break;
        }
        DeactivateStance(thf);
        DeactivateStance(thf2);
        if (enableSwf)
        {
            ActivateStance(swf);
        }
        else
        {
            DeactivateStance(swf);
        }
        DeactivateStance(unarmed);
        DeactivateStance(staff);
        if (item2.Weapon() == Weapon_Orb)
        {
            ActivateStance(orb);
            if (IsEnhancementTrained("SBSwashbucklingStyle", "Arcane Marauder", TT_enhancement))
            {
                enableSwashbuckling = true;
            }
        }
        else
        {
            DeactivateStance(orb);
        }
        if (item2.Weapon() == Weapon_RuneArm)
        {
            ActivateStance(ra);
            if (IsEnhancementTrained("SBSwashbucklingStyle", "Cannoneer", TT_enhancement))
            {
                enableSwashbuckling = true;
            }
        }
        else
        {
            DeactivateStance(ra);
        }
        // swashbuckling also requires light or no armor
        if (IsStanceActive("Medium Armor")
                || IsStanceActive("Heavy Armor"))
        {
            enableSwashbuckling = false;
        }
        // swashbuckling also requires a finessable or thrown weapon
        if (!(IsThrownWeapon(item1.Weapon())
                || IsFinesseableWeapon(item1.Weapon())))
        {
            enableSwashbuckling = false;
        }
        if (enableSwashbuckling)
        {
            ActivateStance(swashbuckling);
        }
        else
        {
            DeactivateStance(swashbuckling);
        }
        if (IsAxe(item1.Weapon()))
        {
            ActivateStance(axe);
        }
        else
        {
            DeactivateStance(axe);
        }
        if (IsThrownWeapon(item1.Weapon()))
        {
            ActivateStance(thrown);
            DeactivateStance(twf);
        }
        else
        {
            DeactivateStance(thrown);
        }
        DeactivateStance(emptyOffhand);
    }
    else if (gear.HasItemInSlot(Inventory_Weapon1))
    {
        // 1 item equipped
        Item item1 = gear.ItemInSlot(Inventory_Weapon1);
        if (IsStanceActive("Favored Weapon", item1.Weapon()))
        {
            ActivateStance(favoredWeapon);
        }
        else
        {
            DeactivateStance(favoredWeapon);
        }
        switch (item1.Weapon())
        {
        case Weapon_Quarterstaff:
            ActivateStance(thf);
            ActivateStance(staff);
            DeactivateStance(thf2);
            DeactivateStance(swashbuckling);
            DeactivateStance(swf);
            break;
        case Weapon_GreatAxe:
        case Weapon_GreatClub:
        case Weapon_GreatSword:
        case Weapon_Maul:
        case Weapon_Falchion:
            ActivateStance(thf);
            DeactivateStance(thf2);
            DeactivateStance(staff);
            DeactivateStance(swashbuckling);
            DeactivateStance(swf);
            break;
        case Weapon_Dart:
        case Weapon_Shuriken:
        case Weapon_ThrowingAxe:
        case Weapon_ThrowingDagger:
        case Weapon_ThrowingHammer:
            DeactivateStance(thf);
            DeactivateStance(thf2);
            ActivateStance(swashbuckling);
            ActivateStance(swf);
            break;
        default:
            DeactivateStance(thf);
            DeactivateStance(thf2);
            if (item1.Weapon() != Weapon_Handwraps
                    && !IsRangedWeapon(item1.Weapon()))
            {
                ActivateStance(swashbuckling);
                ActivateStance(swf);
            }
            else
            {
                DeactivateStance(swashbuckling);
                DeactivateStance(swf);
            }
        }
        DeactivateStance(twf);
        DeactivateStance(sab);
        DeactivateStance(orb);
        DeactivateStance(ra);
        if (IsAxe(item1.Weapon()))
        {
            ActivateStance(axe);
        }
        else
        {
            DeactivateStance(axe);
        }
        if (item1.Weapon() == Weapon_Handwraps)
        {
            ActivateStance(unarmed);
        }
        else
        {
            DeactivateStance(unarmed);
        }
        if (IsThrownWeapon(item1.Weapon()))
        {
            ActivateStance(thrown);
            DeactivateStance(twf);
        }
        else
        {
            DeactivateStance(thrown);
        }
        ActivateStance(emptyOffhand);
    }
    else if (gear.HasItemInSlot(Inventory_Weapon2))
    {
        // 1 off hand item equipped
        Item item1 = gear.ItemInSlot(Inventory_Weapon2);
        DeactivateStance(favoredWeapon);
        DeactivateStance(thf);
        DeactivateStance(thf2);
        DeactivateStance(staff);
        DeactivateStance(swashbuckling);
        DeactivateStance(twf);
        DeactivateStance(unarmed);
        DeactivateStance(sab);
        if (item1.Weapon() == Weapon_Orb)
        {
            ActivateStance(orb);
        }
        else
        {
            DeactivateStance(orb);
        }
        if (item1.Weapon() == Weapon_RuneArm)
        {
            ActivateStance(ra);
        }
        else
        {
            DeactivateStance(ra);
        }
        DeactivateStance(thrown);
        ActivateStance(emptyOffhand);
    }
    else
    {
        // no items equipped
        DeactivateStance(favoredWeapon);
        DeactivateStance(twf);
        DeactivateStance(thf);
        DeactivateStance(thf2);
        DeactivateStance(swf);
        ActivateStance(unarmed);
        DeactivateStance(sab);
        DeactivateStance(staff);
        DeactivateStance(orb);
        DeactivateStance(ra);
        DeactivateStance(swashbuckling);
        DeactivateStance(thrown);
        ActivateStance(emptyOffhand);
    }
    if (IsStanceActive("Wolf")
            || IsStanceActive("Bear")
            || IsStanceActive("Winter Wolf")
            || IsStanceActive("Dire Bear")
            || IsStanceActive("Fire Elemental")
            || IsStanceActive("Water Elemental"))
    {
        // if they are in an animal form, then all specialised fighting
        // stances are disabled
        if (IsStanceActive("Two Handed Fighting")
                && (IsStanceActive("Bear") || IsStanceActive("Dire Bear")))
        {
            ActivateStance(thf2);
        }
        DeactivateStance(twf);
        DeactivateStance(thf);
        DeactivateStance(swf);
        ActivateStance(unarmed);
        DeactivateStance(staff);
        DeactivateStance(orb);
        DeactivateStance(ra);
        DeactivateStance(swashbuckling);
    }
    // finally activate the specific weapon type stances
    // add the auto controlled stances for each weapon type
    WeaponType wt1 = Weapon_Count;
    WeaponType wt2 = Weapon_Count;
    if (gear.HasItemInSlot(Inventory_Weapon1))
    {
        wt1 = gear.ItemInSlot(Inventory_Weapon1).Weapon();
    }
    if (gear.HasItemInSlot(Inventory_Weapon2))
    {
        wt2 = gear.ItemInSlot(Inventory_Weapon2).Weapon();
    }
    for (size_t wt = Weapon_Unknown; wt < Weapon_Count; ++wt)
    {
        CString name = (LPCTSTR)EnumEntryText((WeaponType)wt, weaponTypeMap);
        CString prompt;
        prompt.Format("You are wielding a %s", name);
        Stance weapon(
                (LPCTSTR)name,
                (LPCTSTR)name,
                (LPCTSTR)prompt);
        if (wt == wt1
                || wt == wt2)
        {
            ActivateStance(weapon);
        }
        else
        {
            DeactivateStance(weapon);
        }
    }
    for (size_t rt = Race_Unknown; rt < Race_Count; ++rt)
    {
        CString name = (LPCTSTR)EnumEntryText((RaceType)rt, raceTypeMap);
        CString prompt;
        prompt.Format("You are %s", name);
        Stance race(
                (LPCTSTR)name,
                (LPCTSTR)name,
                (LPCTSTR)prompt);
        if (rt == Race())
        {
            ActivateStance(race);
        }
        else
        {
            DeactivateStance(race);
        }
    }
}

void Character::UpdateArmorStances()
{
    EquippedGear gear = ActiveGearSet();
    // also depending of the item equipped in Inventory_Armor decide
    // which of the armor stances is active
    Stance cloth("Cloth Armor", "", "");
    Stance light("Light Armor", "", "");
    Stance medium("Medium Armor", "", "");
    Stance heavy("Heavy Armor", "", "");
    if (gear.HasItemInSlot(Inventory_Armor))
    {
        Item item1 = gear.ItemInSlot(Inventory_Armor);
        switch (item1.Armor())
        {
        case Armor_Cloth:
            ActivateStance(cloth);
            DeactivateStance(light);
            DeactivateStance(medium);
            DeactivateStance(heavy);
            break;
        case Armor_Light:
            DeactivateStance(cloth);
            ActivateStance(light);
            DeactivateStance(medium);
            DeactivateStance(heavy);
            break;
        case Armor_Medium:
            DeactivateStance(cloth);
            DeactivateStance(light);
            ActivateStance(medium);
            DeactivateStance(heavy);
            break;
        case Armor_Heavy:
            DeactivateStance(cloth);
            DeactivateStance(light);
            DeactivateStance(medium);
            ActivateStance(heavy);
            break;
        case Armor_Docent:
            // based on trained feats
            if (IsFeatTrained("Adamantine Body"))
            {
                DeactivateStance(cloth);
                DeactivateStance(light);
                DeactivateStance(medium);
                ActivateStance(heavy);
            }
            else if (IsFeatTrained("Mithral Body"))
            {
                // Mithral body is light armor
                DeactivateStance(cloth);
                ActivateStance(light);
                DeactivateStance(medium);
                DeactivateStance(heavy);
            }
            else // must be "Composite Plating"
            {
                // warforged no armor (No body feat)
                ActivateStance(cloth);
                DeactivateStance(light);
                DeactivateStance(medium);
                DeactivateStance(heavy);
            }
            break;
        }
    }
    else
    {
        // if we have no item and this toon is Warforged with "Adamantine Body"
        // then they are still in heavy armor mode
        if (IsFeatTrained("Adamantine Body"))
        {
            DeactivateStance(cloth);
            DeactivateStance(light);
            DeactivateStance(medium);
            ActivateStance(heavy);
        }
        else if (IsFeatTrained("Mithral Body"))
        {
            // Mithral body is light armor
            DeactivateStance(cloth);
            ActivateStance(light);
            DeactivateStance(medium);
            DeactivateStance(heavy);
        }
        else // must be "Composite Plating"
        {
            ActivateStance(cloth);
            DeactivateStance(light);
            DeactivateStance(medium);
            DeactivateStance(heavy);
        }
    }
}

void Character::UpdateShieldStances()
{
    Stance shield("Shield", "", "");
    Stance buckler("Buckler", "", "");
    Stance smallShield("Small Shield", "", "");
    Stance largeShield("Large Shield", "", "");
    Stance towerShield("Tower Shield", "", "");
    // assume deactivated at start
    DeactivateStance(shield);
    DeactivateStance(buckler);
    DeactivateStance(smallShield);
    DeactivateStance(largeShield);
    DeactivateStance(towerShield);

    EquippedGear gear = ActiveGearSet();
    if (gear.HasItemInSlot(Inventory_Weapon2))
    {
        // set the correct shield stance
        Item item1 = gear.ItemInSlot(Inventory_Weapon2);
        if (item1.Weapon() == Weapon_ShieldBuckler)
        {
            ActivateStance(shield);
            ActivateStance(buckler);
            DeactivateStance(smallShield);
            DeactivateStance(largeShield);
            DeactivateStance(towerShield);
        }
        else if (item1.Weapon() == Weapon_ShieldSmall)
        {
            ActivateStance(shield);
            DeactivateStance(buckler);
            ActivateStance(smallShield);
            DeactivateStance(largeShield);
            DeactivateStance(towerShield);
        }
        else if (item1.Weapon() == Weapon_ShieldLarge)
        {
            ActivateStance(shield);
            DeactivateStance(buckler);
            DeactivateStance(smallShield);
            ActivateStance(largeShield);
            DeactivateStance(towerShield);
        }
        else if (item1.Weapon() == Weapon_ShieldTower)
        {
            ActivateStance(shield);
            DeactivateStance(buckler);
            DeactivateStance(smallShield);
            DeactivateStance(largeShield);
            ActivateStance(towerShield);
        }
    }
}

void Character::UpdateCenteredStance()
{
    // to be centered the following must be true:
    // Must be in No/Cloth Armor
    // equipped weapons in main and off hand must be centering
    bool isCentered = true;
    if (!IsStanceActive("Cloth Armor"))
    {
        isCentered = false;
    }
    else
    {
        // now check any weapon breakdowns
        BreakdownItem * pBI = FindBreakdown(Breakdown_WeaponEffectHolder);
        if (pBI != NULL)
        {
            BreakdownItemWeaponEffects * pBIWE = dynamic_cast<BreakdownItemWeaponEffects*>(pBI);
            if (pBIWE != NULL)
            {
                if (!pBIWE->AreWeaponsCentering())
                {
                    isCentered = false;
                }
            }
        }
    }
    Stance centered("Centered", "", "");
    if (isCentered)
    {
        ActivateStance(centered);
    }
    else
    {
        DeactivateStance(centered);
    }
}

bool Character::LightWeaponInOffHand() const
{
    bool isLightWeapon = false;
    EquippedGear gear = ActiveGearSet();
    if (gear.HasItemInSlot(Inventory_Weapon2))
    {
        // need to see if this is a light weapon or not
        Item item = gear.ItemInSlot(Inventory_Weapon2);
        if (item.HasWeapon())
        {
            WeaponType wt = item.Weapon();
            // is a light weapon if is any of the following:
            switch (wt)
            {
            case Weapon_Dagger:
            case Weapon_HandAxe:
            case Weapon_Kukri:
            case Weapon_Shortsword:
            case Weapon_Sickle:
            case Weapon_LightHammer:
            case Weapon_LightMace:
            case Weapon_LightPick:
                isLightWeapon = true;
                break;
            case Weapon_Scimitar:
                // this is a light weapon if Tempest enhancement trained
                isLightWeapon = IsEnhancementTrained("TempestCore2", "", TT_enhancement);
                break;
            default:
                // all other weapon types are not a match
                break;
            }
        }
    }
    return isLightWeapon;
}

bool Character::IsFocusWeapon(WeaponType wt) const
{
    // first determine which focus group in the Kensei tree is selected
    bool isFocusWeapon = false;
    bool bExoticWeaponMastery = IsEnhancementTrained("KenseiExoticWeaponMastery", "", TT_enhancement);
    if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Archery", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Longbow
                || wt == Weapon_Shortbow);
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Axes", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_BattleAxe
                || wt == Weapon_GreatAxe
                || wt == Weapon_HandAxe
                || wt == Weapon_ThrowingAxe)
                || (Race() == Race_Dwarf && wt == Weapon_DwarvenAxe);
        // Axes: Adds dwarven axe if you are not a dwarf. (This has no effect if you are a dwarf.)
        if (bExoticWeaponMastery && wt == Weapon_DwarvenAxe)
        {
            isFocusWeapon = true;
        }
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Crossbows", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_HeavyCrossbow
                || wt == Weapon_LightCrossbow);
        // Crossbows: Adds great crossbows, heavy repeating crossbows, and light repeating crossbows.
        if (bExoticWeaponMastery)
        {
            if (wt == Weapon_GreatCrossbow
                    || wt == Weapon_RepeatingHeavyCrossbow
                    || wt == Weapon_RepeatingLightCrossbow)
            {
                isFocusWeapon = true;
            }
        }
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Druidic Weapons", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Club
                || wt == Weapon_Dagger
                || wt == Weapon_Dart
                || wt == Weapon_Quarterstaff
                || wt == Weapon_Scimitar
                || wt == Weapon_Sickle
                || wt == Weapon_Unarmed);
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Heavy Blades", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Falchion
                || wt == Weapon_GreatSword
                || wt == Weapon_Longsword
                || wt == Weapon_Scimitar);
        // Heavy Blades: Adds bastard swords and Khopesh.
        if (bExoticWeaponMastery)
        {
            if (wt == Weapon_BastardSword
                    || wt == Weapon_Khopesh)
            {
                isFocusWeapon = true;
            }
        }
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Light Blades", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Dagger
                || wt == Weapon_Kukri
                || wt == Weapon_Rapier
                || wt == Weapon_Shortsword
                || wt == Weapon_ThrowingDagger);
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Maces and Clubs", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Club
                || wt == Weapon_LightMace
                || wt == Weapon_HeavyMace
                || wt == Weapon_GreatClub
                || wt == Weapon_Morningstar
                || wt == Weapon_Quarterstaff);
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Martial Arts", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_Kama
                || wt == Weapon_Quarterstaff
                || wt == Weapon_Shuriken
                || wt == Weapon_Unarmed
                || wt == Weapon_Handwraps);
    }
    else if (IsEnhancementTrained("KenseiCore1", "Kensei Focus: Picks and Hammers", TT_enhancement))
    {
        isFocusWeapon = (wt == Weapon_LightPick
                || wt == Weapon_HeavyPick
                || wt == Weapon_LightHammer
                || wt == Weapon_Warhammer
                || wt == Weapon_Maul
                || wt == Weapon_ThrowingHammer);
    }
    return isFocusWeapon;
}

void Character::ApplyGuildBuffs()
{
    // we only apply or revoke the effects within the m_previousGuildLevel to
    // GuildLevel range
    if (HasApplyGuildBuffs())
    {
        if (m_previousGuildLevel != GuildLevel())
        {
            // there has been a change in guild level find the list of guild buffs
            // that have changed. Then either apply or revoke them depending
            // on the direction of level change
            bool revoke = (m_previousGuildLevel > GuildLevel());
            size_t minLevel = min(m_previousGuildLevel, GuildLevel());
            size_t maxLevel = max(m_previousGuildLevel, GuildLevel());
            std::list<GuildBuff> guildBuffs = GuildBuffs(); // all known guild buffs
            std::list<GuildBuff>::iterator it = guildBuffs.begin();
            while (it != guildBuffs.end())
            {
                // remove the buff if it is not in the required level range of change
                if ((*it).Level() <= minLevel
                        || (*it).Level() > maxLevel)
                {
                    // this buff is not in the range of those changed, exclude it
                    it = guildBuffs.erase(it);
                }
                else
                {
                    // on to the next buff to check
                    ++it;
                }
            }
            // now apply or revoke the buffs left in the list
            it = guildBuffs.begin();
            while (it != guildBuffs.end())
            {
                const std::list<Effect> & effects = (*it).Effects();
                std::list<Effect>::const_iterator eit = effects.begin();
                while (eit != effects.end())
                {
                    if (revoke)
                    {
                        NotifyItemEffectRevoked((*it).Name(), (*eit));
                    }
                    else
                    {
                        NotifyItemEffect((*it).Name(), (*eit));
                    }
                    ++eit;
                }
                ++it;
            }
            m_previousGuildLevel = GuildLevel();
        }
    }
    else
    {
        // guild buffs are no longer applied all, existing buffs need to be revoked
        size_t minLevel = 0;
        size_t maxLevel = GuildLevel();
        std::list<GuildBuff> guildBuffs = GuildBuffs(); // all known guild buffs
        std::list<GuildBuff>::iterator it = guildBuffs.begin();
        while (it != guildBuffs.end())
        {
            // remove the buff if it is not in the required level range of revoke
            if ((*it).Level() <= minLevel
                    || (*it).Level() > maxLevel)
            {
                // this buff is not in the range of those changed, exclude it
                it = guildBuffs.erase(it);
            }
            else
            {
                // on to the next buff to check
                ++it;
            }
        }
        // now revoke the buffs left in the list
        it = guildBuffs.begin();
        while (it != guildBuffs.end())
        {
            const std::list<Effect> & effects = (*it).Effects();
            std::list<Effect>::const_iterator eit = effects.begin();
            while (eit != effects.end())
            {
                NotifyItemEffectRevoked((*it).Name(), (*eit));
                ++eit;
            }
            ++it;
        }
    }
}

void Character::ToggleApplyGuildBuffs()
{
    SetValue_ApplyGuildBuffs(!HasApplyGuildBuffs());
    // apply changes
    ApplyGuildBuffs();
}

void Character::SetGuildLevel(size_t level)
{
    Set_GuildLevel(level);
    // apply changes
    ApplyGuildBuffs();
}

std::string Character::GetBuildDescription() const
{
    // determine how many levels of each class have been trained
    std::vector<size_t> classLevels = ClassLevels(MaxLevel());

    // levels in each non zero class listed, sorted by count then name
    // unknown listed also as shows how many need to be trained
    std::vector<ClassEntry> classes;
    for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
    {
        if (classLevels[ci] > 0)
        {
            // we have levels trained in this class, add it
            ClassEntry data;
            data.type = (ClassType)ci;
            data.levels = classLevels[ci];
            classes.push_back(data);
        }
    }
    // now that we have the full list, sort them into increasing order
    // now create the text to display
    std::stringstream ss;
    if (classes.size() > 0)
    {
        std::sort(classes.begin(), classes.end(), SortClassEntry);
        for (size_t ci = 0; ci < classes.size(); ++ci)
        {
            if (ci > 0)
            {
                // 2nd or following items separated by a ","
                ss << ", ";
            }
            ss << classes[ci].levels
                    << " "
                    << (LPCSTR)EnumEntryText(classes[ci].type, classTypeMap);
        }
        // e.g. "10 Epic, 8 Fighter, 6 Monk, 6 Ranger"
    }
    return ss.str();
}

void Character::AutoTrainSingleSelectionFeats()
{
    bool featsTrained = false;      // set to true if need to display auto trained feats
    std::stringstream ss;
    ss << "The following feats at the indicated levels were automatically\n"
            "trained for you as there is only a single selection available.\n"
            "\n";
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        std::vector<TrainableFeatTypes> tfts = TrainableFeatTypeAtLevel(level);
        for (size_t i = 0; i < tfts.size(); ++i)
        {
            // only check for certain trainable feat types
            bool check = false;
            switch (tfts[i])
            {
            case TFT_BelovedOf:
            case TFT_BlightCasterToggleImbue:
            case TFT_ChildOf:
            case TFT_DamageReduction:
            case TFT_Deity:
            case TFT_DruidWildShape:
            case TFT_DomainFeat:
            case TFT_KinForm:
            case TFT_TruePact:
            case TFT_WarlockPactAbility:
            case TFT_WarlockPactSaveBonus:
            case TFT_WarlockPactSpell:
            case TFT_WarlockResistance:
                check = true;
                break;
            default:
                // don't check for these feat types
                break;
            }
            if (check)
            {
                // must not have a feat trained in this feat slot to do an auto train
                const LevelTraining & lt = LevelData(level);
                const FeatsListObject & tf = lt.TrainedFeats();
                if (tf.FeatName(tfts[i]).empty())
                {
                    std::vector<Feat> availableFeats = TrainableFeats(
                            tfts[i],
                            level,
                            "");
                    if (availableFeats.size() == 1)
                    {
                        ss << "Level " << (level + 1) << ": ";
                        std::string label = TrainableFeatTypeLabel(tfts[i]);
                        label += ": ";
                        ss << label << ": ";
                        ss << availableFeats[0].Name();
                        ss << "\n";
                        TrainFeat(availableFeats[0].Name(), tfts[i], level, true);
                        featsTrained = true;
                    }
                }
            }
        }
    }
    if (featsTrained)
    {
        AfxMessageBox(ss.str().c_str(), MB_ICONEXCLAMATION);
    }
}

bool Character::NotPresentInEarlierLevel(size_t level, TrainableFeatTypes type) const
{
    // return true if the given feat type does not occur for any of the previous
    // levels.
    bool notPresent = true;
    for (size_t li = 0; li < level; ++li)
    {
        std::vector<TrainableFeatTypes> tfts = Character::TrainableFeatTypeAtLevel(li);
        for (size_t i = 0; i < tfts.size(); ++i)
        {
            if (tfts[i] == type)
            {
                notPresent = false;
            }
        }
    }
    return notPresent;
}

void Character::ApplySpellEffects(const std::string & spellName, size_t castingLevel)
{
    // spells use the same interface for effects as items
    Spell spell = FindSpellByName(spellName);
    // apply any spell stances also
    std::list<Stance> stances = spell.StanceData();
    std::list<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyNewStance((*sit));
        ++sit;
    }
    // if we have any DC objects notify about them
    const std::list<DC> & dcs = spell.EffectDC();
    std::list<DC>::const_iterator dcit = dcs.begin();
    while (dcit != dcs.end())
    {
        NotifyNewDC((*dcit));
        ++dcit;
    }
    // and now the effects
    std::vector<Effect> effects = spell.UpdatedEffects(castingLevel);
    std::vector<Effect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        std::string name = "Spell: " + spell.Name();
        NotifyItemEffect(name, (*it));
        ++it;
    }
}

void Character::RevokeSpellEffects(const std::string & spellName, size_t castingLevel)
{
    // spells use the same interface for effects as items
    Spell spell = FindSpellByName(spellName);
    // apply any spell stances also
    std::list<Stance> stances = spell.StanceData();
    std::list<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyRevokeStance((*sit));
        ++sit;
    }
    // if we have any DC objects notify about them
    const std::list<DC> & dcs = spell.EffectDC();
    std::list<DC>::const_iterator dcit = dcs.begin();
    while (dcit != dcs.end())
    {
        NotifyRevokeDC((*dcit));
        ++dcit;
    }
    // and now the effects
    std::vector<Effect> effects = spell.UpdatedEffects(castingLevel);
    std::vector<Effect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        std::string name = "Spell: " + spell.Name();
        NotifyItemEffectRevoked(name, (*it));
        ++it;
    }
}

// CharacterObserver
void Character::UpdateFeatEffect(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantFeat)
    {
        AddGrantedFeat(effect.Feat());
    }
    if (effect.Type() == Effect_SpellListAddition)
    {
        AddSpellListAddition(effect);
    }
}

void Character::UpdateFeatEffectRevoked(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantFeat)
    {
        RevokeGrantedFeat(effect.Feat());
    }
    if (effect.Type() == Effect_SpellListAddition)
    {
        RevokeSpellListAddition(effect);
    }
}

void Character::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_GrantFeat)
    {
        AddGrantedFeat(effect.m_effect.Feat());
    }
    if (effect.m_effect.Type() == Effect_SpellListAddition)
    {
        AddSpellListAddition(effect.m_effect);
    }
}

void Character::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_GrantFeat)
    {
        RevokeGrantedFeat(effect.m_effect.Feat());
    }
    if (effect.m_effect.Type() == Effect_SpellListAddition)
    {
        RevokeSpellListAddition(effect.m_effect);
    }
}

void Character::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantFeat)
    {
        AddGrantedFeat(effect.Feat());
    }
    if (effect.Type() == Effect_SpellListAddition)
    {
        AddSpellListAddition(effect);
    }
}

void Character::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantFeat)
    {
        RevokeGrantedFeat(effect.Feat());
    }
    if (effect.Type() == Effect_SpellListAddition)
    {
        RevokeSpellListAddition(effect);
    }
}

void Character::AddGrantedFeat(const std::string & featName)
{
    bool alreadyInList = false;
    std::list<TrainedFeat>::iterator it = m_grantedFeats.begin();
    while (!alreadyInList && it != m_grantedFeats.end())
    {
        if ((*it).FeatName() == featName)
        {
            // increment count
            alreadyInList = true;
            (*it).Set_LevelTrainedAt((*it).LevelTrainedAt() + 1);
        }
        ++it;
    }
    if (!alreadyInList)
    {
        // add the granted feat to the list
        TrainedFeat tf;
        tf.Set_FeatName(featName);
        tf.Set_Type(TFT_GrantedFeat);
        tf.Set_LevelTrainedAt(1);
        m_grantedFeats.push_back(tf);
        NotifyGrantedFeatsChanged();

        // now determine whether this granted feat is already trained
        // if it is, we do not notify its effects. If its not, we do
        // we need to record the notify state for this item so that
        // if the user trains this feat in some other way we have to remove
        // the effects for the granted feat
        if (!IsFeatTrained(featName))
        {
            // this feat is not currently trained, we need to notify about its
            // effects
            const Feat & feat = FindFeat(featName);
            ApplyFeatEffects(feat);
            m_grantedNotifyState.push_back(true);
        }
        else
        {
            // we have not notified about this feats effects as it
            // is already trained through some other mechanism (either selected
            // or an automatic feat)
            m_grantedNotifyState.push_back(false);
        }
    }
}

void Character::RevokeGrantedFeat(const std::string & featName)
{
    // remove the granted feat from the list
    ASSERT(m_grantedFeats.size() == m_grantedNotifyState.size());
    std::list<TrainedFeat>::iterator it = m_grantedFeats.begin();
    std::list<bool>::iterator bit = m_grantedNotifyState.begin();
    bool changed = false;
    while (it != m_grantedFeats.end())
    {
        if ((*it).FeatName() == featName)
        {
            if ((*it).LevelTrainedAt() == 1)
            {
                bool notified = (*bit);
                // remove this copy of the feat (it may be present multiple times)
                it = m_grantedFeats.erase(it);
                bit = m_grantedNotifyState.erase(bit);
                if (notified)
                {
                    const Feat & feat = FindFeat(featName);
                    RevokeFeatEffects(feat);
                }
                changed = true;
            }
            else
            {
                // decrement the assigned count
                (*it).Set_LevelTrainedAt((*it).LevelTrainedAt() - 1);
            }
            break;  // and were done, no need to check the rest
        }
        ++it;
        ++bit;
    }
    if (changed)
    {
        NotifyGrantedFeatsChanged();
    }
}

void Character::KeepGrantedFeatsUpToDate()
{
    ASSERT(m_grantedFeats.size() == m_grantedNotifyState.size());
    std::list<TrainedFeat>::iterator it = m_grantedFeats.begin();
    std::list<bool>::iterator bit = m_grantedNotifyState.begin();
    while (it != m_grantedFeats.end())
    {
        if (!IsFeatTrained((*it).FeatName()))
        {
            // if the feat is not trained and we have not notified its effects
            // then apply them now
            if (!(*bit))
            {
                const Feat & feat = FindFeat((*it).FeatName());
                ApplyFeatEffects(feat);
                (*bit) = true;  // it is now notified
            }
        }
        else
        {
            // if the feat is trained and we are currently notified then
            // revoke its effects
            if ((*bit))
            {
                const Feat & feat = FindFeat((*it).FeatName());
                RevokeFeatEffects(feat);
                (*bit) = false;  // no longer notified
            }
        }
        ++it;
        ++bit;
    }
}

void Character::UpdateSkillPoints()
{
    // update the skill points for all levels
    ASSERT(m_Levels.size() == MaxLevel());
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        size_t available = SkillPoints(
                (*it).HasClass() ? (*it).Class() : Class_Unknown,
                Race(),
                AbilityAtLevel(Ability_Intelligence, level, false),
                level);
        (*it).Set_SkillPointsAvailable(available);
        ++it;
    }
}

void Character::UpdateSkillPoints(size_t level)
{
    // update the skill points for this specific level
    ASSERT(m_Levels.size() == MaxLevel());
    if (level < MAX_CLASS_LEVELS)
    {
        // only have skill points for heroic levels
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        std::advance(it, level);
        size_t available = SkillPoints(
                (*it).HasClass() ? (*it).Class() : Class_Unknown,
                Race(),
                AbilityAtLevel(Ability_Intelligence, level, false),
                level);
        (*it).Set_SkillPointsAvailable(available);
    }
}

void Character::VerifyGear()
{
    EquippedGear gear = ActiveGearSet();
    CString text("The following equipped items were removed as the\r\n"
            "requirements for use are no longer met:\r\n");
    bool revokeOccurred = false;
    // need to know how many levels and of what classes they have trained
    std::vector<size_t> classLevels = ClassLevels(MaxLevel());
    // need to know which feats have already been trained by this point
    // include any feats also trained at the current level
    std::list<TrainedFeat> currentFeats = CurrentFeats(MaxLevel());
    // check every item
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (gear.HasItemInSlot((InventorySlotType)i))
        {
            Item item = gear.ItemInSlot((InventorySlotType)i);
            if (gear.IsSlotRestricted((InventorySlotType)i, this))
            {
                revokeOccurred = true;
                ClearGearInSlot(gear.Name(), (InventorySlotType)i);
                CString itemName;
                itemName = EnumEntryText((InventorySlotType)i, InventorySlotTypeMap);
                itemName += ": ";
                itemName += item.Name().c_str();
                itemName += "\r\n";
                text += itemName;
            }
            else
            {
                if (item.HasRequirementsToUse())
                {
                    if (!item.RequirementsToUse().Met(*this, classLevels, MaxLevel(), currentFeats, true))
                    {
                        revokeOccurred = true;
                        ClearGearInSlot(gear.Name(), (InventorySlotType)i);
                        CString itemName;
                        itemName = EnumEntryText((InventorySlotType)i, InventorySlotTypeMap);
                        itemName += ": ";
                        itemName += item.Name().c_str();
                        itemName += "\r\n";
                        text += itemName;
                    }
                }
            }
        }
    }
    if (revokeOccurred)
    {
        AfxMessageBox(text, MB_OK);
        UpdateGreensteelStances();      // only on gear changes
    }
}

void Character::UpdateGreensteelStances()
{
    // update which of the "Dominion", "Escalation" or "Opposition"
    // stances are active when Greensteel is equipped
    Stance dominion("Dominion", "Dominion", "Dominion");
    Stance escalation("Escalation", "Escalation", "Escalation");
    Stance opposition("Opposition", "Opposition", "Opposition");
    Stance ethereal("Ethereal", "Ethereal", "Ethereal");
    Stance material("Material", "Material", "Material");
    // active stance is based on:
    // Must have at least 2 Greensteel items equipped
    EquippedGear gear = ActiveGearSet();
    size_t greensteelItemCount = 0;
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        // only equipment items count towards Greensteel set bonuses
        if (gear.HasItemInSlot((InventorySlotType)i)
                && i != Inventory_Weapon1
                && i != Inventory_Weapon2)
        {
            Item item = gear.ItemInSlot((InventorySlotType)i);
            if (item.HasIsGreensteel())
            {
                ++greensteelItemCount;
            }
        }
    }
    if (greensteelItemCount >= 2)
    {
        // Must have a dominant stance to be enabled
        // count can be found by seeing how many stacks a given stance has
        size_t dominionCount = StanceStackCount("Dominion");
        size_t escalationCount = StanceStackCount("Escalation");
        size_t oppositionCount = StanceStackCount("Opposition");
        size_t etherealCount = StanceStackCount("Ethereal");
        size_t materialCount = StanceStackCount("Material");
        if (dominionCount > max(escalationCount, oppositionCount))
        {
            ActivateStance(dominion);
        }
        else
        {
            DeactivateStance(dominion);
        }
        if (escalationCount > max(dominionCount, oppositionCount))
        {
            ActivateStance(escalation);
        }
        else
        {
            DeactivateStance(escalation);
        }
        if (oppositionCount > max(dominionCount, escalationCount))
        {
            if (dominionCount == escalationCount)
            {
                ActivateStance(opposition);
            }
            else
            {
                // Opposition dominance only occurs when ALL augments are "Opposition"
                // any other type present and the bonus does not apply
                DeactivateStance(opposition);
            }
        }
        else
        {
            DeactivateStance(opposition);
        }
        // requires 4 or more item for material/ethereal dominance set bonus
        if (greensteelItemCount >= 4)
        {
            // its either material or ethereal dominance
            if (etherealCount > 0 && etherealCount > materialCount)
            {
                ActivateStance(ethereal);
            }
            else
            {
                DeactivateStance(ethereal);
            }
            if (materialCount > 0 && materialCount > etherealCount)
            {
                ActivateStance(material);
            }
            else
            {
                DeactivateStance(material);
            }
        }
        else
        {
            DeactivateStance(ethereal);
            DeactivateStance(material);
        }
    }
    else
    {
        // one or less items equipped, Greensteel set bonus's disabled
        DeactivateStance(dominion);
        DeactivateStance(escalation);
        DeactivateStance(opposition);
        DeactivateStance(ethereal);
        DeactivateStance(material);
    }
}

void Character::UpdateGearToLatestVersions()
{
    // needs to be done for all gear sets and all items
    std::list<EquippedGear>::iterator it = m_GearSetups.begin();
    while (it != m_GearSetups.end())
    {
        // update all the items
        for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
        {
            if ((*it).HasItemInSlot((InventorySlotType)i))
            {
                Item latestVersion = GetLatestVersionOfItem((*it).ItemInSlot((InventorySlotType)i));
                (*it).SetItem((InventorySlotType)i, this, latestVersion, false);
            }
        }
        ++it;
    }
}

Item Character::GetLatestVersionOfItem(const Item & original)
{
    Item newVersion = original; // assume unchanged
    const Item & foundItem = FindItem(original.Name());
    // no name if not found
    if (foundItem.Name() == original.Name())
    {
        // this is the item we want to copy
        newVersion = foundItem;
        // now copy across specific fields from the source item
        newVersion.CopyUserSetValues(original);
        // make sure all the selected augments are valid
        std::vector<ItemAugment> augments = original.Augments();
        for (size_t i = 0; i < augments.size(); ++i)
        {
            if (augments[i].HasSelectedAugment()
                    && augments[i].SelectedAugment() != "")
            {
                const Augment & augment = FindAugmentByName(augments[i].SelectedAugment());
                if (augment.Name() == "")
                {
                    // if we failed to find the augment, its a bad one. Just clear it
                    augments[i].Clear_SelectedAugment();
                    augments[i].Clear_Value();
                    augments[i].Clear_Value2();
                }
            }
        }
        newVersion.Set_Augments(augments);
    }
    return newVersion;
}

// self and party buffs
const std::list<std::string> Character::EnabledSelfAndPartyBuffs() const
{
    return SelfAndPartyBuffs();
}

void Character::EnableSelfAndPartyBuff(const std::string & name)
{
    // add it to the list of enabled buffs
    m_SelfAndPartyBuffs.push_front(name);
    // find the buff and notify its effects
    OptionalBuff buff = FindOptionalBuff(name);
    // get the list of effects this OptionalBuff has
    const std::vector<Effect> & effects = buff.Effects();
    std::vector<Effect>::const_iterator feit = effects.begin();
    while (feit != effects.end())
    {
        NotifyItemEffect(name, (*feit));
        ++feit;
    }
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::DisableSelfAndPartyBuff(const std::string & name)
{
    // remove it from the list of enabled buffs
    bool found = false;
    std::list<std::string>::iterator it = m_SelfAndPartyBuffs.begin();
    while (!found && it != m_SelfAndPartyBuffs.end())
    {
        if ((*it) == name)
        {
            // this is the one to remove
            found = true;
            it = m_SelfAndPartyBuffs.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // find the buff and clear its effects
    OptionalBuff buff = FindOptionalBuff(name);
    // get the list of effects this OptionalBuff has
    const std::vector<Effect> & effects = buff.Effects();
    std::vector<Effect>::const_iterator feit = effects.begin();
    while (feit != effects.end())
    {
        NotifyItemEffectRevoked(name, (*feit));
        ++feit;
    }
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::NotifyAllSelfAndPartyBuffs()
{
    // character has just become active. Notify about all the self buff
    // effects it has selected
    std::list<std::string>::iterator it = m_SelfAndPartyBuffs.begin();
    while (it != m_SelfAndPartyBuffs.end())
    {
        OptionalBuff buff = FindOptionalBuff((*it));
        const std::vector<Effect> & effects = buff.Effects();
        std::vector<Effect>::const_iterator feit = effects.begin();
        while (feit != effects.end())
        {
            NotifyItemEffect((*it), (*feit));
            ++feit;
        }
        ++it;
    }
}

// Notes support
void Character::SetNotes(const std::string & notes)
{
    Set_Notes(notes);
    m_pDocument->SetModifiedFlag(TRUE);
}

bool Character::HasUnique(const std::string& name) const
{
    bool bHas = false;
    auto it = m_uniqueSelections.begin();
    while (it != m_uniqueSelections.end())
    {
        if ((*it).first == name)
        {
            bHas = true;
        }
        ++it;
    }
    return bHas;
}

bool Character::UniqueIs(const std::string& name, const std::string& value) const
{
    bool bIs = false;
    auto it = m_uniqueSelections.begin();
    while (it != m_uniqueSelections.end())
    {
        if ((*it).first == name
                && (*it).second == value)
        {
            bIs = true;
        }
        ++it;
    }
    return bIs;
}

void Character::ResetBuild()
{
    // create a new blank character
    Character blankCharacter;
    blankCharacter.m_pDocument = m_pDocument;
    // copy across what we want to keep
    blankCharacter.m_Name = m_Name;
    blankCharacter.m_GuildLevel = m_GuildLevel;
    blankCharacter.m_hasApplyGuildBuffs = m_hasApplyGuildBuffs;
    blankCharacter.m_StrTome = m_StrTome;
    blankCharacter.m_DexTome = m_DexTome;
    blankCharacter.m_ConTome = m_ConTome;
    blankCharacter.m_IntTome = m_IntTome;
    blankCharacter.m_WisTome = m_WisTome;
    blankCharacter.m_ChaTome = m_ChaTome;
    blankCharacter.m_Tomes = m_Tomes;
    blankCharacter.m_SpecialFeats = m_SpecialFeats;
    blankCharacter.m_ReaperTreeSpend = m_ReaperTreeSpend;
    blankCharacter.m_GearSetups = m_GearSetups;
    blankCharacter.m_ActiveGear = m_ActiveGear;
    blankCharacter.m_SelfAndPartyBuffs = m_SelfAndPartyBuffs;
    // now assign to ourselves
    (*this) = blankCharacter;
    JustLoaded();
}

bool Character::ShowUnavailable() const
{
    return m_bShowUnavailableFeats;
}

bool Character::ShowEpicOnly() const
{
    return m_bShowEpicOnly;
}

void Character::ToggleShowEpicOnly()
{
    m_bShowEpicOnly = !m_bShowEpicOnly;
}

void Character::ToggleShowUnavailable()
{
    m_bShowUnavailableFeats = !m_bShowUnavailableFeats;
}

bool Character::ShowIgnoredItems() const
{
    return m_bShowIgnoredItems;
}

void Character::ToggleShowIgnoredItems()
{
    m_bShowIgnoredItems = !m_bShowIgnoredItems;
}

void Character::AddToIgnoreList(const std::string & name)
{
    ::AddToIgnoreList(name);
}

void Character::RemoveFromIgnoreList(const std::string & name)
{
    ::RemoveFromIgnoreList(name);
}

bool Character::IsInIgnoreList(const std::string & name) const
{
    return ::IsInIgnoreList(name);
}

void Character::ApplySetBonus(
        const std::string & set,
        const std::string & name)
{
    const SetBonus & setObject = FindSetBonus(set);
    // sets effects
    std::vector<Effect> effects = setObject.Effects();
    std::vector<Effect>::iterator it = effects.begin();
    while (it != effects.end())
    {
        NotifyItemEffect(name, (*it));
        ++it;
    }
    // set bonuses always have a stance object, create it
    Stance setStance(setObject.Name(), setObject.Icon(), setObject.Description());
    setStance.Set_SetBonus();
    NotifyNewStance(setStance);

    // they may also have regular stance objects
    const std::vector<Stance> & stances = setObject.Stances();
    std::vector<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyNewStance((*sit));
        ++sit;
    }
}

void Character::RevokeSetBonus(
        const std::string & set,
        const std::string & name)
{
    const SetBonus & setObject = FindSetBonus(set);
    // sets effects
    std::vector<Effect> effects = setObject.Effects();
    std::vector<Effect>::iterator it = effects.begin();
    while (it != effects.end())
    {
        NotifyItemEffectRevoked(name, (*it));
        ++it;
    }
    // set bonuses always have a stance object, create it
    Stance setStance(setObject.Name(), setObject.Icon(), setObject.Description());
    setStance.Set_SetBonus();
    NotifyRevokeStance(setStance);

    // they may also have regular stance objects
    const std::vector<Stance> & stances = setObject.Stances();
    std::vector<Stance>::const_iterator sit = stances.begin();
    while (sit != stances.end())
    {
        NotifyRevokeStance((*sit));
        ++sit;
    }
}

AlignmentType Character::OverrideAlignment() const
{
    AlignmentType a = Alignment(); // assume
    if (IsEnhancementTrained("PalShadCore4", "", TT_enhancement))
    {
        a = Alignment_NeutralEvil;
    }
    else if (IsEnhancementTrained("PalShadCore1", "", TT_enhancement))
    {
        a = Alignment_TrueNeutral;
    }
    return a;
}

void Character::U51Destiny_SetSelectedTrees(const SelectedDestinyTrees & trees)
{
    Set_DestinyTrees(trees);
}

void Character::U51Destiny_TrainEnhancement(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        const std::vector<size_t>& cost)
{
    // Find the tree tracking amount spent and enhancements trained
    DestinySpendInTree * pItem = U51Destiny_FindTree(treeName);
    const EnhancementTree & eTree = GetEnhancementTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        DestinySpendInTree tree;
        tree.Set_TreeName(treeName);
        tree.Set_TreeVersion(eTree.Version());  // need to track version of tree spent in
        m_DestinyTreeSpend.push_back(tree);
        pItem = &m_DestinyTreeSpend.back();
    }
    size_t ranks = 0;
    const EnhancementTreeItem * pTreeItem = FindEnhancement(enhancementName);
    int spent = pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            pTreeItem->MinSpent(selection),
            false,
            &ranks);
    m_destinyTreeSpend += spent;
    ASSERT(pTreeItem != NULL);
    if (HasU51Destiny_Tier5Tree() && pTreeItem->HasTier5())
    {
        ASSERT(U51Destiny_Tier5Tree() == treeName);    // should only be able to train other tier 5's in the same tree
    }
    else
    {
        // no tier 5 enhancement yet selected
        if (pTreeItem->HasTier5())
        {
            // this is a tier 5, lockout all other tree's tier 5 enhancements
            Set_U51Destiny_Tier5Tree(treeName);
        }
    }
    // track whether this is a tier 5 enhancement
    // now notify all and sundry about the enhancement effects
    ApplyEnhancementEffects(treeName, enhancementName, selection, 1);
    NotifyEnhancementTrained(enhancementName, selection, pTreeItem->HasTier5(), true);
    NotifyActionPointsChanged();
    NotifyAPSpentInTreeChanged(treeName);
    UpdateWeaponStances();
    SetAlignmentStances();
    UpdateCenteredStance();
}

void Character::U51Destiny_RevokeEnhancement(
        const std::string& treeName,
        const std::string& revokedEnhancement)
{
    std::string revokedEnhancementCopy(revokedEnhancement);
    DestinySpendInTree * pItem = U51Destiny_FindTree(treeName);
    if (pItem != NULL
            && pItem->Enhancements().size() > 0)
    {
        // return points available to spend also
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        int spent = pItem->RevokeEnhancement(
                revokedEnhancementCopy,
                &revokedEnhancementSelection,
                &ranks);
        m_destinyTreeSpend -= spent;
        RevokeEnhancementEffects(treeName, revokedEnhancementCopy, revokedEnhancementSelection, 1);
        // determine whether we still have a tier 5 enhancement trained if the tree just had one
        // revoked in it
        if (HasU51Destiny_Tier5Tree() && U51Destiny_Tier5Tree() == treeName)
        {
            // may have lost the tier 5 status, check the tree to see if any tier 5 are still trained
            if (!pItem->HasTier5())
            {
                Clear_U51Destiny_Tier5Tree();  // no longer a tier 5 trained
            }
        }
        NotifyEnhancementRevoked(revokedEnhancementCopy, revokedEnhancementSelection, false, true);
        NotifyActionPointsChanged();
        NotifyAPSpentInTreeChanged(treeName);
        UpdateWeaponStances();
        SetAlignmentStances();
        VerifyGear();
        UpdateCenteredStance();
    }
}

void Character::U51Destiny_ResetEnhancementTree(std::string treeName)
{
    // a whole tree is being reset
    DestinySpendInTree * pItem = U51Destiny_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            std::string itemName = pItem->Enhancements().back().EnhancementName();
            U51Destiny_RevokeEnhancement(treeName, itemName);
            pItem = U51Destiny_FindTree(treeName);
        }
        // now remove the tree entry from the list (not present if no spend in tree)
        std::list<DestinySpendInTree>::iterator it = m_DestinyTreeSpend.begin();
        while (it != m_DestinyTreeSpend.end())
        {
            if ((*it).TreeName() == treeName)
            {
                // done once we find it
                m_DestinyTreeSpend.erase(it);
                break;
            }
            ++it;
        }
        NotifyEnhancementTreeReset();
        NotifyActionPointsChanged();
        NotifyAPSpentInTreeChanged(treeName);
        UpdateWeaponStances();
        UpdateCenteredStance();
    }
}

DestinySpendInTree * Character::U51Destiny_FindTree(const std::string & treeName)
{
    // Find the tree tracking amount spent and enhancements trained
    DestinySpendInTree * pItem = NULL;
    std::list<DestinySpendInTree>::iterator it = m_DestinyTreeSpend.begin();
    while (pItem == NULL
            && it != m_DestinyTreeSpend.end())
    {
        if ((*it).TreeName() == treeName)
        {
            pItem = &(*it);
            break;
        }
        ++it;
    }
    return pItem;
}

void Character::U51Destiny_SwapTrees(const std::string & tree1, const std::string & tree2)
{
    m_DestinyTrees.SwapTrees(tree1, tree2);
    NotifyEnhancementTreeOrderChanged();
}

void Character::SetLamanniaMode(bool bLamanniaPreview)
{
    m_hasLamanniaMode = bLamanniaPreview;
    // add or remove required objects
    if (m_hasLamanniaMode)
    {
        // need to add new additional LevelTraining objects
        while (m_Levels.size() < MaxLevel())
        {
            LevelTraining lt;
            if (m_Levels.size() < MAX_CLASS_LEVELS + MAX_EPIC_LEVELS)
            {
                // need to add epic
                lt.Set_Class(Class_Epic);
            }
            else
            {
                // need to add legendary
                lt.Set_Class(Class_Legendary);
            }
            m_Levels.push_back(lt);
        }
    }
    else
    {
        // need to remove the additional LevelTraining objects
        while (m_Levels.size() > MaxLevel())
        {
            // now clear the object
            // note that feat revokation etc is handled by or swap too/from the character
            // in the main view
            m_Levels.pop_back();
        }
    }
    // changing too/from Lamannia mode changes the feats available
    UpdateFeats();
    VerifyTrainedFeats();
    NotifyRaceChanged(Race());  // just get stuff to update
    m_pDocument->SetModifiedFlag(TRUE);
}

size_t Character::MaxLevel() const
{
    size_t level = MAX_NORMAL_LEVEL;
    if (m_hasLamanniaMode)
    {
        level = MAX_LAMANNIA_LEVEL;
    }
    return level;
}

const SpendInTree* Character::FindSpendInTree(const std::string& treeName) const
{
    const SpendInTree* pTree = NULL;

    std::list<EnhancementSpendInTree>::const_iterator eit = m_EnhancementTreeSpend.begin();
    while (pTree == NULL
        && eit != m_EnhancementTreeSpend.end())
    {
        if ((*eit).TreeName() == treeName)
        {
            pTree = &(*eit);
            break;
        }
        ++eit;
    }
    if (pTree == NULL)
    {
        std::list<ReaperSpendInTree>::const_iterator rit = m_ReaperTreeSpend.begin();
        while (pTree == NULL
            && rit != m_ReaperTreeSpend.end())
        {
            if ((*rit).TreeName() == treeName)
            {
                pTree = &(*rit);
                break;
            }
            ++rit;
        }
    }
    if (pTree == NULL)
    {
        std::list<DestinySpendInTree>::const_iterator dit = m_DestinyTreeSpend.begin();
        while (pTree == NULL
            && dit != m_DestinyTreeSpend.end())
        {
            if ((*dit).TreeName() == treeName)
            {
                pTree = &(*dit);
                break;
            }
            ++dit;
        }
    }
    return pTree;
}

SpendInTree* Character::FindSpendInTree(const std::string& treeName)
{
    SpendInTree* pTree = NULL;

    std::list<EnhancementSpendInTree>::iterator eit = m_EnhancementTreeSpend.begin();
    while (pTree == NULL
        && eit != m_EnhancementTreeSpend.end())
    {
        if ((*eit).TreeName() == treeName)
        {
            pTree = &(*eit);
            break;
        }
        ++eit;
    }
    if (pTree == NULL)
    {
        std::list<ReaperSpendInTree>::iterator rit = m_ReaperTreeSpend.begin();
        while (pTree == NULL
            && rit != m_ReaperTreeSpend.end())
        {
            if ((*rit).TreeName() == treeName)
            {
                pTree = &(*rit);
                break;
            }
            ++rit;
        }
    }
    if (pTree == NULL)
    {
        std::list<DestinySpendInTree>::iterator dit = m_DestinyTreeSpend.begin();
        while (pTree == NULL
            && dit != m_DestinyTreeSpend.end())
        {
            if ((*dit).TreeName() == treeName)
            {
                pTree = &(*dit);
                break;
            }
            ++dit;
        }
    }
    return pTree;
}

void Character::AddSpellListAddition(const Effect& effect)
{
    // check to make sure it is not already present
    if (!IsSpellInSpellListAdditionList(effect.Class(), effect.SpellLevel(), effect.Spell()))
    {
        SpellListAddition sla(effect.Class(), effect.SpellLevel(), effect.Spell());
        m_additionalSpells.push_back(sla);
    }
    else
    {
        // its already present, add to its count
        for (size_t i = 0; i < m_additionalSpells.size(); ++i)
        {
            if (m_additionalSpells[i].AddsToSpellList(effect.Class(), effect.SpellLevel())
                    && m_additionalSpells[i].SpellName() == effect.Spell())
            {
                m_additionalSpells[i].AddReference();
            }
        }
    }
}

void Character::RevokeSpellListAddition(const Effect& effect)
{
    bool bUpdate = false;
    // if its present remove a stack and revoke if now all gone
    for (size_t i = 0; i < m_additionalSpells.size(); ++i)
    {
        if (m_additionalSpells[i].AddsToSpellList(effect.Class(), effect.SpellLevel())
                && m_additionalSpells[i].SpellName() == effect.Spell())
        {
            bool erase = m_additionalSpells[i].RemoveReference();
            if (erase)
            {
                m_additionalSpells.erase(m_additionalSpells.begin() + i);
                --i;        // keep index right
                bUpdate = true;
            }
        }
    }
    if (bUpdate)
    {
        // make sure trained spell lists are correct on spell list changes
        UpdateSpells();
    }
}

bool Character::IsSpellInSpellListAdditionList(
        ClassType ct,
        size_t spellLevel,
        const std::string& spellName) const
{
    bool bPresent = false;
    for (size_t i = 0; !bPresent && i < m_additionalSpells.size(); ++i)
    {
        bPresent = m_additionalSpells[i].AddsToSpellList(ct, spellLevel)
                && m_additionalSpells[i].SpellName() == spellName;
    }
    return bPresent;
}
