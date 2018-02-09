// EquippedGear.cpp
//
#include "StdAfx.h"
#include "EquippedGear.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EquippedGear

namespace
{
    const wchar_t f_saxElementName[] = L"EquippedGear";
    DL_DEFINE_NAMES(EquippedGear_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EquippedGear::EquippedGear(const std::string & name) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_Helmet(L"Helmet"),
    m_Necklace(L"Necklace"),
    m_Trinket(L"Trinket"),
    m_Cloak(L"Cloak"),
    m_Belt(L"Belt"),
    m_Goggles(L"Goggles"),
    m_Gloves(L"Gloves"),
    m_Boots(L"Boots"),
    m_Bracers(L"Bracers"),
    m_Armor(L"Armor"),
    m_MainHand(L"MainHand"),
    m_OffHand(L"OffHand"),
    m_Quiver(L"Quiver"),
    m_Arrow(L"Arrow"),
    m_Ring1(L"Ring1"),
    m_Ring2(L"Ring2")
{
    DL_INIT(EquippedGear_PROPERTIES)
    m_Name = name;
}

DL_DEFINE_ACCESS(EquippedGear_PROPERTIES)

XmlLib::SaxContentElementInterface * EquippedGear::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EquippedGear_PROPERTIES)

    return subHandler;
}

void EquippedGear::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EquippedGear_PROPERTIES)
}

void EquippedGear::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EquippedGear_PROPERTIES)
    writer->EndElement();
}

bool EquippedGear::HasItemInSlot(InventorySlotType slot) const
{
    switch (slot)
    {
    case Inventory_Arrows:  return HasArrow();
    case Inventory_Armor:   return HasArmor();
    case Inventory_Belt:    return HasBelt();
    case Inventory_Boots:   return HasBoots();
    case Inventory_Bracers: return HasBracers();
    case Inventory_Cloak:   return HasCloak();
    case Inventory_Gloves:  return HasGloves();
    case Inventory_Goggles: return HasGoggles();
    case Inventory_Helmet:  return HasHelmet();
    case Inventory_Necklace:return HasNecklace();
    case Inventory_Quiver:  return HasQuiver();
    case Inventory_Ring1:   return HasRing1();
    case Inventory_Ring2:   return HasRing2();
    case Inventory_Trinket: return HasTrinket();
    case Inventory_Weapon1: return HasMainHand();
    case Inventory_Weapon2: return HasOffHand();
    }
    return false;
}

Item EquippedGear::ItemInSlot(InventorySlotType slot) const
{
    Item noItem;
    switch (slot)
    {
    case Inventory_Arrows:
        if (HasArrow())
        {
            return Arrow();
        }
        break;
    case Inventory_Armor:
        if (HasArmor())
        {
            return Armor();
        }
        break;
    case Inventory_Belt:
        if (HasBelt())
        {
            return Belt();
        }
        break;
    case Inventory_Boots:
        if (HasBoots())
        {
            return Boots();
        }
        break;
    case Inventory_Bracers:
        if (HasBracers())
        {
            return Bracers();
        }
        break;
    case Inventory_Cloak:
        if (HasCloak())
        {
            return Cloak();
        }
        break;
    case Inventory_Gloves:
        if (HasGloves())
        {
            return Gloves();
        }
        break;
    case Inventory_Goggles:
        if (HasGoggles())
        {
            return Goggles();
        }
        break;
    case Inventory_Helmet:
        if (HasHelmet())
        {
            return Helmet();
        }
        break;
    case Inventory_Necklace:
        if (HasNecklace())
        {
            return Necklace();
        }
        break;
    case Inventory_Quiver:
        if (HasQuiver())
        {
            return Quiver();
        }
        break;
    case Inventory_Ring1:
        if (HasRing1())
        {
            return Ring1();
        }
        break;
    case Inventory_Ring2:
        if (HasRing2())
        {
            return Ring2();
        }
        break;
    case Inventory_Trinket:
        if (HasTrinket())
        {
            return Trinket();
        }
        break;
    case Inventory_Weapon1:
        if (HasMainHand())
        {
            return MainHand();
        }
        break;
    case Inventory_Weapon2:
        if (HasOffHand())
        {
            return OffHand();
        }
        break;
    }
    return noItem;
}

void EquippedGear::SetItem(InventorySlotType slot, const Item & item)
{
    switch (slot)
    {
    case Inventory_Arrows:  Set_Arrow(item); break;
    case Inventory_Armor:   Set_Armor(item); break;
    case Inventory_Belt:    Set_Belt(item); break;
    case Inventory_Boots:   Set_Boots(item); break;
    case Inventory_Bracers: Set_Bracers(item); break;
    case Inventory_Cloak:   Set_Cloak(item); break;
    case Inventory_Gloves:  Set_Gloves(item); break;
    case Inventory_Goggles: Set_Goggles(item); break;
    case Inventory_Helmet:  Set_Helmet(item); break;
    case Inventory_Necklace: Set_Necklace(item); break;
    case Inventory_Quiver:  Set_Quiver(item); break;
    case Inventory_Ring1:   Set_Ring1(item); break;
    case Inventory_Ring2:   Set_Ring2(item); break;
    case Inventory_Trinket: Set_Trinket(item); break;
    case Inventory_Weapon1: Set_MainHand(item); break;
    case Inventory_Weapon2: Set_OffHand(item); break;
    default: ASSERT(FALSE); break;
    }
    if (slot == Inventory_Weapon1
            && !ItemInSlot(Inventory_Weapon1).CanEquipToSlot(Inventory_Weapon2))
    {
        // item in this slot now stops an item in weapon slot 2
        ClearItem(Inventory_Weapon2);
    }
}

void EquippedGear::ClearItem(InventorySlotType slot)
{
    switch (slot)
    {
    case Inventory_Arrows:  Clear_Arrow(); break;
    case Inventory_Armor:   Clear_Armor(); break;
    case Inventory_Belt:    Clear_Belt(); break;
    case Inventory_Boots:   Clear_Boots(); break;
    case Inventory_Bracers: Clear_Bracers(); break;
    case Inventory_Cloak:   Clear_Cloak(); break;
    case Inventory_Gloves:  Clear_Gloves(); break;
    case Inventory_Goggles: Clear_Goggles(); break;
    case Inventory_Helmet:  Clear_Helmet(); break;
    case Inventory_Necklace: Clear_Necklace(); break;
    case Inventory_Quiver:  Clear_Quiver(); break;
    case Inventory_Ring1:   Clear_Ring1(); break;
    case Inventory_Ring2:   Clear_Ring2(); break;
    case Inventory_Trinket: Clear_Trinket(); break;
    case Inventory_Weapon1: Clear_MainHand(); break;
    case Inventory_Weapon2: Clear_OffHand(); break;
    default: ASSERT(FALSE); break;
    }
}

