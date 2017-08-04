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
    DR_All,
    DR_Bludgeoning,
    DR_Byeshk,
    DR_Chaotic,
    DR_ColdIron,
    DR_Epic,
    DR_Evil,
    DR_Good,
    DR_Lawful,
    DR_Magic,
    DR_Piercing,
    DR_Silver,
    DR_Slashing,
};
const XmlLib::enumMapEntry<DamageReductionType> drTypeMap[] =
{
    {DR_Unknown, L"Unknown"},
    {DR_Adamantine, L"Adamantine"},
    {DR_Alignment, L"Alignment"},
    {DR_All, L"All"},
    {DR_Bludgeoning, L"Bludgeoning"},
    {DR_Byeshk, L"Byeshk"},
    {DR_Chaotic, L"Chaotic"},
    {DR_ColdIron, L"Cold Iron"},
    {DR_Epic, L"Epic"},
    {DR_Evil, L"Evil"},
    {DR_Good, L"Good"},
    {DR_Lawful, L"Lawful"},
    {DR_Magic, L"Magic"},
    {DR_Piercing, L"Piercing"},
    {DR_Silver, L"Silver"},
    {DR_Slashing, L"Slashing"},
    {DamageReductionType(0), NULL}
};
