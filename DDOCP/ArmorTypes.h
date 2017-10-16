// ArmorTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum ArmorType
{
    Armor_Unknown = 0,
    // these are not alphabetical, but in the order they appear in the UI
    Armor_Cloth,
    Armor_Light,
    Armor_Medium,
    Armor_Heavy,
    Armor_Docent,
};
const XmlLib::enumMapEntry<ArmorType> armorTypeMap[] =
{
    {Armor_Unknown, L"Unknown"},
    {Armor_Cloth, L"Cloth"},
    {Armor_Light, L"Light"},
    {Armor_Medium, L"Medium"},
    {Armor_Heavy, L"Heavy"},
    {Armor_Docent, L"Docent"},
    {ArmorType(0), NULL}
};
