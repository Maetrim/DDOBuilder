// EquipmentSlot.cpp
//
#include "StdAfx.h"
#include "EquipmentSlot.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EquipmentSlot

namespace
{
    const wchar_t f_saxElementName[] = L"EquipmentSlot";
    DL_DEFINE_NAMES(EquipmentSlot_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EquipmentSlot::EquipmentSlot() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(EquipmentSlot_PROPERTIES)
}

DL_DEFINE_ACCESS(EquipmentSlot_PROPERTIES)

XmlLib::SaxContentElementInterface * EquipmentSlot::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EquipmentSlot_PROPERTIES)

    return subHandler;
}

void EquipmentSlot::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EquipmentSlot_PROPERTIES)
}

void EquipmentSlot::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EquipmentSlot_PROPERTIES)
    writer->EndElement();
}

bool EquipmentSlot::HasSlot(InventorySlotType slot) const
{
    bool bHas = false;

    switch (slot)
    {
        case Inventory_Arrows:      bHas = HasArrow(); break;
        case Inventory_Armor:       bHas = HasArmor(); break;
        case Inventory_Belt:        bHas = HasBelt(); break;
        case Inventory_Boots:       bHas = HasBoots(); break;
        case Inventory_Bracers:     bHas = HasBracers(); break;
        case Inventory_Cloak:       bHas = HasCloak(); break;
        case Inventory_Gloves:      bHas = HasGloves(); break;
        case Inventory_Goggles:     bHas = HasGoggles(); break;
        case Inventory_Helmet:      bHas = HasHelmet(); break;
        case Inventory_Necklace:    bHas = HasNecklace(); break;
        case Inventory_Quiver:      bHas = HasQuiver(); break;
        case Inventory_Ring1:       bHas = HasRing(); break;
        case Inventory_Ring2:       bHas = HasRing(); break;
        case Inventory_Trinket:     bHas = HasTrinket(); break;
        case Inventory_Weapon1:     bHas = HasWeapon1(); break;
        case Inventory_Weapon2:     bHas = HasWeapon2(); break;
        default: break;
    }
    return bHas;
}
