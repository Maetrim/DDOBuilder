// Item.h
//
// An XML object wrapper that holds information on an item that can be equipped
#pragma once
#include "XmlLib\DLMacros.h"
#include "ArmorTypes.h"
#include "Augment.h"
#include "DC.h"
#include "Dice.h"
#include "Effect.h"
#include "EquipmentSlot.h"
#include "InventorySlotTypes.h"
#include "ItemAugment.h"
#include "Requirements.h"
#include "SentientJewel.h"
#include "SlotUpgrade.h"
#include "ItemSpecificEffects.h"

class Item :
    public XmlLib::SaxContentElement
{
    public:
        Item(void);
        Item(const XmlLib::SaxString & objectName);
        void Write(XmlLib::SaxWriter * writer) const;

        bool CanEquipToSlot(InventorySlotType slot) const;
        void AddImage(CImageList * pIL) const;
        void VerifyObject() const;
        bool ContainsSearchText(const std::string & searchText) const;

        void CopyUserSetValues(const Item & original);
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        bool SearchForText(std::string source, const std::string & find) const;

        #define Item_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Icon) \
                DL_STRING(_, Description) \
                DL_OPTIONAL_STRING(_, DropLocation) \
                DL_SIMPLE(_, size_t, MinLevel, 0) \
                DL_STRING_LIST(_, EffectDescription) \
                DL_STRING_LIST(_, DRBypass) \
                DL_OPTIONAL_OBJECT(_, Requirements, RequirementsToUse) \
                DL_OPTIONAL_OBJECT(_, Dice, DamageDice) \
                DL_OPTIONAL_OBJECT(_, ItemSpecificEffects, ItemEffects) \
                DL_OPTIONAL_SIMPLE(_, size_t, CriticalMultiplier, 0) \
                DL_OPTIONAL_ENUM(_, WeaponType, Weapon, Weapon_Unknown, weaponTypeMap) \
                DL_OPTIONAL_ENUM(_, WeaponDamageType, DamageType, WeaponDamage_Unknown, weaponDamageTypeMap) \
                DL_OPTIONAL_ENUM(_, AbilityType, AttackModifier, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, AbilityType, DamageModifier, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, ArmorType, Armor, Armor_Unknown, armorTypeMap) \
                DL_OBJECT(_, EquipmentSlot, Slots) \
                DL_OBJECT_VECTOR(_, Effect, Effects) \
                DL_OBJECT_VECTOR(_, ItemAugment, Augments) \
                DL_OBJECT_VECTOR(_, SlotUpgrade, SlotUpgrades) \
                DL_FLAG(_, IsGreensteel) \
                DL_FLAG(_, MinorArtifact) \
                DL_OBJECT_LIST(_, DC, EffectDC) \
                DL_OPTIONAL_OBJECT(_, SentientJewel, SentientIntelligence)

        DL_DECLARE_ACCESS(Item_PROPERTIES)
        DL_DECLARE_VARIABLES(Item_PROPERTIES)

        friend class CItemSelectDialog;
        friend class CFindGearDialog;
        friend class EquippedGear;
};
