// InventorySlotTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum InventorySlotType
{
    Inventory_Unknown = 0,

    Inventory_Arrows,
    Inventory_Armor,
    Inventory_Belt,
    Inventory_Boots,
    Inventory_Bracers,
    Inventory_Cloak,
    Inventory_Gloves,
    Inventory_Goggles,
    Inventory_Helmet,
    Inventory_Necklace,
    Inventory_Quiver,
    Inventory_Ring1,
    Inventory_Ring2,
    Inventory_Trinket,
    Inventory_Weapon1,
    Inventory_Weapon2,
    Inventory_Trinket2,
    Inventory_Trinket3,

    Inventory_Count,
    Inventory_FindItems,
};
const XmlLib::enumMapEntry<InventorySlotType> InventorySlotTypeMap[] =
{
    {Inventory_Unknown, L"Unknown"},
    {Inventory_Arrows, L"Arrows"},
    {Inventory_Armor, L"Armor"},
    {Inventory_Belt, L"Belt"},
    {Inventory_Boots, L"Boots"},
    {Inventory_Bracers, L"Bracers"},
    {Inventory_Cloak, L"Cloak"},
    {Inventory_Gloves, L"Gloves"},
    {Inventory_Goggles, L"Goggles"},
    {Inventory_Helmet, L"Helmet"},
    {Inventory_Necklace, L"Necklace"},
    {Inventory_Quiver, L"Quiver"},
    {Inventory_Ring1, L"Ring1"},
    {Inventory_Ring2, L"Ring2"},
    {Inventory_Trinket, L"Trinket"},
    {Inventory_Weapon1, L"Weapon1"},
    {Inventory_Weapon2, L"Weapon2"},
    {Inventory_Trinket2, L"Trinket2"},
    {Inventory_Trinket3, L"Trinket3"},
    {InventorySlotType(0), NULL}
};
