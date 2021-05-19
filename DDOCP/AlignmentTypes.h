// AlignmentTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum AlignmentType
{
    Alignment_Unknown = 0,
    Alignment_LawfulGood,
    Alignment_LawfulNeutral,
    Alignment_NeutralGood,
    Alignment_TrueNeutral,
    Alignment_ChaoticNeutral,
    Alignment_ChaoticGood,
    // not selectable
    Alignment_Count,
    Alignment_NeutralEvil,
    Alignment_ChaoticEvil,
};
const XmlLib::enumMapEntry<AlignmentType> alignmentTypeMap[] =
{
    {Alignment_Unknown, L"Unknown"},
    {Alignment_LawfulGood, L"Lawful Good"},
    {Alignment_LawfulNeutral, L"Lawful Neutral"},
    {Alignment_NeutralGood, L"Neutral Good"},
    {Alignment_TrueNeutral, L"Neutral"},
    {Alignment_ChaoticNeutral, L"Chaotic Neutral"},
    {Alignment_ChaoticGood, L"Chaotic Good"},
    {Alignment_NeutralEvil, L"Neutral Evil"},
    {Alignment_ChaoticEvil, L"Chaotic Evil"},
    {AlignmentType(0), NULL}
};
