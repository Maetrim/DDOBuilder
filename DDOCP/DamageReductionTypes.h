// DamageReductionTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// damage reduction types
enum DamageReductionType
{
    DR_Unknown = 0,
    DR_Adamantine,
    DR_Alignment,
    DR_Bludgeon,
    DR_Byeshk,
    DR_Chaotic,
    DR_Cold,
    DR_ColdIron,
    DR_Crystal,
    DR_Epic,
    DR_Evil,
    DR_Fire,
    DR_Force,
    DR_Good,
    DR_Lawful,
    DR_Light,
    DR_Magic,
    DR_Metalline,
    DR_Mithral,
    DR_Pierce,
    DR_Silver,
    DR_Slash,
    DR_All,
    DR_Percent,     // new for barbarian DR (U39)
};
const XmlLib::enumMapEntry<DamageReductionType> drTypeMap[] =
{
    {DR_Unknown, L"Unknown"},
    {DR_Adamantine, L"Adamantine"},
    {DR_Alignment, L"Alignment"},
    {DR_Bludgeon, L"Bludgeon"},
    {DR_Byeshk, L"Byeshk"},
    {DR_Chaotic, L"Chaotic"},
    {DR_Cold, L"Cold"},
    {DR_ColdIron, L"Cold Iron"},
    {DR_Crystal, L"Crystal"},
    {DR_Epic, L"Epic"},
    {DR_Evil, L"Evil"},
    {DR_Fire, L"Fire"},
    {DR_Force, L"Force"},
    {DR_Good, L"Good"},
    {DR_Lawful, L"Lawful"},
    {DR_Light, L"Light"},
    {DR_Magic, L"Magic"},
    {DR_Metalline, L"Metalline"},
    {DR_Mithral, L"Mithral"},
    {DR_Pierce, L"Pierce"},
    {DR_Silver, L"Silver"},
    {DR_Slash, L"Slash"},
    {DR_All, L"-"},
    {DR_Percent, L"%"},
    {DamageReductionType(0), NULL}
};
