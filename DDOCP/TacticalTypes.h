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
    Tactical_All,
    Tactical_Trip,
    Tactical_StunningBlow,
    Tactical_Sunder,
    Tactical_StunningShield,
};
const XmlLib::enumMapEntry<TacticalType> tacticalTypeMap[] =
{
    {Tactical_Unknown, L"Unknown"},
    {Tactical_All, L"All"},
    {Tactical_Trip, L"Trip"},
    {Tactical_StunningBlow, L"Stunning Blow"},
    {Tactical_Sunder, L"Sunder"},
    {Tactical_StunningShield, L"Stunning Shield"},
    {TacticalType(0), NULL}
};
