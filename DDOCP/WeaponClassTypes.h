// WeaponClassTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// weapon class types
enum WeaponClassType
{
    WeaponClass_Unknown = 0,
    WeaponClass_Thrown,
    WeaponClass_Unarmed,
    WeaponClass_OneHanded,
    WeaponClass_Ranged,
    WeaponClass_TwoHanded,
    WeaponClass_Bows,
    WeaponClass_Crossbows,
    WeaponClass_ReapeatingCrossbows,
    WeaponClass_Melee,
    WeaponClass_Druidic,
    WeaponClass_HeavyBlades,
    WeaponClass_LightBlades,
    WeaponClass_PicksAndHammers,
    WeaponClass_MacesAndClubs,
    WeaponClass_MartialArts,
    WeaponClass_Axe,
    WeaponClass_Light,
};
const XmlLib::enumMapEntry<WeaponClassType> weaponClassTypeMap[] =
{
    {WeaponClass_Unknown, L"Unknown"},
    {WeaponClass_Thrown, L"Thrown"},
    {WeaponClass_Unarmed, L"Unarmed"},
    {WeaponClass_OneHanded, L"OneHanded"},
    {WeaponClass_Ranged, L"Ranged"},
    {WeaponClass_TwoHanded, L"TwoHanded"},
    {WeaponClass_Bows, L"Bows"},
    {WeaponClass_Crossbows, L"Crossbows"},
    {WeaponClass_ReapeatingCrossbows, L"RepeatingCrossbows"},
    {WeaponClass_Melee, L"Melee"},
    {WeaponClass_Druidic, L"Druidic"},
    {WeaponClass_HeavyBlades, L"HeavyBlades"},
    {WeaponClass_LightBlades, L"LightBlades"},
    {WeaponClass_PicksAndHammers, L"PicksAndHammers"},
    {WeaponClass_MacesAndClubs, L"MacesAndClubs"},
    {WeaponClass_MartialArts, L"MartialArts"},
    {WeaponClass_Axe, L"Axe"},
    {WeaponClass_Light, L"Light"},
    {WeaponClassType(0), NULL}
};
