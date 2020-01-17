// GroupTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// Feat grouping
enum GroupType
{
    Group_Unknown = 0,
    Group_AlchemistBonus,
    Group_ArtificerBonus,
    Group_Divine,
    Group_DruidShape,
    Group_FavoredEnemy,
    Group_FavoredSoulBonus,
    Group_FighterBonus,
    Group_HalfElfBonus,
    Group_Metamagic,
    Group_MonkBonus,
    Group_PastLife,
    Group_RogueBonus,
    Group_Special,
};
const XmlLib::enumMapEntry<GroupType> groupTypeMap[] =
{
    {Group_Unknown, L"Unknown"},
    {Group_AlchemistBonus, L"AlchemistBonus"},
    {Group_ArtificerBonus, L"ArtificerBonus"},
    {Group_Divine, L"Divine"},
    {Group_DruidShape, L"DruidShape"},
    {Group_FavoredEnemy, L"FavoredEnemy"},
    {Group_FavoredSoulBonus, L"FavoredSoulBonus"},
    {Group_FighterBonus, L"FighterBonus"},
    {Group_HalfElfBonus, L"HalfElfBonus"},
    {Group_Metamagic, L"Metamagic"},
    {Group_MonkBonus, L"MonkBonus"},
    {Group_PastLife, L"PastLife"},
    {Group_RogueBonus, L"RogueBonus"},
    {Group_Special, L"Special"},
    {GroupType(0), NULL}
};
