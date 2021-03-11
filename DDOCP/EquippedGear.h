// EquippedGear.h
//
// An XML object wrapper that holds information on all item equipped
#pragma once
#include "XmlLib\DLMacros.h"
#include "Item.h"
#include "InventorySlotTypes.h"

class Character;

class EquippedGear :
    public XmlLib::SaxContentElement
{
    public:
        EquippedGear(const std::string & name = "");
        void Write(XmlLib::SaxWriter * writer) const;

        bool HasItemInSlot(InventorySlotType slot) const;
        Item ItemInSlot(InventorySlotType slot) const;
        bool IsSlotRestricted(InventorySlotType slot, Character * pChar) const;
        void SetItem(InventorySlotType slot,
                Character * pCharacter,
                const Item & item);
        void ClearItem(InventorySlotType slot);

        void SetNumFiligrees(size_t count);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define EquippedGear_PROPERTIES(_) \
                DL_STRING (_, Name) \
                DL_OPTIONAL_OBJECT(_, Item, Helmet) \
                DL_OPTIONAL_OBJECT(_, Item, Necklace) \
                DL_OPTIONAL_OBJECT(_, Item, Trinket) \
                DL_OPTIONAL_OBJECT(_, Item, Cloak) \
                DL_OPTIONAL_OBJECT(_, Item, Belt) \
                DL_OPTIONAL_OBJECT(_, Item, Goggles) \
                DL_OPTIONAL_OBJECT(_, Item, Gloves) \
                DL_OPTIONAL_OBJECT(_, Item, Boots) \
                DL_OPTIONAL_OBJECT(_, Item, Bracers) \
                DL_OPTIONAL_OBJECT(_, Item, Armor) \
                DL_OPTIONAL_OBJECT(_, Item, MainHand) \
                DL_OPTIONAL_OBJECT(_, Item, OffHand) \
                DL_OPTIONAL_OBJECT(_, Item, Quiver) \
                DL_OPTIONAL_OBJECT(_, Item, Arrow) \
                DL_OPTIONAL_OBJECT(_, Item, Ring1) \
                DL_OPTIONAL_OBJECT(_, Item, Ring2) \
                DL_OBJECT(_, SentientJewel, SentientIntelligence)

        DL_DECLARE_ACCESS(EquippedGear_PROPERTIES)
        DL_DECLARE_VARIABLES(EquippedGear_PROPERTIES)

        friend class CEquipmentView;
        friend class EquippedGear;
        friend class CInventoryDialog;
};
