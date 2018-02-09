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
    DR_ColdIron,
    DR_Epic,
    DR_Evil,
    DR_Good,
    DR_Lawful,
    DR_Magic,
    DR_Metalline,
    DR_Pierce,
    DR_Silver,
    DR_Slash,
    DR_All,
};
const XmlLib::enumMapEntry<DamageReductionType> drTypeMap[] =
{
    {DR_Unknown, L"Unknown"},
    {DR_Adamantine, L"Adamantine"},
    {DR_Alignment, L"Alignment"},
    {DR_Bludgeon, L"Bludgeon"},
    {DR_Byeshk, L"Byeshk"},
    {DR_Chaotic, L"Chaotic"},
    {DR_ColdIron, L"Cold Iron"},
    {DR_Epic, L"Epic"},
    {DR_Evil, L"Evil"},
    {DR_Good, L"Good"},
    {DR_Lawful, L"Lawful"},
    {DR_Magic, L"Magic"},
    {DR_Metalline, L"Metalline"},
    {DR_Pierce, L"Pierce"},
    {DR_Silver, L"Silver"},
    {DR_Slash, L"Slash"},
    {DR_All, L"-"},
    {DamageReductionType(0), NULL}
};
