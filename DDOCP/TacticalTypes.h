// TacticalTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum TacticalType
{
    Tactical_Unknown = 0,
    Tactical_Assassinate,
    Tactical_Trip,
    Tactical_Stun,
    Tactical_Sunder,
    Tactical_StunningShield,
    Tactical_General,
    Tactical_Wands,
    Tactical_Fear,
    Tactical_InnateAttack,
    Tactical_BreathWeapon,
    Tactical_Poison,
    Tactical_RuneArm,
};
const XmlLib::enumMapEntry<TacticalType> tacticalTypeMap[] =
{
    {Tactical_Unknown, L"Unknown"},
    {Tactical_Assassinate,    L"Assassinate"},
    {Tactical_Trip,           L"Trip"},
    {Tactical_Stun,           L"Stun"},
    {Tactical_Sunder,         L"Sunder"},
    {Tactical_StunningShield, L"Stunning Shield"},
    {Tactical_General,        L"General"},
    {Tactical_Wands,          L"Wands"},
    {Tactical_Fear,           L"Fear"},
    {Tactical_InnateAttack,   L"Innate Attack"},
    {Tactical_BreathWeapon,   L"Breath Weapon"},
    {Tactical_Poison,         L"Poison"},
    {Tactical_RuneArm,        L"Rune Arm"},
    {TacticalType(0), NULL}
};
