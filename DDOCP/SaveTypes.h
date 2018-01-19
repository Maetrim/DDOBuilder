// SaveTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum SaveType
{
    Save_Unknown = 0,
    Save_All,
    Save_Curse,
    Save_Disease,
    Save_Enchantment,
    Save_Fear,
    Save_Fortitude,
    Save_Illusion,
    Save_Magic,
    Save_Poison,
    Save_Reflex,
    Save_Spell,
    Save_Traps,
    Save_Will,
};
const XmlLib::enumMapEntry<SaveType> saveTypeMap[] =
{
    {Save_Unknown, L"Unknown"},
    {Save_All, L"All"},
    {Save_Curse, L"Curse"},
    {Save_Disease, L"Disease"},
    {Save_Enchantment, L"Enchantment"},
    {Save_Fear, L"Fear"},
    {Save_Fortitude, L"Fortitude"},
    {Save_Illusion, L"Illusion"},
    {Save_Magic, L"Magic"},
    {Save_Poison, L"Poison"},
    {Save_Reflex, L"Reflex"},
    {Save_Spell, L"Spell"},
    {Save_Traps, L"Traps"},
    {Save_Will, L"Will"},
    {SaveType(0), NULL}
};
