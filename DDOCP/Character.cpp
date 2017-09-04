// Character.cpp
//
#include "StdAfx.h"
#include "Character.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "DDOCPDoc.h"
#include <algorithm>

#define DL_ELEMENT Character

namespace
{
    const wchar_t f_saxElementName[] = L"Character";
    DL_DEFINE_NAMES(Character_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Character::Character(CDDOCPDoc * pDoc) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pDocument(pDoc),
    m_SpecialFeats(L"SpecialFeats"),
    m_bonusActionPoints(0),
    m_racialTreeSpend(0),
    m_otherTreeSpend(0),
    m_previousGuildLevel(0)
{
    DL_INIT(Character_PROPERTIES)
    // make sure we have MAX_LEVEL default LevelTraining objects in the list
    size_t count = m_Levels.size();
    for (size_t index = count; index < MAX_LEVEL; ++index)
    {
        LevelTraining lt;
        if (index >= MAX_CLASS_LEVEL)
        {
            // all levels above level MAX_CLASS_LEVEL are epic levels by default
            lt.Set_Class(Class_Epic);
        }
        m_Levels.push_back(lt);
    }
    // ensure we have MAX_TWISTS twist of fate objects
    while (m_Twists.size() < MAX_TWISTS)
    {
        m_Twists.push_back(TwistOfFate());
    }
    UpdateFeats();
    // by default they start with 1 gear layout called "Standard"
    EquippedGear gear("Standard");  // with no items yet selected
    m_GearSetups.push_back(gear);
    m_ActiveGear = "Standard";
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
    // clear the first MAX_LEVEL
    while (m_Levels.size() > MAX_LEVEL)
    {
        m_Levels.erase(m_Levels.begin());
    }
    // same for twists of fate
    // ensure we have MAX_TWISTS twist of fate objects
    while (m_Twists.size() > MAX_TWISTS)
    {
        m_Twists.pop_front();
    }
    // keep older files working
    m_hasGuildLevel = true; // default guild level is 0

    SaxContentElement::EndElement();
    DL_END(Character_PROPERTIES)
    ASSERT(m_Levels.size() == MAX_LEVEL);
    ASSERT(m_Twists.size() == MAX_TWISTS);

    // also remove the default "Standard" gear layout which we get by default
    m_GearSetups.pop_front();
    m_hasActiveGear = true;
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

bool Character::IsFeatTrained(const std::string & featName) const
{
    // return true if the given feat is trained
    bool bTrained = false;
    std::list<TrainedFeat> currentFeats = CurrentFeats(MAX_LEVEL);

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
    ASSERT(m_Levels.size() == MAX_LEVEL);
    ASSERT(level < MAX_LEVEL);
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
            size_t available = SkillPoints(
                    type,
                    Race(),
                    AbilityAtLevel(Ability_Intelligence, level),
                    level);
            (*it).Set_SkillPointsAvailable(available);
            hadRevoke = true;
        }
        ++it;
        ++level;
    }
    return hadRevoke;
}

void Character::UpdateFeats()
{
    // we start with any special feats (past lives etc)
    std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
    // for each level, determine what automatic feats are trained
    // also, update each level with the trainable feat types
    for (size_t level = 0; level < MAX_LEVEL; ++level)
    {
        UpdateFeats(level, &allFeats);
    }
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

void Character::NotifyEpicCompletionistChanged()
{
    NotifyAll(&CharacterObserver::UpdateEpicCompletionistChanged, this);
}

void Character::NotifyAvailableTwistsChanged()
{
    NotifyAll(&CharacterObserver::UpdateAvailableTwistsChanged, this);
}

void Character::NotifyEnhancementEffect(
        const std::string & enhancementName,
        const Effect & effect,
        size_t ranks)
{
    EffectTier et(effect, ranks);
    NotifyAll(&CharacterObserver::UpdateEnhancementEffect, this, enhancementName, et);
}

void Character::NotifyEnhancementEffectRevoked(
        const std::string & name,
        const Effect & effect,
        size_t tier)
{
    EffectTier et(effect, tier);
    NotifyAll(&CharacterObserver::UpdateEnhancementEffectRevoked, this, name, et);
}

void Character::NotifyEnhancementTrained(const std::string & enhancementName, bool isTier5, bool bActiveTree)
{
    NotifyAll(&CharacterObserver::UpdateEnhancementTrained, this, enhancementName, isTier5);
    if (bActiveTree)
    {
        const EnhancementTreeItem * pItem = FindEnhancement(enhancementName);
        if (pItem != NULL)
        {
            // enhancements may give multiple stances
            std::list<Stance>::const_iterator sit = pItem->Stances().begin();
            while (sit != pItem->Stances().end())
            {
                NotifyNewStance((*sit));
                ++sit;
            }
        }
    }
}

void Character::NotifyEnhancementRevoked(const std::string & enhancementName, bool isTier5, bool bActiveTree)
{
    NotifyAll(&CharacterObserver::UpdateEnhancementRevoked, this, enhancementName, isTier5);
    if (bActiveTree)
    {
        const EnhancementTreeItem * pItem = FindEnhancement(enhancementName);
        if (pItem != NULL)
        {
            // enhancements may give multiple stances
            std::list<Stance>::const_iterator sit = pItem->Stances().begin();
            while (sit != pItem->Stances().end())
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

void Character::NotifyAllLoadedEffects()
{
    // get a list of all feats and notify for each feat effect
    std::list<TrainedFeat> feats = CurrentFeats(MAX_LEVEL);
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
        // for each item trained, apply its effects
        std::list<TrainedEnhancement>::const_iterator eit = tree.m_Enhancements.begin();
        while (eit != tree.m_Enhancements.end())
        {
            const EnhancementTreeItem * pTreeItem = FindEnhancement((*eit).EnhancementName());
            ApplyEnhancementEffects(
                    tree.TreeName(),
                    (*eit).EnhancementName(),
                    (*eit).HasSelection() ? (*eit).Selection() : "",
                    pTreeItem->Ranks());
            ++eit;
        }
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

void Character::NotifyAllDestinyEffects()
{
    // we only notify for the active tree and trained twists of fate
    EpicDestinySpendInTree * pActiveTree = EpicDestiny_FindTree(ActiveEpicDestiny());
    if (pActiveTree != NULL)
    {
        ApplyAllEffects(pActiveTree->TreeName(), pActiveTree->Enhancements());
    }
}

void Character::NotifyAllTwistEffects()
{
    for (size_t twist = 0; twist < MAX_TWISTS; ++twist)
    {
        const TrainedEnhancement * trainedTwist = TrainedTwist(twist);
        if (trainedTwist != NULL)
        {
            // now train the effects of the selected twist
            std::string treeName;
            const EnhancementTreeItem * item = FindEnhancement(trainedTwist->EnhancementName(), &treeName);
            ApplyEnhancementEffects(
                    treeName,
                    trainedTwist->EnhancementName(),
                    trainedTwist->HasSelection() ? trainedTwist->Selection() : "",
                    trainedTwist->Ranks());
        }
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
    DetermineBuildPoints();     // switching too/from drop can affect the number of build points
    m_pDocument->SetModifiedFlag(TRUE);
    // revoking a racial feat can invalidate a feat selection in other levels (e.g. loss of Dodge)
    VerifyTrainedFeats();
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
        // class 1 is now no longer valid, bump it from all levels
        RevokeClass(Class2());
        Set_Class2(Class3());
        Set_Class3(Class_Unknown);
        classChoiceChange = true;
    }
    // do the same for class 2 and class 3
    while (!IsClassAvailable(Class3())
            && Class3() != Class_Unknown)
    {
        // class 1 is now no longer valid, bump it from all levels
        RevokeClass(Class3());
        Set_Class3(Class_Unknown);
        classChoiceChange = true;
    }
    if (classChoiceChange)
    {
        NotifyClassChoiceChanged();
    }
    UpdateFeats();
    NotifyAlignmentChanged(alignment);
    m_pDocument->SetModifiedFlag(TRUE);
    // revoking a class in theory can invalidate a feat selection
    VerifyTrainedFeats();
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
        ASSERT(m_Levels.size() == MAX_LEVEL);
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
        {
            size_t available = SkillPoints(
                    (*it).HasClass() ? (*it).Class() : Class_Unknown,
                    Race(),
                    AbilityAtLevel(Ability_Intelligence, level),
                    level);
            (*it).Set_SkillPointsAvailable(available);
            ++it;
        }
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
    // changing an inherent tome value can invalidate a feat selection (e.g Single Weapon Fighting)
    VerifyTrainedFeats();
}

void Character::SetClass1(size_t level, ClassType type)
{
    if (Class1() != Class_Unknown)
    {
        ClassType classFrom = Class1();
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
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    VerifyTrainedFeats();
}

void Character::SetClass2(size_t level, ClassType type)
{
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
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    VerifyTrainedFeats();
}

void Character::SetClass3(size_t level, ClassType type)
{
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
    NotifyClassChoiceChanged();
    m_pDocument->SetModifiedFlag(TRUE);
    VerifyTrainedFeats();
}

void Character::SetClass(size_t level, ClassType type)
{
    ClassType classFrom = Class_Unknown;;
    if (level < MAX_CLASS_LEVEL)    // 0 based
    {
        ASSERT(m_Levels.size() == MAX_LEVEL);
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        std::advance(it, level);
        classFrom = (*it).HasClass() ? (*it).Class() : Class_Unknown;
        (*it).Set_Class(type);
        size_t available = SkillPoints(
                type,
                Race(),
                AbilityAtLevel(Ability_Intelligence, level),
                level);
        (*it).Set_SkillPointsAvailable(available);
        // special case. When setting the class for level 1, all other heroic levels
        // currently set to Class_Unknown are also changed to the chosen class
        if (level == 0)
        {
            while (level < MAX_CLASS_LEVEL)
            {
                ++level;
                ++it;
                if (!(*it).HasClass()
                        || (*it).Class() == Class_Unknown)
                {
                    // train this class as well
                    (*it).Set_Class(type);
                    size_t available = SkillPoints(
                            type,
                            Race(),
                            AbilityAtLevel(Ability_Intelligence, level),
                            level);
                    (*it).Set_SkillPointsAvailable(available);
                }
            }
        }
        m_pDocument->SetModifiedFlag(TRUE);
    }
    NotifyClassChanged(classFrom, type, level);    // must be done before feat updates to keep spell lists kosher
    UpdateFeats();
}

void Character::SpendSkillPoint(
        size_t level,
        SkillType skill)
{
    // update the skill point spend for the correct level data
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    (*it).TrainSkill(skill);
    NotifySkillSpendChanged(level, skill);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::RevokeSkillPoint(
        size_t level,
        SkillType skill)
{
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    (*it).RevokeSkill(skill);
    NotifySkillSpendChanged(level, skill);
    // revoking a skill point in theory can invalidate a feat selection
    VerifyTrainedFeats();
    m_pDocument->SetModifiedFlag(TRUE);
}

// active point build affected
void Character::SpendOnAbility(AbilityType ability)
{
    m_BuildPoints.SpendOnAbility(ability);
    // if Intelligence has changed, update the available skill points
    if (ability == Ability_Intelligence)
    {
        ASSERT(m_Levels.size() == MAX_LEVEL);
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
        {
            size_t available = SkillPoints(
                    (*it).HasClass() ? (*it).Class() : Class_Unknown,
                    Race(),
                    AbilityAtLevel(Ability_Intelligence, level),
                    level);
            (*it).Set_SkillPointsAvailable(available);
            ++it;
        }
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
        ASSERT(m_Levels.size() == MAX_LEVEL);
        std::list<LevelTraining>::iterator it = m_Levels.begin();
        for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
        {
            size_t available = SkillPoints(
                    (*it).HasClass() ? (*it).Class() : Class_Unknown,
                    Race(),
                    AbilityAtLevel(Ability_Intelligence, level),
                    level);
            (*it).Set_SkillPointsAvailable(available);
            ++it;
        }
    }
    NotifyAbilityValueChanged(ability);
    m_pDocument->SetModifiedFlag(TRUE);
    // revoking an ability point in theory can invalidate a feat selection
    VerifyTrainedFeats();
}

void Character::TrainSpecialFeat(
        const std::string & featName)
{
    // just add a copy of the feat name to the current list
    std::list<TrainedFeat> trainedFeats = SpecialFeats().Feats();
    TrainedFeat tf;
    tf.Set_FeatName(featName);
    tf.Set_Type(TFT_Special);
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

    // number of past lives affects how many build points they have to spend
    DetermineBuildPoints();
    DetermineFatePoints();
    DetermineEpicCompletionist();

    // special feats may change the number of action points available
    CountBonusAP();
}

void Character::RevokeSpecialFeat(
        const std::string & featName)
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
        DetermineFatePoints();
        DetermineEpicCompletionist();
        // special feats may change the number of action points available
        CountBonusAP();
        if (feat.Acquire() == FeatAcquisition_RacialPastLife)
        {
            std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
            UpdateFeats(0, &allFeats);      // racial completionist state may have changed
        }
        if (feat.Acquire() == FeatAcquisition_HeroicPastLife)
        {
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
    default:
        ASSERT(FALSE);      // invalid level up level
        break;
    }
    NotifyAbilityValueChanged(old);
    NotifyAbilityValueChanged(ability);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::TrainFeat(
        const std::string & featName,
        TrainableFeatTypes type,
        size_t level)
{
    // ensure re-selection of same feat in same slot does not change anything
    // as this can cause enhancements and feats to be revoked.
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    std::advance(it, level);
    if (featName != (*it).FeatName(type))
    {
        // first revoke any previous trained feat in this slot at level
        std::string lostFeat = (*it).RevokeFeat(type);
        if (!lostFeat.empty())
        {
            const Feat & feat = FindFeat(lostFeat);
            RevokeFeatEffects(feat);
        }
        // train new
        (*it).TrainFeat(featName, type, level);
        const Feat & feat = FindFeat(featName);
        ApplyFeatEffects(feat);

        NotifyFeatTrained(featName);
        // some automatic feats may have changed due to the trained feat
        UpdateFeats();
        // a feat change can invalidate a feat selection at at later level
        VerifyTrainedFeats();
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

void Character::NowActive()
{
    // keep all views up to date
    DetermineBuildPoints();
    DetermineFatePoints();

    NotifyAllLoadedEffects();
    NotifyAllEnhancementEffects();
    NotifyAllReaperEnhancementEffects();
    NotifyAllDestinyEffects();
    NotifyAllTwistEffects();
    ApplyGearEffects();     // apply effects from equipped gear
    m_previousGuildLevel = 0;   // ensure all effects apply
    ApplyGuildBuffs();
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
    std::list<Feat>::const_iterator it = allFeats.begin();
    level++;    // 1 based for level comparison
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
                            || (*aait).Class() == Class_All);
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
            // if we meet the auto acquire criteria, make sure
            // we also meet the feats Requirements
            if (meetsRace
                    && meetsClass
                    && meetsLevel
                    && meetsSpecificLevel)
            {
                if ((*aait).HasIgnoreRequirements())
                {
                    // don't have to meet feat requirements
                    acquire = true;
                }
                else
                {
                    // let the feat determine whether its acquired
                    acquire = (*it).RequirementsToTrain().CanTrainFeat(
                            *this,
                            classLevels,
                            level,
                            currentFeats);
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
    return feats;
}

void Character::ActivateStance(const Stance & stance)
{
    // first activate the stance
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
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::DeactivateStance(const Stance & stance)
{
    // deactivation of a stance only affects that stance
    m_Stances.RevokeStance(stance.Name());
    NotifyStanceDeactivated(stance.Name());
    m_pDocument->SetModifiedFlag(TRUE);
}

bool Character::IsStanceActive(const std::string & name) const
{
    return m_Stances.IsStanceActive(name);
}

bool Character::IsClassSkill(
        SkillType skill,
        size_t level) const
{
    bool isClassSkill = false;  // assume not a class skill

    // check the class at each previous trained level to see if its a class skill
    ASSERT(m_Levels.size() == MAX_LEVEL);
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
        size_t level) const  // level is 0 based
{
    size_t abilityValue = 8
            + RacialModifier(Race(), ability)
            + m_BuildPoints.GetAbilitySpend(ability);

    abilityValue += LevelUpsAtLevel(ability, level);

    // add tomes on at levels 3, 7, 11, 15, 19, 23 and 27
    abilityValue += TomeAtLevel(ability, level);

    return abilityValue;
}

int Character::LevelUpsAtLevel(
        AbilityType ability,
        size_t level) const
{
    size_t levelUps = 0;
    // add on level ups on 4,8,12,16,20,24 and 28
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
    return levelUps;
}

int Character::TomeAtLevel(
        AbilityType ability,
        size_t level) const
{
    // level is 1 based for this calculation
    ++level;
    // add tomes on at levels 3, 7, 11, 15, 19, 23 and 27
    size_t maxTome = AbilityTomeValue(ability);
    size_t tv = 0;
    for (size_t tl = 3; tl <= level && tl < MAX_LEVEL; tl += 4)
    {
        if (tv < maxTome)
        {
            ++tv;   // tome applies from this level
        }
    }
    return tv;
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
    level = min(MAX_CLASS_LEVEL, level); // only valid for heroic levels
    if (isClassSkill)
    {
        // max skill is 3 + character level
        maxSkill = 3.0 + level;
    }
    else
    {
        // some skills must be a class skill to be trained
        if (skill == Skill_Perform
                || skill == Skill_DisableDevice
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

    ASSERT(m_Levels.size() == MAX_LEVEL);
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
        size_t tomeValue = SkillTomeValue(skill);
        size_t maxTome = (level / 4) + 1;
        skillLevel += min(tomeValue, maxTome);
    }
    return skillLevel;
}

std::vector<size_t> Character::ClassLevels(
        size_t level) const
{
    // return a vector of the number of class levels trained in each class
    // at the specified level
    std::vector<size_t> classLevels(Class_Count, 0);    // 0 for each class at start
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
    return classLevels;
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
        if (level < MAX_CLASS_LEVEL)
        {
            // TFT_Epic handles for level 21+
            trainable.push_back(TFT_Standard);
        }
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
    case Class_Artificer:
        // artificers get bonus feats at levels 4, 8, 12, 16 and 20
        if (classLevels[Class_Artificer] % 4 == 0)
        {
            trainable.push_back(TFT_ArtificerBonus);
        }
        break;

    case Class_Cleric:
        // clerics get a Follower of (faith) selection at level 1
        // and a deity feat at level 6
        if (classLevels[Class_Cleric] == 1)
        {
            trainable.push_back(TFT_FollowerOf);
        }
        if (classLevels[Class_Cleric] == 6)
        {
            trainable.push_back(TFT_Deity);
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

    case Class_FavoredSoul:
        // favored souls can select a follower of faith at level 1
        // a child of feat at level 3
        // a deity feat at level 6
        // a beloved of feat at level 12
        // and a damage reduction feat at level 20
        if (classLevels[Class_FavoredSoul] == 1)
        {
            trainable.push_back(TFT_FollowerOf);
        }
        if (classLevels[Class_FavoredSoul] == 3)
        {
            trainable.push_back(TFT_ChildOf);
        }
        if (classLevels[Class_FavoredSoul] == 6)
        {
            trainable.push_back(TFT_Deity);
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

        // epic destiny feats can also be trained at levels 26, 28 and 29
        if (level == 25     // level is 0 based
                || level == 27
                || level == 28)
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
            trainable.push_back(TFT_FollowerOf);
        }
        if (classLevels[Class_Paladin] == 6)
        {
            trainable.push_back(TFT_Deity);
        }
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
        // warlocks can select pact spells at levels 2, 5, 9, 14, 17 and 19
        if (classLevels[Class_Warlock] == 2
                || classLevels[Class_Warlock] == 5
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
            if (IsFeatTrainable(level, type, (*it))
                    ||  (*it).Name() == includeThisFeat)
            {
                // they can select this one, add it to the available list
                trainable.push_back((*it));
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
            // they have levels in this class
            // work out how much BAB these class levels give
            switch (ci)
            {
            case Class_Barbarian:
            case Class_Fighter:
            case Class_Paladin:
            case Class_Ranger:
                // full BAB class
                bab += classLevels[ci];
                break;

            case Class_Artificer:
            case Class_Bard:
            case Class_Cleric:
            case Class_Druid:
            case Class_FavoredSoul:
            case Class_Monk:
            case Class_Rogue:
            case Class_Warlock:
                // 0.75 BAB class
                bab += ((classLevels[ci] * 3) / 4); // fractions dropped
                break;

            case Class_Epic:
                // epic levels are 0.5 BAB also
            case Class_Sorcerer:
            case Class_Wizard:
                // 0.5 BAB class
                bab += (classLevels[ci] / 2);       // fractions dropped
                break;
            }
        }
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
    // get the list of effects this feat has
    const std::list<Effect> & effects = feat.Effects();
    std::list<Effect>::const_iterator feit = effects.begin();
    while (feit != effects.end())
    {
        NotifyFeatEffect(feat.Name(), (*feit));
        ++feit;
    }
    NotifyFeatTrained(feat.Name());
    // if we have just trained a feat that is also a stance
    // at a stance selection button for it
    if (feat.HasStanceData())
    {
        NotifyNewStance(feat.StanceData());
    }
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
    if (feat.HasStanceData())
    {
        NotifyRevokeStance(feat.StanceData());
    }
}

// enhancement support
bool Character::IsEnhancementTrained(
        const std::string & enhancementName,
        const std::string & selection) const
{
    // return true if this enhancement and selection is trained
    bool isTrained = (IsTrained(enhancementName, selection) != NULL);
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

size_t Character::TotalPointsAvailable(const std::string & treeName, TreeType type) const
{
    size_t aps = 0;
    switch (type)
    {
    case TT_racial:
        aps = MAX_ACTION_POINTS + m_bonusActionPoints;  // bonus APs only apply to racial tree
        break;
    case TT_enhancement:
        aps = MAX_ACTION_POINTS;
        break;
    case TT_reaper:
        aps = 1000;  // no upper limit on reaper APs (i.e. you can buy all)
        break;
    case TT_epicDestiny:
        // 0 APs unless the tree is claimed
        if (DestinyClaimed(treeName))
        {
            aps = DestinyLevel(treeName) * 4; // destiny level is 0..6, i.e. 0 to 24 APs
        }
        break;
    }
    return aps;
}

size_t Character::AvailableActionPoints(const std::string & treeName, TreeType type) const
{
    // for actions points we need to track:
    // how many bonus action points the character has (m_bonusActionPoints)
    // how many action points have been spent in the racial tree (m_racialTreeSpend)
    // how many action points have been spent in other trees (m_otherTreeSpend)
    size_t aps = 0;
    switch (type)
    {
    case TT_racial:
        aps = MAX_ACTION_POINTS + m_bonusActionPoints
                - m_racialTreeSpend
                - m_otherTreeSpend;
        break;
    case TT_enhancement:
        aps = MAX_ACTION_POINTS
                - max(m_racialTreeSpend - m_bonusActionPoints, 0)
                - m_otherTreeSpend;
        break;
    case TT_reaper:
        aps = 1000;  // no upper limit on reaper APs (i.e. you can buy all)
        break;
    case TT_epicDestiny:
        // 0 APs unless the tree is claimed
        if (DestinyClaimed(treeName))
        {
            aps = DestinyLevel(treeName) * 4; // destiny level is 0..6, i.e. 0 to 24 APs
            Character * pThis = const_cast<Character*>(this); // its used in a const way
            aps -= pThis->APSpentInTree(treeName);
        }
        break;
    }
    return aps;
}

size_t Character::APSpentInTree(const std::string & treeName)
{
    size_t spent = 0;
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
    EpicDestinySpendInTree * pEpicDestinyItem = EpicDestiny_FindTree(treeName);
    if (pEpicDestinyItem != NULL) // NULL if no items spent in tree or enhancement/reaper tree
    {
        spent = pEpicDestinyItem->Spent();
    }
    return spent;
}

const TrainedEnhancement * Character::IsTrained(
        const std::string & enhancementName,
        const std::string & selection) const
{
    // return NULL if item is not in the list
    const TrainedEnhancement * pItem = NULL;
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
    if (pItem == NULL)
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
    if (pItem == NULL)
    {
        // iterate the epic destiny list to see if its present
        std::list<EpicDestinySpendInTree>::const_iterator it = m_EpicDestinyTreeSpend.begin();
        while (pItem == NULL
                && it != m_EpicDestinyTreeSpend.end())
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
        size_t cost)
{
    // Find the tree tracking amount spent and enhancements trained
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    const EnhancementTree & eTree = GetEnhancementTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        EnhancementSpendInTree tree;
        tree.Set_TreeName(treeName);
        tree.Set_TreeVersion(eTree.Version());  // need to track version of tree spent in
        m_EnhancementTreeSpend.push_back(tree);
        pItem = &m_EnhancementTreeSpend.back();
    }
    size_t ranks = 0;
    const EnhancementTreeItem * pTreeItem = FindEnhancement(enhancementName);
    int spent = pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            pTreeItem->HasTier5(),
            &ranks);
    if (eTree.HasIsRacialTree())
    {
        m_racialTreeSpend += spent;
    }
    else
    {
        m_otherTreeSpend += spent;
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
    ApplyEnhancementEffects(treeName, enhancementName, selection, ranks);
    NotifyEnhancementTrained(enhancementName, pTreeItem->HasTier5(), true);
    NotifyActionPointsChanged();
    NotifyAPSpentInTreeChanged(treeName);
}

void Character::Enhancement_RevokeEnhancement(
        const std::string & treeName)
{
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    if (pItem != NULL)
    {
        bool wasTier5 = pItem->HasTier5();
        // return points available to spend also
        std::string revokedEnhancement;
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        size_t spent = pItem->RevokeEnhancement(
                &revokedEnhancement,
                &revokedEnhancementSelection,
                &ranks);
        const EnhancementTree & eTree = GetEnhancementTree(treeName);
        if (eTree.HasIsRacialTree())
        {
            m_racialTreeSpend -= spent;
        }
        else
        {
            m_otherTreeSpend -= spent;
        }
        // now notify all and sundry about the enhancement effects
        // get the list of effects this enhancement has
        std::string displayName = GetEnhancementName(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        std::list<Effect> effects = GetEnhancementEffects(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        RevokeEnhancementEffects(displayName, ranks, effects);
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
        NotifyEnhancementRevoked(revokedEnhancement, wasTier5, true);
        NotifyActionPointsChanged();
        NotifyAPSpentInTreeChanged(treeName);
    }
}

void Character::Enhancement_ResetEnhancementTree(const std::string & treeName)
{
    // a whole tree is being reset
    EnhancementSpendInTree * pItem = Enhancement_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            Enhancement_RevokeEnhancement(treeName);
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
    }
}

void Character::Enhancement_SetSelectedTrees(const SelectedEnhancementTrees & trees)
{
    Set_SelectedTrees(trees);
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

void Character::DetermineBuildPoints()
{
    // determine how many build points the current past life count
    // allows this character to have
    size_t numPastLifeFeats = 0;
    const std::list<TrainedFeat> & feats = SpecialFeats().Feats();
    std::list<TrainedFeat>::const_iterator it = feats.begin();
    while (it != feats.end())
    {
        const Feat & feat = FindFeat((*it).FeatName());
        if (feat.Acquire() == FeatAcquisition_EpicPastLife
                || feat.Acquire() == FeatAcquisition_HeroicPastLife
                || feat.Acquire() == FeatAcquisition_RacialPastLife
                || feat.Acquire() == FeatAcquisition_IconicPastLife)
        {
            // it is a trained feat that affects number of available
            // build points
            ++numPastLifeFeats;
        }
        ++it;
    }
    // we do not currently support 28/32 (adventurer vs Champion) build point selection
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
        // all other races go 32->34->36
        switch (numPastLifeFeats)
        {
            case 0: buildPoints = 32; break;
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
}

void Character::CountBonusAP()
{
    // look through the list of all special feats and count how many bonus AP there are
    // if the number has changed from what we currently have, change it and notify
    CWinApp * pApp = AfxGetApp();
    CDDOCPApp * pDDOApp = dynamic_cast<CDDOCPApp*>(pApp);
    if (pDDOApp != NULL)
    {
        const std::list<Feat> & specialFeats = pDDOApp->RacialPastLifeFeats();
        size_t APcount = 0;
        std::list<Feat>::const_iterator fi = specialFeats.begin();
        while (fi != specialFeats.end())
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
                        APcount += (size_t)ei->Amount();
                    }
                    else if (ei->HasAmountVector())
                    {
                        size_t count = GetSpecialFeatTrainedCount(fi->Name());
                        if (count > 0)
                        {
                            APcount += (size_t)ei->AmountVector()[count-1];
                        }
                    }
                }
                ei++;
            }
            fi++;
        }
        if (APcount != m_bonusActionPoints)
        {
            m_bonusActionPoints = APcount;
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

    // keep a message that will inform the user about changes made
    bool displayMessage = false;
    CString message;

    {
        std::list<EnhancementSpendInTree>::iterator etsit = m_EnhancementTreeSpend.begin();
        while (etsit != m_EnhancementTreeSpend.end())
        {
            const EnhancementTree & tree = GetEnhancementTree((*etsit).TreeName());
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
                size_t apsSpent = 0;
                std::list<TrainedEnhancement> te = (*etsit).Enhancements();
                std::list<TrainedEnhancement>::iterator teit = te.begin();
                while (teit != te.end())
                {
                    const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                    apsSpent += pTreeItem->Cost() * (*teit).Ranks();
                    // cost also updated so revoke of items will work
                    (*teit).SetCost(pTreeItem->Cost());
                    ++teit;
                }
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
                    m_otherTreeSpend += apsSpent;
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
                size_t apsSpent = 0;
                std::list<TrainedEnhancement> te = (*rtsit).Enhancements();
                std::list<TrainedEnhancement>::iterator teit = te.begin();
                while (teit != te.end())
                {
                    const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                    apsSpent += pTreeItem->Cost() * (*teit).Ranks();
                    // cost also updated so revoke of items will work
                    (*teit).SetCost(pTreeItem->Cost());
                    ++teit;
                }
                // now set it on the tree so it knows how much has been spent in it
                (*rtsit).Set_Enhancements(te);
                (*rtsit).SetSpent(apsSpent);
                // done
                ++rtsit;
            }
        }
    }

    {
        std::list<EpicDestinySpendInTree>::iterator edtsit = m_EpicDestinyTreeSpend.begin();
        while (edtsit != m_EpicDestinyTreeSpend.end())
        {
            const EnhancementTree & tree = GetEnhancementTree((*edtsit).TreeName());
            // is the recorded tree spend up to date with that loaded?
            size_t spendVersion = (*edtsit).TreeVersion();
            if (spendVersion != tree.Version())
            {
                // looks like this tree is now out of date, have to revoke all these
                // enhancements in this tree (i.e. just delete it)
                CString text;
                text.Format("All enhancements in tree \"%s\" were revoked as the tree has since been superseded\n",
                        (*edtsit).TreeName().c_str());
                message += text;
                displayMessage = true;
                edtsit = m_EpicDestinyTreeSpend.erase(edtsit);
            }
            else
            {
                // the tree is up to date, sum how many APs were spent in it
                size_t apsSpent = 0;
                std::list<TrainedEnhancement> te = (*edtsit).Enhancements();
                std::list<TrainedEnhancement>::iterator teit = te.begin();
                while (teit != te.end())
                {
                    const EnhancementTreeItem * pTreeItem = FindEnhancement((*teit).EnhancementName());
                    apsSpent += pTreeItem->Cost() * (*teit).Ranks();
                    // cost also updated so revoke of items will work
                    (*teit).SetCost(pTreeItem->Cost());
                    ++teit;
                }
                // now set it on the tree so it knows how much has been spent in it
                (*edtsit).Set_Enhancements(te);
                (*edtsit).SetSpent(apsSpent);
                // done
                ++edtsit;
            }
        }
    }
    // racial completionist state may have changed
    std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
    UpdateFeats(0, &allFeats);      // racial completionist state may have changed

    if (displayMessage)
    {
        AfxMessageBox(message, MB_ICONWARNING);
        m_pDocument->SetModifiedFlag(TRUE);
    }
    CountBonusAP();
}

void Character::VerifyTrainedFeats()
{
    // user has taken an action that could have invalidated a current feat selection
    // check them all to see if they are still valid, revoke those which are not
 
    // keep a message that will inform the user about changes made
    bool displayMessage = false;
    CString message("The following feats were revoked as requirements no longer met:\n\n");
    std::list<LevelTraining>::iterator it = m_Levels.begin();
    size_t level = 0;
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
            if (!IsFeatTrainable(level, (*fit).Type(), feat, true))
            {
                // no longer trainable, remove it from the list
                displayMessage = true;
                CString text;
                text.Format("Level %d: %s\n",
                        level + 1,     // 0 based, 1 based for display
                        (*fit).FeatName().c_str());
                message += text;
                // revoke from level training in this slot at level
                (*it).RevokeFeat((*fit).Type());
                RevokeFeatEffects(feat);
                NotifyFeatRevoked(feat.Name());
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
            canTrain = (type == TFT_Standard);
        }
    }
    if (canTrain)
    {
        // do we meet the requirements to train this feat?
        canTrain = feat.RequirementsToTrain().CanTrainFeat(
                *this,
                classLevels,
                level,
                currentFeats);
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

void Character::ApplyEnhancementEffects(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        size_t ranks)
{
    std::string displayName = GetEnhancementName(treeName, enhancementName, selection);
    std::list<Effect> effects = GetEnhancementEffects(treeName, enhancementName, selection);
    std::list<Effect>::const_iterator eit = effects.begin();
    while (eit != effects.end())
    {
        NotifyEnhancementEffect(displayName, (*eit), ranks);
        ++eit;
    }
}

void Character::RevokeEnhancementEffects(
        const std::string & name,
        size_t ranks,
        const std::list<Effect> & effects)
{
    std::list<Effect>::const_iterator eit = effects.begin();
    while (eit != effects.end())
    {
        NotifyEnhancementEffectRevoked(name, (*eit), ranks);
        ++eit;
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
        size_t cost)
{
    // Find the tree tracking amount spent and enhancements trained
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        ReaperSpendInTree tree;
        tree.Set_TreeName(treeName);
        const EnhancementTree & eTree = GetEnhancementTree(treeName);
        tree.Set_TreeVersion(eTree.Version());  // need to track version of tree spent in
        m_ReaperTreeSpend.push_back(tree);
        pItem = &m_ReaperTreeSpend.back();
    }
    size_t ranks = 0;
    pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            false,
            &ranks);
    // now notify all and sundry about the enhancement effects
    ApplyEnhancementEffects(treeName, enhancementName, selection, ranks);
    NotifyEnhancementTrained(enhancementName, false, true);
    NotifyAPSpentInTreeChanged(treeName);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::Reaper_RevokeEnhancement(
        const std::string & treeName)
{
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem != NULL)
    {
        // return points available to spend also
        std::string revokedEnhancement;
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        pItem->RevokeEnhancement(
                &revokedEnhancement,
                &revokedEnhancementSelection,
                &ranks);
        // now notify all and sundry about the enhancement effects
        // get the list of effects this enhancement has
        std::string displayName = GetEnhancementName(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        std::list<Effect> effects = GetEnhancementEffects(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        RevokeEnhancementEffects(displayName, ranks, effects);
        NotifyEnhancementRevoked(revokedEnhancement, false, true);
        NotifyAPSpentInTreeChanged(treeName);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

void Character::Reaper_ResetEnhancementTree(const std::string & treeName)
{
    // a whole tree is being reset
    ReaperSpendInTree * pItem = Reaper_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            Reaper_RevokeEnhancement(treeName);
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

void Character::DetermineFatePoints()
{
    // from the DDOWiki:
    //
    // Characters receive fate points from various sources:
    //      +1 for every 3 Epic Destiny levels
    //      +2 for reaching level 29
    //      +3 for reaching level 30
    //      +1 for every 4 Epic Past Lives
    //      Up to +3 from the Tome of Fate
    //
    // Fate points are never lost, except the +5 for reaching level 29 and 30. 
    //
    // Maximum fate points as of Update 29
    // There are currently 12 Epic Destinies available with 5 levels each:
    //      a total of 20 Fate Points available from levels. 
    // 12 Epic Past Life Feats each stack up to 3 times:
    //      36 Epic Past Lives -> 9 Fate Points 
    // 3 Fate Points from the Tome of Fate 
    // 5 Fate Points for level cap
    //
    // Total maximum achievable Fate Points = 37 

    // as we only consider characters at cap, we always start with at least
    // the basic 5 fate points from levels 29/30
    size_t fatePoints = 5;

    //      +1 for every 3 Epic Destiny levels
    // sum the levels for all epic destinies
    size_t destinyLevels = 0;
    std::list<EpicDestinySpendInTree>::iterator dit = m_EpicDestinyTreeSpend.begin();
    while (dit != m_EpicDestinyTreeSpend.end())
    {
        size_t level = DestinyLevel((*dit).TreeName());
        if (level > 0)
        {
            // DestinyLevel is 1 to 6, yet true level for Fate points
            // is 0 to 5
            --level;
        }
        destinyLevels += level;
        ++dit;
    }
    fatePoints += (destinyLevels / 3); // integer arithmetic (drop fractions)

    //      +1 for every 4 Epic Past Life 
    // count the number of trained epic past lives
    size_t numEpicPastLives = 0;
    const std::list<TrainedFeat> & feats = SpecialFeats().Feats();
    std::list<TrainedFeat>::const_iterator fit = feats.begin();
    while (fit != feats.end())
    {
        const Feat & feat = FindFeat((*fit).FeatName());
        if (feat.Acquire() == FeatAcquisition_EpicPastLife)
        {
            // epic past life
            ++numEpicPastLives;
        }
        ++fit;
    }
    fatePoints += (numEpicPastLives / 4);   // integer arithmetic (drop fractions)

    //      Up to +3 from the Tome of Fate 
    fatePoints += TomeOfFate();
    if (fatePoints != FatePoints())
    {
        // number of fate points available has changed
        Set_FatePoints(fatePoints);
        NotifyFatePointsChanged();
    }
}

void Character::DetermineEpicCompletionist()
{
    // for epic completionist, they need 3 or more feats in each sphere
    // of arcane, divine, martial and primal.
    // Determine the counts in each
    size_t spherePastLives[] = {0, 0, 0, 0};
    const std::list<TrainedFeat> & feats = SpecialFeats().Feats();
    std::list<TrainedFeat>::const_iterator fit = feats.begin();
    while (fit != feats.end())
    {
        const Feat & feat = FindFeat((*fit).FeatName());
        if (feat.Acquire() == FeatAcquisition_EpicPastLife)
        {
            // epic past life, what sphere?
            if (feat.Sphere() == "Arcane")
            {
                spherePastLives[0]++;
            }
            if (feat.Sphere() == "Divine")
            {
                spherePastLives[1]++;
            }
            if (feat.Sphere() == "Martial")
            {
                spherePastLives[2]++;
            }
            if (feat.Sphere() == "Primal")
            {
                spherePastLives[3]++;
            }
        }
        ++fit;
    }
    // now we have added them all up, see if we qualify for epic completionist
    bool qualify = true;
    for (size_t i = 0; i < 4; ++i)
    {
        if (spherePastLives[i] < 3)
        {
            qualify = false;
        }
    }
    if (qualify != HasEpicCompletionist())
    {
        // we have a change in state of our epic completionist
        if (qualify)
        {
            Set_EpicCompletionist();
        }
        else
        {
            Clear_EpicCompletionist();
            // Ensure any twist trained in the epic completionist slot
            // is removed as its no longer available
            SetTwist(MAX_TWISTS - 1, NULL);
        }
        NotifyEpicCompletionistChanged();
    }
}

void Character::EpicDestiny_SetActiveDestiny(const std::string & treeName)
{
    if (treeName != ActiveEpicDestiny())
    {
        // there is a change in active destiny

        // first revoke all the enhancement effects of the previously active
        // destiny (if there was one)
        EpicDestinySpendInTree * pPrevious = EpicDestiny_FindTree(ActiveEpicDestiny());
        if (pPrevious != NULL)
        {
            // we cheat for this. To get everything to revoke correctly we
            // do to a tree reset at this point, but to avoid losing the user
            // selections, we first take a copy of the tree state and then re-insert the
            // copy back into the tree list after it becomes de-active
            EpicDestinySpendInTree copy = *pPrevious;
            EpicDestiny_ResetEnhancementTree(ActiveEpicDestiny());
            // no re-insert the copy back into the trained tree list
            ASSERT(EpicDestiny_FindTree(ActiveEpicDestiny()) == NULL);
            m_EpicDestinyTreeSpend.push_back(copy);
        }
        Set_ActiveEpicDestiny(treeName);
        // now apply all the trained enhancement effects in the selected destiny
        EpicDestinySpendInTree * pNewActive = EpicDestiny_FindTree(treeName);
        if (pNewActive != NULL) // may be active and have nothing trained
        {
            ApplyAllEffects(pNewActive->TreeName(), pNewActive->Enhancements());
        }
        // active tree affects available twists
        DetermineFatePoints(); // as we did a tree reset we lost the fate points, recalculate then back in
        NotifyAvailableTwistsChanged();
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

bool Character::DestinyClaimed(const std::string & treeName) const
{
    // the destiny is claimed if any items are trained in it
    // core items in destinies cost 0 AP, thus user can always train the 1st
    // core enhancement
    Character * pThis = const_cast<Character*>(this); // its used in a const way
    EpicDestinySpendInTree * pItem = pThis->EpicDestiny_FindTree(treeName);
    return (pItem != NULL);
}

size_t Character::DestinyLevel(const std::string & treeName) const
{
    // return the number of trained core items in this destiny tree
    size_t destinyLevel = 0;
    Character * pThis = const_cast<Character*>(this); // its used in a const way
    EpicDestinySpendInTree * pItem = pThis->EpicDestiny_FindTree(treeName);
    if (pItem != NULL)
    {
        // count each core item trained in this tree to determine destiny level
        const std::list<TrainedEnhancement> & te = pItem->Enhancements();
        std::list<TrainedEnhancement>::const_iterator it = te.begin();
        while (it != te.end())
        {
            if ((*it).EnhancementName().find("Core") != std::string::npos)
            {
                // a core item is present
                ++destinyLevel;
            }
            ++it;
        }
    }
    return destinyLevel;
}

void Character::EpicDestiny_TrainEnhancement(
        const std::string & treeName,
        const std::string & enhancementName,
        const std::string & selection,
        size_t cost)
{
    // Find the tree tracking amount spent and enhancements trained
    EpicDestinySpendInTree * pItem = EpicDestiny_FindTree(treeName);
    if (pItem == NULL)
    {
        // no tree item available for this tree, its a first time spend
        // create an object to handle this tree
        EpicDestinySpendInTree tree;
        tree.Set_TreeName(treeName);
        const EnhancementTree & eTree = GetEnhancementTree(treeName);
        tree.Set_TreeVersion(eTree.Version());  // need to track version of tree spent in
        m_EpicDestinyTreeSpend.push_back(tree);
        pItem = &m_EpicDestinyTreeSpend.back();
    }
    size_t ranks = 0;
    pItem->TrainEnhancement(
            enhancementName,
            selection,
            cost,
            false,
            &ranks);
    // now notify all and sundry about the enhancement effects only if this is the
    // active tree
    if (treeName == ActiveEpicDestiny())
    {
        ApplyEnhancementEffects(treeName, enhancementName, selection, ranks);
    }
    DetermineFatePoints();
    NotifyEnhancementTrained(enhancementName, false, (treeName == ActiveEpicDestiny()));

    const EnhancementTreeItem * pTreeItem = FindEnhancement(enhancementName);
    ASSERT(pTreeItem != NULL);
    if (pTreeItem->YPosition() > 0 && pTreeItem->YPosition() <= MAX_TWIST_LEVEL)
    {
        // this item affects the available twists
        NotifyAvailableTwistsChanged();
    }
    NotifyAPSpentInTreeChanged(treeName);
    m_pDocument->SetModifiedFlag(TRUE);
}

void Character::EpicDestiny_RevokeEnhancement(
        const std::string & treeName)
{
    EpicDestinySpendInTree * pItem = EpicDestiny_FindTree(treeName);
    if (pItem != NULL)
    {
        // return points available to spend also
        std::string revokedEnhancement;
        std::string revokedEnhancementSelection;
        size_t ranks = 0;
        pItem->RevokeEnhancement(
                &revokedEnhancement,
                &revokedEnhancementSelection,
                &ranks);
        // now notify all and sundry about the enhancement effects
        // get the list of effects this enhancement has
        std::string displayName = GetEnhancementName(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        std::list<Effect> effects = GetEnhancementEffects(
                treeName,
                revokedEnhancement,
                revokedEnhancementSelection);
        // now notify all and sundry about the enhancement effects only if this is the
        // active tree
        if (treeName == ActiveEpicDestiny())
        {
            RevokeEnhancementEffects(displayName, ranks, effects);
        }
        DetermineFatePoints();
        NotifyEnhancementRevoked(revokedEnhancement, false, (treeName == ActiveEpicDestiny()));

        const EnhancementTreeItem * pTreeItem = FindEnhancement(revokedEnhancement);
        ASSERT(pTreeItem != NULL);
        if (pTreeItem->YPosition() > 0 && pTreeItem->YPosition() <= MAX_TWIST_LEVEL)
        {
            // this item affects the available twists
            NotifyAvailableTwistsChanged();
        }
        NotifyAPSpentInTreeChanged(treeName);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

void Character::EpicDestiny_ResetEnhancementTree(const std::string & treeName)
{
    // a whole tree is being reset
    EpicDestinySpendInTree * pItem = EpicDestiny_FindTree(treeName);
    if (pItem != NULL)
    {
        // clear all the enhancements trained by revoking them until none left
        while (pItem->Enhancements().size() > 0)
        {
            EpicDestiny_RevokeEnhancement(treeName);
            pItem = EpicDestiny_FindTree(treeName);
        }
        // now remove the tree entry from the list (not present if no spend in tree)
        std::list<EpicDestinySpendInTree>::iterator it = m_EpicDestinyTreeSpend.begin();
        while (it != m_EpicDestinyTreeSpend.end())
        {
            if ((*it).TreeName() == treeName)
            {
                // done once we find it
                m_EpicDestinyTreeSpend.erase(it);
                break;
            }
            ++it;
        }
        NotifyEnhancementTreeReset();
        NotifyAPSpentInTreeChanged(treeName);
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

EpicDestinySpendInTree * Character::EpicDestiny_FindTree(const std::string & treeName)
{
    // Find the tree tracking amount spent and enhancements trained
    EpicDestinySpendInTree * pItem = NULL;
    std::list<EpicDestinySpendInTree>::iterator it = m_EpicDestinyTreeSpend.begin();
    while (pItem == NULL
            && it != m_EpicDestinyTreeSpend.end())
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
                pTreeItem->Ranks());
        // enhancements may give multiple stances
        std::list<Stance>::const_iterator sit = pTreeItem->Stances().begin();
        while (sit != pTreeItem->Stances().end())
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
        std::string displayName = GetEnhancementName(
                treename,
                (*eit).EnhancementName(),
                (*eit).HasSelection() ? (*eit).Selection() : "");
        std::list<Effect> effects = GetEnhancementEffects(
                treename,
                (*eit).EnhancementName(),
                (*eit).HasSelection() ? (*eit).Selection() : "");
        RevokeEnhancementEffects(
                displayName,
                pTreeItem->Ranks(),
                effects);
        // enhancements may give multiple stances
        std::list<Stance>::const_iterator sit = pTreeItem->Stances().begin();
        while (sit != pTreeItem->Stances().end())
        {
            NotifyRevokeStance((*sit));
            ++sit;
        }
        ++eit;
    }
}

void Character::SetTomeOfFate(size_t value)
{
    if (value != TomeOfFate())
    {
        Set_TomeOfFate(value);
        DetermineFatePoints();  // will have changed
        m_pDocument->SetModifiedFlag(TRUE);
    }
}

bool Character::IsTwistActive(size_t twistIndex) const
{
    ASSERT(twistIndex < MAX_TWISTS);
    // a twist is active if:
    // its the first twist
    // or the previous twist has at least 1 trained rank
    bool active = false;
    if (twistIndex == 0)
    {
        active = true;
    }
    else
    {
        active = (TwistTier(twistIndex-1) > 0);
    }
    return active;
}

size_t Character::TwistTier(size_t twistIndex) const
{
    ASSERT(twistIndex < MAX_TWISTS);
    size_t tier = 0;
    // ask the object for its tier if it exists, else its tier 0
    if (twistIndex < m_Twists.size())
    {
        std::list<TwistOfFate>::const_iterator it = m_Twists.begin();
        std::advance(it, twistIndex);
        tier = (*it).Tier();
    }
    return tier;
}

bool Character::CanUpgradeTwist(size_t twistIndex) const
{
    ASSERT(twistIndex < MAX_TWISTS);
    // we can only upgrade this twist if we have enough fate points
    // first  twist costs 1,2,3,4 to upgrade
    // 2nd twist costs 2,3,4,5 to upgrade
    // 3rd twist costs 3,4,5,6 to upgrade...
    // and we are not at the maximum tier
    size_t cost = 1 + twistIndex + TwistTier(twistIndex);
    size_t spent = SpentFatePoints();
    // available can be negative due to user shenanigans
    int availableFatePoints = (int)FatePoints() - (int)spent;
    return ((int)cost <= availableFatePoints)
            && (TwistTier(twistIndex) < MAX_TWIST_LEVEL);
}

void Character::UpgradeTwist(size_t twistIndex)
{
    // note that upgrading a twist level cannot invalidate any
    // trained enhancement
    ASSERT(twistIndex < MAX_TWISTS);
    ASSERT(TwistTier(twistIndex) < MAX_TWIST_LEVEL);
    std::list<TwistOfFate>::iterator it = m_Twists.begin();
    std::advance(it, twistIndex);
    (*it).IncrementTier();
    NotifyFatePointsChanged();  // number available has changed
    m_pDocument->SetModifiedFlag(TRUE);
    NotifyAvailableTwistsChanged();
}

void Character::DowngradeTwist(size_t twistIndex)
{
    ASSERT(twistIndex < MAX_TWISTS);
    ASSERT(TwistTier(twistIndex) > 0);
    std::list<TwistOfFate>::iterator it = m_Twists.begin();
    std::advance(it, twistIndex);
    (*it).DecrementTier();
    NotifyFatePointsChanged();  // number available has changed
    m_pDocument->SetModifiedFlag(TRUE);
    // if the twist can no longer hold the trained enhancement, revoke it
    const TrainedEnhancement * trainedTwist = TrainedTwist(twistIndex);
    if (trainedTwist != NULL)
    {
        std::string treeName;
        const EnhancementTreeItem * item = FindEnhancement(trainedTwist->EnhancementName(), &treeName);
        if (item->YPosition() > TwistTier(twistIndex))
        {
            // the selected twist is too high level for this slot
            // have to revoke it
            SetTwist(twistIndex, NULL);
        }
    }
    NotifyAvailableTwistsChanged();
}

const TrainedEnhancement * Character::TrainedTwist(size_t twistIndex) const
{
    const TrainedEnhancement * item = NULL;
    std::list<TwistOfFate>::const_iterator it = m_Twists.begin();
    std::advance(it, twistIndex);
    if ((*it).HasTwist())
    {
        item = &(*it).Twist();
    }
    return item;
}

std::list<TrainedEnhancement> Character::AvailableTwists(size_t twistIndex) const
{
    size_t maxTwistTier = TwistTier(twistIndex);
    std::list<TrainedEnhancement> availableTwists;
    // to get the list of available twists we need to iterate all the
    // trained enhancements in all destinies except the active one
    // we only include the TrainedEnhancement if:
    // its not a core item
    // its tier is less than or equal to the max tier
    std::list<EpicDestinySpendInTree>::const_iterator edti = m_EpicDestinyTreeSpend.begin();
    while (edti != m_EpicDestinyTreeSpend.end())
    {
        // must not be the active tree
        if ((*edti).TreeName() != ActiveEpicDestiny())
        {
            const std::list<TrainedEnhancement> & te = (*edti).Enhancements();
            std::list<TrainedEnhancement>::const_iterator tei = te.begin();
            while (tei != te.end())
            {
                // must be in tier range to train. Get the actual enhancement
                const EnhancementTreeItem * item = FindEnhancement((*tei).EnhancementName());
                ASSERT(item != NULL);
                // we can tell what tier this item is by the enhancements YPosition value
                if (item->YPosition() > 0           // not a core (tier 0)
                        && item->YPosition() <= maxTwistTier)
                {
                    // this item qualifies as a twist, add it
                    availableTwists.push_back((*tei));
                }
                ++tei;
            }
        }
        ++edti;     // on to the next tree
    }
    // remove any other trained twists from the list (can't twist same item twice)
    for (size_t twist = 0; twist < MAX_TWISTS; ++twist)
    {
        // don't remove any item that is the selection for this twist
        if (twist != twistIndex)
        {
            // remove this twist (if there is one)
            const TrainedEnhancement * trainedTwist = TrainedTwist(twist);
            if (trainedTwist != NULL)
            {
                bool found = false;
                std::list<TrainedEnhancement>::iterator it = availableTwists.begin();
                while (!found && it != availableTwists.end())
                {
                    if ((*it).EnhancementName() == trainedTwist->EnhancementName())
                    {
                        // this twist is selected elsewhere, remove from available list
                        it = availableTwists.erase(it);
                        found = true;   // were done
                    }
                    else
                    {
                        // check the next one
                        ++it;
                    }
                }
            }
        }
    }

    return availableTwists;
}

void Character::SetTwist(size_t twistIndex, const TrainedEnhancement * te)
{
    // as the twist is changing, clear the effects on any previously trained twist
    const TrainedEnhancement * trainedTwist = TrainedTwist(twistIndex);
    if (trainedTwist != NULL)
    {
        std::string treeName;
        const EnhancementTreeItem * item = FindEnhancement(trainedTwist->EnhancementName(), &treeName);
        ASSERT(item != NULL);
        std::list<Effect> effects = GetEnhancementEffects(
                treeName,
                trainedTwist->EnhancementName(),
                trainedTwist->HasSelection() ? trainedTwist->Selection() : "");
        std::string displayName = GetEnhancementName(
                treeName,
                trainedTwist->EnhancementName(),
                trainedTwist->HasSelection() ? trainedTwist->Selection() : "");
        // need to call once per rank to fully revoke
        for (size_t i = 0; i < trainedTwist->Ranks(); ++i)
        {
            RevokeEnhancementEffects(
                    displayName,
                    trainedTwist->Ranks(),
                    effects);
        }
    }

    ASSERT(twistIndex < MAX_TWISTS);
    std::list<TwistOfFate>::iterator it = m_Twists.begin();
    std::advance(it, twistIndex);
    if (te != NULL)
    {
        (*it).Set_Twist(*te);
        // now train the effects of the selected twist
        std::string treeName;
        const EnhancementTreeItem * item = FindEnhancement(te->EnhancementName(), &treeName);
        ApplyEnhancementEffects(
                treeName,
                te->EnhancementName(),
                te->HasSelection() ? te->Selection() : "",
                te->Ranks());
    }
    else
    {
        (*it).Clear_Twist();
        // cleared, no effects
    }

    // as this selection has changed, other twists selectable twists will have changed
    NotifyAvailableTwistsChanged();
    m_pDocument->SetModifiedFlag(TRUE);
}

size_t Character::SpentFatePoints() const
{
    // count how many fate points have been spent by all 5 possible twists
    size_t spent = 0;
    for (size_t twistIndex = 0; twistIndex < MAX_TWISTS; ++twistIndex)
    {
        size_t tier = TwistTier(twistIndex);
        for (size_t tierCost = 0; tierCost < tier; ++tierCost)
        {
            spent += (1 + twistIndex + tierCost);
        }
    }
    return spent;
}

int Character::AvailableActionPoints() const
{
    return MAX_ACTION_POINTS
            + m_bonusActionPoints
            - m_racialTreeSpend
            - m_otherTreeSpend;
}

int Character::BonusActionPoints() const
{
    return m_bonusActionPoints;
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
}

EquippedGear Character::GetGearSet(const std::string & name)
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

EquippedGear Character::ActiveGearSet()
{
    return GetGearSet(ActiveGear());
}

void Character::SetGear(const std::string & name, const EquippedGear & gear)
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
            (*it) = gear;
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
}

void Character::RevokeGearEffects()
{
    EquippedGear gear = ActiveGearSet();
    // iterate the items
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (gear.HasItemInSlot((InventorySlotType)i))
        {
            Item item = gear.ItemInSlot((InventorySlotType)i);
            // revoke the items effects
            const std::vector<Effect> & effects = item.Effects();
            std::vector<Effect>::const_iterator it = effects.begin();
            while (it != effects.end())
            {
                NotifyItemEffectRevoked(item.Name(), (*it));
                ++it;
            }
            // do the same for any augment slots
            const std::vector<ItemAugment> & augments = item.Augments();
            for (size_t ai = 0; ai < augments.size(); ++ai)
            {
                if (augments[ai].HasSelectedAugment())
                {
                    // there is an augment in this position
                    const Augment & augment = FindAugmentByName(augments[ai].SelectedAugment());
                    // name is:
                    // <item>:<augment type>:<Augment name>
                    std::stringstream ss;
                    ss << item.Name()
                            << " : " << EnumEntryText(augments[ai].Type(), augmentTypeMap)
                            << " : " << augment.Name();
                    // now revoke the augments effects
                    std::string name;
                    name = ss.str();
                    const std::list<Effect> & effects = augment.Effects();
                    std::list<Effect>::const_iterator it = effects.begin();
                    while (it != effects.end())
                    {
                        NotifyItemEffectRevoked(name, (*it));
                        ++it;
                    }
                }
            }
        }
    }
}

void Character::ApplyGearEffects()
{
    EquippedGear gear = ActiveGearSet();
    // iterate the items
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (gear.HasItemInSlot((InventorySlotType)i))
        {
            Item item = gear.ItemInSlot((InventorySlotType)i);
            // apply the items effects
            const std::vector<Effect> & effects = item.Effects();
            std::vector<Effect>::const_iterator it = effects.begin();
            while (it != effects.end())
            {
                NotifyItemEffect(item.Name(), (*it));
                ++it;
            }
            // do the same for any augment slots
            const std::vector<ItemAugment> & augments = item.Augments();
            for (size_t ai = 0; ai < augments.size(); ++ai)
            {
                if (augments[ai].HasSelectedAugment())
                {
                    // there is an augment in this position
                    const Augment & augment = FindAugmentByName(augments[ai].SelectedAugment());
                    // name is:
                    // <item>:<augment type>:<Augment name>
                    std::stringstream ss;
                    ss << item.Name()
                            << " : " << EnumEntryText(augments[ai].Type(), augmentTypeMap)
                            << " : " << augment.Name();
                    // now notify the augments effects
                    std::string name;
                    name = ss.str();
                    const std::list<Effect> & effects = augment.Effects();
                    std::list<Effect>::const_iterator it = effects.begin();
                    while (it != effects.end())
                    {
                        NotifyItemEffect(name, (*it));
                        ++it;
                    }
                }
            }
        }
    }
}

void Character::ApplyGuildBuffs()
{
    // we only apply or revoke the effects within the m_previousGuildLevel to GuildLevel range
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
