// Item.cpp
//
#include "StdAfx.h"
#include "Item.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Item

namespace
{
    const wchar_t f_saxElementName[] = L"Item";
    DL_DEFINE_NAMES(Item_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Item::Item() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Item_PROPERTIES)
}

Item::Item(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent)
{
    DL_INIT(Item_PROPERTIES)
}

DL_DEFINE_ACCESS(Item_PROPERTIES)

XmlLib::SaxContentElementInterface * Item::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Item_PROPERTIES)

    return subHandler;
}

void Item::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Item_PROPERTIES)
}

void Item::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Item_PROPERTIES)
    writer->EndElement();
}

bool Item::CanEquipToSlot(InventorySlotType slot) const
{
    bool canEquipToSlot = false;
    switch (slot)
    {
    case Inventory_Arrows:
        return m_Slots.HasArrow();
    case Inventory_Armor:
        return m_Slots.HasArmor();
    case Inventory_Belt:
        return m_Slots.HasBelt();
    case Inventory_Boots:
        return m_Slots.HasBoots();
    case Inventory_Bracers:
        return m_Slots.HasBracers();
    case Inventory_Cloak:
        return m_Slots.HasCloak();
    case Inventory_Gloves:
        return m_Slots.HasGloves();
    case Inventory_Goggles:
        return m_Slots.HasGoggles();
    case Inventory_Helmet:
        return m_Slots.HasHelmet();
    case Inventory_Necklace:
        return m_Slots.HasNecklace();
    case Inventory_Quiver:
        return m_Slots.HasQuiver();
    case Inventory_Ring1:
    case Inventory_Ring2:
        return m_Slots.HasRing();
    case Inventory_Trinket:
        return m_Slots.HasTrinket();
    case Inventory_Weapon1:
        return m_Slots.HasWeapon1();
    case Inventory_Weapon2:
        return m_Slots.HasWeapon2();
    }
    return canEquipToSlot;
}

void Item::AddImage(CImageList * pIL) const
{
    CImage image;
    HRESULT result = LoadImageFile(
            IT_item,
            m_Icon,
            &image);
    if (result == S_OK)
    {
        if (image.GetHeight() != 32
                || image.GetWidth() != 32)
        {
            ::OutputDebugString("Image file incorrect size - ");
            ::OutputDebugString(m_Icon.c_str());
            ::OutputDebugString("\n");
        }
        CBitmap bitmap;
        bitmap.Attach(image.Detach());
        int ret = pIL->Add(&bitmap, RGB(255, 128, 255));  // standard mask color (purple)
        ASSERT(ret >= 0);
    }
}
