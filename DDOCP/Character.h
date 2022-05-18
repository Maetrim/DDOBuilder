// Character.h
//
#pragma once
#include "XmlLib\DLMacros.h"

#include "AbilitySpend.h"
#include "ActiveStances.h"
#include "AlignmentTypes.h"
#include "DestinySpendInTree.h"
#include "EnhancementSpendInTree.h"
#include "EquippedGear.h"
#include "SelectedEnhancementTrees.h"
#include "SelectedDestinyTrees.h"
#include "FeatsListObject.h"
#include "LevelTraining.h"
#include "ObserverSubject.h"
#include "RaceTypes.h"
#include "ReaperSpendInTree.h"
#include "SkillTomes.h"
#include "SpellListAddition.h"
#include "Stance.h"
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
        virtual void UpdateEnhancementTreeOrderChanged(Character * charData) {};
        virtual void UpdateActionPointsChanged(Character * charData) {};
        virtual void UpdateAPSpentInTreeChanged(Character * charData, const std::string & treeName) {};
        virtual void UpdateSpellTrained(Character * charData, const TrainedSpell & spell) {};
        virtual void UpdateSpellRevoked(Character * charData, const TrainedSpell & spell) {};
        virtual void UpdateFatePointsChanged(Character * charData) {};
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) {};
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) {};
        virtual void UpdateGrantedFeatsChanged(Character * charData) {};
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) {};
        virtual void UpdateNewDC(Character * charData, const DC & dc) {};
        virtual void UpdateRevokeDC(Character * charData, const DC & dc) {};
};

class Character :
    public XmlLib::SaxContentElement,
    public Subject<CharacterObserver>,
    public CharacterObserver        // we observes ourselves
{
    public:
        explicit Character();
        Character(CDDOCPDoc * pDocument);
        void Write(XmlLib::SaxWriter * writer) const;

        void SetLamanniaMode(bool bLamanniaPreview);
        size_t MaxLevel() const;

        void JustLoaded();  // called when file loaded and needs to be checked / updated
        void NowActive();   // called when active MDI changed

        void ResetBuild();

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
        void SwapClasses(size_t level1, size_t level2);
        bool RevokeClass(ClassType type);
        ClassType Class(size_t index) const;

        // skill points
        bool IsClassSkill(SkillType skill, size_t level) const;
        void SpendSkillPoint(size_t level, SkillType skill, bool suppressUpdate = false);
        void RevokeSkillPoint(size_t level, SkillType skill, bool suppressUpdate);
        double MaxSkillForLevel(SkillType skill, size_t level) const;
        size_t SpentAtLevel(SkillType skill, size_t level) const; // level is 0 based
        double SkillAtLevel(SkillType skill, size_t level, bool includeTome) const; // level is 0 based
        void SkillsUpdated();

        // feats
        bool IsFeatTrained(const std::string & featName, bool includeGrantedFeats = false) const;
        size_t GetSpecialFeatTrainedCount(const std::string & featName) const;
        TrainedFeat GetTrainedFeat(size_t level, TrainableFeatTypes type) const;
        void TrainSpecialFeat(const std::string & featName, TrainableFeatTypes type);
        void RevokeSpecialFeat(const std::string & featName, TrainableFeatTypes type);
        void SetAbilityLevelUp(size_t level, AbilityType ability);
        void TrainFeat(const std::string & featName, TrainableFeatTypes type, size_t level, bool autoTrained = false, bool suppressVerify = false);
        void TrainAlternateFeat(const std::string & featName, TrainableFeatTypes type, size_t level);
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
        bool ShowUnavailable() const;
        bool ShowEpicOnly() const;
        void ToggleShowEpicOnly();
        void ToggleShowUnavailable();
        bool ShowIgnoredItems() const;
        void ToggleShowIgnoredItems();
        void AddToIgnoreList(const std::string & name);
        void RemoveFromIgnoreList(const std::string & name);
        bool IsInIgnoreList(const std::string & name) const;

        // spells
        std::list<TrainedSpell> TrainedSpells(ClassType classType, size_t level) const;
        void TrainSpell(ClassType classType, size_t level, const std::string & spellName);
        void RevokeSpell(ClassType classType, size_t level, const std::string & spellName);
        bool IsSpellTrained(const std::string & spellName) const;
        void ApplySpellEffects(const std::string & spellName, size_t castingLevel);
        void RevokeSpellEffects(const std::string & spellName, size_t castingLevel);
        std::list<TrainedSpell> FixedSpells(ClassType classType, size_t level) const;
        void AddSpellListAddition(const Effect& effect);
        void RevokeSpellListAddition(const Effect& effect);
        bool IsSpellInSpellListAdditionList(ClassType ct, size_t spellLevel, const std::string& spellName) const;

        // stances
        void ActivateStance(const Stance & stance);
        void DeactivateStance(const Stance & stance);
        bool IsStanceActive(const std::string & name, WeaponType wt = Weapon_Unknown) const;
        void StanceSliderChanged();

        // checks all possible tree types
        int TotalPointsAvailable(const std::string & treeName, TreeType type) const;
        int AvailableActionPoints(const std::string & treeName, TreeType type) const;
        int APSpentInTree(const std::string & treeName);
        const TrainedEnhancement * IsTrained(
                const std::string & enhancementName,
                const std::string & selection,
                TreeType type) const;

        const SpendInTree* FindSpendInTree(const std::string& treeName) const;
        SpendInTree* FindSpendInTree(const std::string& treeName);

        // enhancement support
        const EnhancementTree & FindTree(const std::string & treeName) const;
        bool IsEnhancementTrained(
                const std::string & enhancementName,
                const std::string & selection,
                TreeType type) const;
        std::list<TrainedEnhancement> CurrentEnhancements() const;
        void Enhancement_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                const std::vector<size_t>& cost);
        void Enhancement_RevokeEnhancement(
                const std::string & treeName,
                const std::string & revokedEnhancement);
        void Enhancement_ResetEnhancementTree(std::string treeName);
        void Enhancement_SetSelectedTrees(const SelectedEnhancementTrees & trees);
        bool Enhancement_IsTreeTrained(const std::string & tree) const;
        void Enhancement_SwapTrees(const std::string & tree1, const std::string & tree2);

        int AvailableActionPoints() const;
        int BonusRacialActionPoints() const;
        int BonusUniversalActionPoints() const;

        // reaper enhancement support
        void Reaper_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                const std::vector<size_t>& cost);
        void Reaper_RevokeEnhancement(
                const std::string& treeName,
                const std::string & revokedEnhancement);
        void Reaper_ResetEnhancementTree(std::string treeName);

        // U51 Epic destiny support
        int U51Destiny_AvailableDestinyPoints() const;
        void U51Destiny_TrainEnhancement(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                const std::vector<size_t>& cost);
        void U51Destiny_RevokeEnhancement(
                const std::string& treeName,
                const std::string & revokedEnhancement);
        void U51Destiny_ResetEnhancementTree(std::string treeName);
        void U51Destiny_SetSelectedTrees(const SelectedDestinyTrees & trees);
        bool U51Destiny_IsTreeTrained(const std::string & tree) const;
        void U51Destiny_SwapTrees(const std::string & tree1, const std::string & tree2);

        // gear support
        void AddGearSet(const EquippedGear & gear);
        void DeleteGearSet(const std::string & name);
        bool DoesGearSetExist(const std::string & name) const;
        void SetActiveGearSet(const std::string & name);
        EquippedGear GetGearSet(const std::string & name) const;
        EquippedGear ActiveGearSet() const;
        void UpdateActiveGearSet(const EquippedGear & newGear);
        void SetGear(const std::string & name, InventorySlotType slot, const Item & item);
        void ClearGearInSlot(const std::string & name, InventorySlotType slot);
        bool LightWeaponInOffHand() const;
        bool IsFocusWeapon(WeaponType wt) const;
        void SetNumFiligrees(size_t count);
        void ApplyFiligree(const std::string & filigree, size_t si, bool bArtifact);
        void RevokeFiligree(const std::string & filigree, size_t si, bool bArtifact);

        // guild support
        void ToggleApplyGuildBuffs();
        void SetGuildLevel(size_t level);

        // general support functions
        std::string GetBuildDescription() const;

        // self and party buffs
        const std::list<std::string> EnabledSelfAndPartyBuffs() const;
        void EnableSelfAndPartyBuff(const std::string & name);
        void DisableSelfAndPartyBuff(const std::string & name);
        void NotifyAllSelfAndPartyBuffs();

        // Notes support
        void SetNotes(const std::string & notes);

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
        void NotifyItemEffect(const std::string & itemName, const Effect & effect);
        void NotifyItemEffectRevoked(const std::string & itemName, const Effect & effect);
        void NotifyGearChanged(InventorySlotType slot);

        void NotifyAllLoadedEffects();
        void NotifyAllEnhancementEffects();
        void NotifyAllReaperEnhancementEffects();
        void NotifyAllDestinyEffects();
        void NotifyAllU51DestinyEffects();
        void NotifyEnhancementEffect(const std::string & enhancementName,  const Effect & effect, size_t ranks);
        void NotifyEnhancementEffectRevoked(const std::string & enhancementName, const Effect & effect, size_t ranks);
        void NotifyEnhancementTrained(const std::string & enhancementName, const std::string & selection, bool isTier5, bool bActiveTree);
        void NotifyEnhancementRevoked(const std::string & enhancementName, const std::string & selection, bool isTier5, bool bActiveTree);
        void NotifyEnhancementTreeReset();
        void NotifyEnhancementTreeOrderChanged();
        void NotifyActionPointsChanged();
        void NotifyAPSpentInTreeChanged(const std::string & treeName);
        void NotifySpellTrained(const TrainedSpell & spell);
        void NotifySpellRevoked(const TrainedSpell & spell);
        void NotifyGrantedFeatsChanged();
        void NotifyNewDC(const DC & dc);
        void NotifyRevokeDC(const DC & dc);

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
                DL_ENUM(_, AbilityType, Level32, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level36, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, AbilityType, Level40, Ability_Strength, abilityTypeMap) \
                DL_ENUM(_, ClassType, Class1, Class_Unknown, classTypeMap) \
                DL_ENUM(_, ClassType, Class2, Class_Unknown, classTypeMap) \
                DL_ENUM(_, ClassType, Class3, Class_Unknown, classTypeMap) \
                DL_OBJECT(_, SelectedEnhancementTrees, SelectedTrees) \
                DL_OBJECT(_, SelectedDestinyTrees, DestinyTrees) \
                DL_OPTIONAL_STRING(_, Tier5Tree) \
                DL_OPTIONAL_STRING(_, U51Destiny_Tier5Tree) \
                DL_OBJECT_LIST(_, LevelTraining, Levels) \
                DL_OBJECT_VECTOR(_, TrainedSpell, TrainedSpells) \
                DL_OBJECT_LIST(_, EnhancementSpendInTree, EnhancementTreeSpend) \
                DL_OBJECT_LIST(_, ReaperSpendInTree, ReaperTreeSpend) \
                DL_OBJECT_LIST(_, DestinySpendInTree, DestinyTreeSpend) \
                DL_SIMPLE(_, size_t, FatePoints, 0) \
                DL_STRING(_, ActiveGear) \
                DL_OBJECT_LIST(_, EquippedGear, GearSetups) \
                DL_STRING_LIST(_, SelfAndPartyBuffs) \
                DL_STRING(_, Notes) \
                DL_FLAG(_, LamanniaMode)

        DL_DECLARE_ACCESS(Character_PROPERTIES)
        DL_DECLARE_VARIABLES(Character_PROPERTIES)
    private:
        void UpdateSpells();
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
        void RevokeEnhancementEffects(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection,
                size_t ranks);
        EnhancementSpendInTree * Enhancement_FindTree(const std::string & treeName);
        ReaperSpendInTree * Reaper_FindTree(const std::string & treeName);
        DestinySpendInTree * U51Destiny_FindTree(const std::string & treeName);
        std::string GetEnhancementName(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection);
        std::list<Effect> GetEnhancementEffects(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection);
        std::list<DC> GetEnhancementDCs(
                const std::string & treeName,
                const std::string & enhancementName,
                const std::string & selection);
        void ApplyAllEffects(
                const std::string & treename,
                const std::list<TrainedEnhancement> & enhancements);
        void RevokeAllEffects(
                const std::string & treename,
                const std::list<TrainedEnhancement> & enhancements);
        void CountBonusRacialAP();
        void CountBonusUniversalAP();
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
        void UpdateGreensteelStances();
        void VerifyGear();

        void UpdateGearToLatestVersions();
        Item GetLatestVersionOfItem(const Item & original);

        void ApplySetBonus(const std::string & set, const std::string & name);
        void RevokeSetBonus(const std::string & set, const std::string & name);

        AlignmentType OverrideAlignment() const;

        // CharacterObserver
        // we need to track all sources from which a granted feat can come from
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;

        CDDOCPDoc * m_pDocument;
        int m_bonusRacialActionPoints;
        int m_bonusUniversalActionPoints;
        int m_bonusDestinyActionPoints;
        int m_destinyTreeSpend;
        int m_racialTreeSpend;
        int m_universalTreeSpend;
        int m_classTreeSpend;
        size_t m_previousGuildLevel;
        // we track the granted feats from all sources on the character
        // but this information is not saved. Note that granted feats cannot
        // be used as a requirement met for a feat you want to train.
        std::list<TrainedFeat> m_grantedFeats;
        std::list<bool> m_grantedNotifyState;
        std::vector<SpellListAddition> m_additionalSpells;

        bool m_bShowEpicOnly;
        bool m_bShowUnavailableFeats;
        bool m_bShowIgnoredItems;

        friend class CForumExportDlg;
        friend class CEnhancementTreeDialog;
        friend class CDestinyTreeDialog;
        friend class CStancesView;
};
