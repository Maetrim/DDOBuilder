// Character.h
//
#pragma once
#include "XmlLib\DLMacros.h"

#include "AbilitySpend.h"
#include "ActiveStances.h"
#include "AlignmentTypes.h"
#include "EnhancementSpendInTree.h"
#include "EpicDestinySpendInTree.h"
#include "EquippedGear.h"
#include "SelectedEnhancementTrees.h"
#include "FeatsListObject.h"
#include "LevelTraining.h"
#include "ObserverSubject.h"
#include "RaceTypes.h"
#include "ReaperSpendInTree.h"
#include "SkillTomes.h"
#include "Stance.h"
#include "TwistOfFate.h"
#include "TrainedSpell.h"

class Character;
class CDDOCPDoc;
class Effect;
class Feat;

struct EffectTier
{
    EffectTier(const Effect & effect, size_t tier) : m_effect(effect), m_tier(tier) {};
    Effect m_effect;
    size_t m_tier;
    bool operator==(const EffectTier & other) const
    {
        return m_effect == other.m_effect
                && m_tier == other.m_tier;
    };
};

class CharacterObserver :
    public Observer<Character>
{
    public:
        virtual void UpdateAvailableBuildPointsChanged(Character * charData) {};
        virtual void UpdateAlignmentChanged(Character * charData, AlignmentType alignment) {};
        virtual void UpdateSkillSpendChanged(Character * charData, size_t level, SkillType skill) {};
        virtual void UpdateSkillTomeChanged(Character * charData, SkillType skill) {};
        virtual void UpdateClassChoiceChanged(Character * charData) {};
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) {};
        virtual void UpdateAbilityValueChanged(Character * charData, AbilityType ability) {};
        virtual void UpdateAbilityTomeChanged(Character * charData, AbilityType ability) {};
        virtual void UpdateRaceChanged(Character * charData, RaceType race) {};
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) {};
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) {};
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) {};
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) {};
        virtual void UpdateNewStance(Character * charData, const Stance & stance) {};
        virtual void UpdateRevokeStance(Character * charData, const Stance & stance) {};
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName) {};
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) {};
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) {};
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) {};
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) {};
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) {};
        virtual void UpdateEnhancementTreeReset(Character * charData) {};
        virtual void UpdateActionPointsChanged(Character * charData) {};
        virtual void UpdateAPSpentInTreeChanged(Character * charData, const std::string & treeName) {};
        virtual void UpdateSpellTrained(Character * charData, const TrainedSpell & spell) {};
        virtual void UpdateSpellRevoked(Character * charData, const TrainedSpell & spell) {};
        virtual void UpdateFatePointsChanged(Character * charData) {};
        virtual void UpdateEpicCompletionistChanged(Character * charData) {};
        virtual void UpdateAvailableTwistsChanged(Character * charData) {};
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) {};
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) {};
        virtual void UpdateGrantedFeatsChanged(Character * charData) {};
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) {};
};

class Character :
    public XmlLib::SaxContentElement,
    public Subject<CharacterObserver>,
    public CharacterObserver        // we observes ourselves
{
    public:
        Character(CDDOCPDoc * pDocument);
        void Write(XmlLib::SaxWriter * writer) const;

        void JustLoaded();  // called when file loaded and needs to be checked / updated
        void NowActive();   // called when active MDI changed

        // active point build affected
        size_t BaseAbilityValue(AbilityType ability) const;
        void SpendOnAbility(AbilityType ability);
        void RevokeSpendOnAbility(AbilityType ability);
        size_t DetermineBuildPoints();
        void SetBuildPoints(size_t buildPoints);

        // ability tomes
        void SetAbilityTome(AbilityType ability, size_t value);
        size_t AbilityTomeValue(AbilityType ability) const;

        // skill tomes
        void SetSkillTome(SkillType skill, size_t value);
        size_t SkillTomeValue(SkillType skill) const;

        // support query functions
        const LevelTraining & LevelData(size_t level) const;
        std::vector<size_t> ClassLevels(size_t level) const;
        size_t ClassLevels(ClassType ct) const;
        size_t BaseAttackBonus(size_t level) const;
        size_t AbilityAtLevel(AbilityType ability, size_t level, bool includeTomes) const;
        int LevelUpsAtLevel(AbilityType ability, size_t level) const;
        int TomeAtLevel(AbilityType ability, size_t level) const;
        AbilityType AbilityLevelUp(size_t level) const;

        void SetCharacterName(const CString & name);
        void SetRace(RaceType race);
        void SetAlignment(AlignmentType alignment);
        void SetAlignmentStances();

        // class selection
        bool IsClassAvailable(ClassType type) const;
        void SetClass1(size_t level, ClassType type);
        void SetClass2(size_t level, ClassType type);
        void SetClass3(size_t level, ClassType type);
        void SetClass(size_t level, ClassType type);
        bool RevokeClass(ClassType type);

        // skill points
        bool IsClassSkill(SkillType skill, size_t level) const;
        void SpendSkillPoint(size_t level, SkillType skill);
        void RevokeSkillPoint(size_t level, SkillType skill);
        double MaxSkillForLevel(SkillType skill, size_t level) const;
        size_t SpentAtLevel(SkillType skill, size_t level) const; // level is 0 based
        double SkillAtLevel(SkillType skill, size_t level, bool includeTome) const; // level is 0 based

        // feats
        bool IsFeatTrained(const std::string & featName) const;
        size_t GetSpecialFeatTrainedCount(const std::string & featName) const;
        TrainedFeat GetTrainedFeat(size_t level, TrainableFeatTypes type) const;
        void TrainSpecialFeat(const std::string & featName, TrainableFeatTypes type);
        void RevokeSpecialFeat(const std::string & featName, TrainableFeatTypes type);
        void SetAbilityLevelUp(size_t level, AbilityType ability);
        void TrainFeat(const std::string & featName, TrainableFeatTypes type, size_t level, bool autoTrained = false);
        std::list<TrainedFeat> AutomaticFeats(
                size_t level,
                const std::list<TrainedFeat> & currentFeats) const;
        std::vector<TrainableFeatTypes> TrainableFeatTypeAtLevel(size_t level) const;
        std::list<TrainedFeat> CurrentFeats(size_t level) const;
        std::vector<Feat> TrainableFeats(
                TrainableFeatTypes type,
                size_t level,
                const std::string & includeThisFeat) const;
        bool IsFeatTrainable(
                size_t level,
                TrainableFeatTypes type,
                const Feat & feat,
                bool includeTomes,
                bool alreadyTrained = false) const;
        bool HasGrantedFeats() const;
        const std::list<TrainedFeat> & GrantedFeats() const;

        // spells
        std::list<TrainedSpell> TrainedSpells(ClassType classType, size_t level) const;
        void TrainSpell(ClassType classType, size_t level, const std::string & spellName);
        void RevokeSpell(ClassType classType, size_t level, const std::string & spellName);
        bool IsSpellTrained(const std::string & spellName) const;
        void ApplySpellEffects(const std::string & spellName, size_t castingLevel);
        void RevokeSpellEffects(const std::string & spellName, size_t castingLevel);

        // stances
        void ActivateStance(const Stance & stance);
        void DeactivateStance(const Stance & stance);
        bool IsStanceActive(const std::string & name) const;

        // checks all possible tree types
        size_t TotalPointsAvailable(const std::string & treeName, TreeType type) const;
        size_t AvailableActionPoints(const std::string & treeName, TreeType type) const;
        size_t APSpentInTree(const std::string & treeName);
        const TrainedEnhancement * IsTrained(
                const std::string & enhancementName,
                const std::string & selection) const;

        // enhancement support
        bool IsEnhancementTrained(
                const std::string & enhancementName,
                const std::string & selection) const;
        std::list<TrainedEnhancement> CurrentEnhancements() const;
        void Enhancement_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                size_t cost);
        void Enhancement_RevokeEnhancement(
                const std::string & treeName,
                std::string * enhancementName = NULL,
                std::string * enhancementSelection = NULL);
        void Enhancement_ResetEnhancementTree(std::string treeName);
        void Enhancement_SetSelectedTrees(const SelectedEnhancementTrees & trees);
        int AvailableActionPoints() const;
        int BonusActionPoints() const;

        // reaper enhancement support
        void Reaper_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                size_t cost);
        void Reaper_RevokeEnhancement(const std::string & treeName);
        void Reaper_ResetEnhancementTree(std::string treeName);

        // epic destiny support
        void EpicDestiny_SetActiveDestiny(const std::string & treeName);
        bool DestinyClaimed(const std::string & treeName) const;
        size_t DestinyLevel(const std::string & treeName) const;
        void EpicDestiny_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                size_t cost);
        void EpicDestiny_RevokeEnhancement(
                const std::string & treeName,
                std::string * enhancementName = NULL,
                std::string * enhancementSelection = NULL);
        void EpicDestiny_ResetEnhancementTree(std::string treeName);

        // twists of fate support
        void SetTomeOfFate(size_t value);
        size_t SpentFatePoints() const;
        bool IsTwistActive(size_t twistIndex) const;
        size_t TwistTier(size_t twistIndex) const;
        bool CanUpgradeTwist(size_t twistIndex) const;
        void UpgradeTwist(size_t twistIndex);
        void DowngradeTwist(size_t twistIndex);
        const TrainedEnhancement * TrainedTwist(size_t twistIndex) const;
        std::list<TrainedEnhancement> AvailableTwists(size_t twistIndex) const;
        void SetTwist(size_t twistIndex, const TrainedEnhancement * te);

        // gear support
        void AddGearSet(const EquippedGear & gear);
        void DeleteGearSet(const std::string & name);
        EquippedGear GetGearSet(const std::string & name) const;
        EquippedGear ActiveGearSet() const;
        void SetGear(const std::string & name, InventorySlotType slot, const Item & item);
        void ClearGearInSlot(const std::string & name, InventorySlotType slot);
        bool LightWeaponInOffHand() const;
        bool IsFocusWeapon(WeaponType wt) const;

        // guild support
        void ToggleApplyGuildBuffs();
        void SetGuildLevel(size_t level);

        // general support functions
        std::string GetBuildDescription() const;

     protected:
        // notifications
        void NotifyAvailableBuildPointsChanged();
        void NotifyAlignmentChanged(AlignmentType alignment);
        void NotifySkillSpendChanged(size_t level, SkillType skill);
        void NotifySkillTomeChanged(SkillType skill);
        void NotifyClassChoiceChanged();
        void NotifyClassChanged(ClassType classFrom, ClassType classTo, size_t level);
        void NotifyAbilityValueChanged(AbilityType ability);
        void NotifyAbilityTomeChanged(AbilityType ability);
        void NotifyRaceChanged(RaceType race);
        void NotifyFeatTrained(const std::string & featName);
        void NotifyFeatRevoked(const std::string & featName);
        void NotifyFeatEffect(const std::string & featName, const Effect & effect);
        void NotifyFeatEffectRevoked(const std::string & featName, const Effect & effect);
        void NotifyNewStance(const Stance & stance);
        void NotifyRevokeStance(const Stance & stance);
        void NotifyStanceActivated(const std::string & name);
        void NotifyStanceDeactivated(const std::string & name);
        void NotifyFatePointsChanged();
        void NotifyEpicCompletionistChanged();
        void NotifyAvailableTwistsChanged();
        void NotifyItemEffect(const std::string & itemName, const Effect & effect);
        void NotifyItemEffectRevoked(const std::string & itemName, const Effect & effect);
        void NotifyGearChanged(InventorySlotType slot);

        void NotifyAllLoadedEffects();
        void NotifyAllEnhancementEffects();
        void NotifyAllReaperEnhancementEffects();
        void NotifyAllDestinyEffects();
        void NotifyAllTwistEffects();
        void NotifyEnhancementEffect(const std::string & enhancementName,  const Effect & effect, size_t ranks);
        void NotifyEnhancementEffectRevoked(const std::string & enhancementName, const Effect & effect, size_t ranks);
        void NotifyEnhancementTrained(const std::string & enhancementName, const std::string & selection, bool isTier5, bool bActiveTree);
        void NotifyEnhancementRevoked(const std::string & enhancementName, const std::string & selection, bool isTier5, bool bActiveTree);
        void NotifyEnhancementTreeReset();
        void NotifyActionPointsChanged();
        void NotifyAPSpentInTreeChanged(const std::string & treeName);
        void NotifySpellTrained(const TrainedSpell & spell);
        void NotifySpellRevoked(const TrainedSpell & spell);
        void NotifyGrantedFeatsChanged();

        // XML
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Character_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_ENUM(_, AlignmentType, Alignment, Alignment_LawfulGood, alignmentTypeMap) \
                DL_ENUM(_, RaceType, Race, Race_Human, raceTypeMap) \
                DL_OBJECT(_, AbilitySpend, BuildPoints) \
                DL_SIMPLE(_, size_t, GuildLevel, 0) \
                DL_FLAG(_, ApplyGuildBuffs) \
                DL_SIMPLE(_, size_t, StrTome, 0) \
                DL_SIMPLE(_, size_t, DexTome, 0) \
                DL_SIMPLE(_, size_t, ConTome, 0) \
                DL_SIMPLE(_, size_t, IntTome, 0) \
                DL_SIMPLE(_, size_t, WisTome, 0) \
                DL_SIMPLE(_, size_t, ChaTome, 0) \
                DL_SIMPLE(_, size_t, TomeOfFate, 0) \
                DL_OBJECT(_, SkillTomes, Tomes) \
                DL_OBJECT(_, FeatsListObject, SpecialFeats) \
                DL_OBJECT(_, ActiveStances, Stances) \
                DL_ENUM(_, AbilityType, Level4, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level8, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level12, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level16, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level20, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level24, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level28, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, ClassType, Class1, Class_Unknown, classTypeMap) \
                DL_ENUM(_, ClassType, Class2, Class_Unknown, classTypeMap) \
                DL_ENUM(_, ClassType, Class3, Class_Unknown, classTypeMap) \
                DL_OBJECT(_, SelectedEnhancementTrees, SelectedTrees) \
                DL_OPTIONAL_STRING(_, Tier5Tree) \
                DL_OBJECT_LIST(_, LevelTraining, Levels) \
                DL_OBJECT_VECTOR(_, TrainedSpell, TrainedSpells) \
                DL_OBJECT_LIST(_, EnhancementSpendInTree, EnhancementTreeSpend) \
                DL_OBJECT_LIST(_, ReaperSpendInTree, ReaperTreeSpend) \
                DL_STRING(_, ActiveEpicDestiny) \
                DL_FLAG(_, EpicCompletionist) \
                DL_SIMPLE(_, size_t, FatePoints, 0) \
                DL_OBJECT_LIST(_, EpicDestinySpendInTree, EpicDestinyTreeSpend) \
                DL_OBJECT_LIST(_, TwistOfFate, Twists) \
                DL_STRING(_, ActiveGear) \
                DL_OBJECT_LIST(_, EquippedGear, GearSetups)

        DL_DECLARE_ACCESS(Character_PROPERTIES)
        DL_DECLARE_VARIABLES(Character_PROPERTIES)
    private:
        void UpdateFeats();
        void UpdateFeats(size_t level, std::list<TrainedFeat> * allFeats);
        void ApplyFeatEffects(const Feat & feat);
        void RevokeFeatEffects(const Feat & feat);
        void AutoTrainSingleSelectionFeats();
        bool NotPresentInEarlierLevel(size_t level, TrainableFeatTypes type) const;
        void VerifyTrainedFeats();
        void ApplyEnhancementEffects(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                size_t ranks);
        void RevokeEnhancementEffects(const std::string & name, size_t ranks, const std::list<Effect> & effects);
        EnhancementSpendInTree * Enhancement_FindTree(const std::string & treeName);
        ReaperSpendInTree * Reaper_FindTree(const std::string & treeName);
        EpicDestinySpendInTree * EpicDestiny_FindTree(const std::string & treeName);
        std::string GetEnhancementName(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection);
        std::list<Effect> GetEnhancementEffects(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection);
        void ApplyAllEffects(
                const std::string & treename,
                const std::list<TrainedEnhancement> & enhancements);
        void RevokeAllEffects(
                const std::string & treename,
                const std::list<TrainedEnhancement> & enhancements);
        void DetermineEpicCompletionist();
        void CountBonusAP();
        void DetermineFatePoints();
        void RevokeGearEffects();
        void ApplyGearEffects();
        void ApplyGuildBuffs();
        void AddGrantedFeat(const std::string & featName);
        void RevokeGrantedFeat(const std::string & featName);
        void KeepGrantedFeatsUpToDate();
        void UpdateSkillPoints();
        void UpdateSkillPoints(size_t level);
        void UpdateWeaponStances();
        void UpdateArmorStances();
        void UpdateShieldStances();
        void UpdateCenteredStance();

        // CharacterObserver
        // we need to track all sources from which a granted feat can come from
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;

        CDDOCPDoc * m_pDocument;
        int m_bonusActionPoints;
        int m_racialTreeSpend;
        int m_otherTreeSpend;
        size_t m_previousGuildLevel;
        // we track the granted feats from all sources on the character
        // but this information is not saved. Note that granted feats cannot
        // be used as a requirement met for a feat you want to train.
        std::list<TrainedFeat> m_grantedFeats;
        std::list<bool> m_grantedNotifyState;

        friend class CDDOCPDoc;
        friend class CEnhancementTreeDialog;
        friend class CDestinyTreeDialog;
};
