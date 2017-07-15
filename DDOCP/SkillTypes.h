// SkillTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum SkillType
{
    Skill_Unknown = 0,
    Skill_Balance,
    Skill_Bluff,
    Skill_Concentration,
    Skill_Diplomacy,
    Skill_DisableDevice,
    Skill_Haggle,
    Skill_Heal,
    Skill_Hide,
    Skill_Intimidate,
    Skill_Jump,
    Skill_Listen,
    Skill_MoveSilently,
    Skill_OpenLock,
    Skill_Perform,
    Skill_Repair,
    Skill_Search,
    Skill_SpellCraft,
    Skill_Spot,
    Skill_Swim,
    Skill_Tumble,
    Skill_UMD,

    Skill_Count,
    Skill_All,
};
const XmlLib::enumMapEntry<SkillType> skillTypeMap[] =
{
    {Skill_Unknown,         L"Unknown"},
    {Skill_Balance,         L"Balance"},
    {Skill_Bluff,           L"Bluff"},
    {Skill_Concentration,   L"Concentration"},
    {Skill_Diplomacy,       L"Diplomacy"},
    {Skill_DisableDevice,   L"Disable Device"},
    {Skill_Haggle,          L"Haggle"},
    {Skill_Heal,            L"Heal"},
    {Skill_Hide,            L"Hide"},
    {Skill_Intimidate,      L"Intimidate"},
    {Skill_Jump,            L"Jump"},
    {Skill_Listen,          L"Listen"},
    {Skill_MoveSilently,    L"Move Silently"},
    {Skill_OpenLock,        L"Open Lock"},
    {Skill_Perform,         L"Perform"},
    {Skill_Repair,          L"Repair"},
    {Skill_Search,          L"Search"},
    {Skill_SpellCraft,      L"Spell Craft"},
    {Skill_Spot,            L"Spot"},
    {Skill_Swim,            L"Swim"},
    {Skill_Tumble,          L"Tumble"},
    {Skill_UMD,             L"Use Magic Device"},
    {Skill_All,             L"All"},
    {SkillType(0), NULL}
};
