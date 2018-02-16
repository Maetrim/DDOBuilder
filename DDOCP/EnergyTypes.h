// EnergyTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// energy types
enum EnergyType
{
    Energy_Unknown = 0,
    Energy_Acid,
    Energy_All,
    Energy_Bludgeon,
    Energy_Cold,
    Energy_Electric,
    Energy_Evil,
    Energy_Fire,
    Energy_Force,
    Energy_Good,
    Energy_Holy,
    Energy_Light,
    Energy_Negative,
    Energy_Pierce,
    Energy_Poison,
    Energy_Positive,
    Energy_Repair,
    Energy_Rust,
    Energy_Slash,
    Energy_Sonic,
    Energy_Untyped,
    Energy_Bane,
    Energy_Chaos,
    Energy_Lawful,
    Energy_Strength,
    Energy_Intelligence,
    Energy_Wisdom,
    Energy_Dexterity,
    Energy_Constitution,
    Energy_Charisma,
};
const XmlLib::enumMapEntry<EnergyType> energyTypeMap[] =
{
    {Energy_Unknown, L"Unknown"},
    {Energy_Acid, L"Acid"},
    {Energy_All, L"All"},
    {Energy_Bludgeon, L"Bludgeon"},
    {Energy_Cold, L"Cold"},
    {Energy_Electric, L"Electric"},
    {Energy_Evil, L"Evil"},
    {Energy_Fire, L"Fire"},
    {Energy_Force, L"Force"},
    {Energy_Good, L"Good"},
    {Energy_Holy, L"Holy"},
    {Energy_Light, L"Light"},
    {Energy_Negative, L"Negative"},
    {Energy_Pierce, L"Pierce"},
    {Energy_Poison, L"Poison"},
    {Energy_Positive, L"Positive"},
    {Energy_Repair, L"Repair"},
    {Energy_Rust, L"Rust"},
    {Energy_Slash, L"Slash"},
    {Energy_Sonic, L"Sonic"},
    {Energy_Untyped, L"Untyped"},
    {Energy_Bane, L"Bane"},
    {Energy_Chaos, L"Chaos"},
    {Energy_Lawful, L"Lawful"},
    {Energy_Strength, L"Strength"},
    {Energy_Intelligence, L"Intelligence"},
    {Energy_Wisdom, L"Wisdom"},
    {Energy_Dexterity, L"Dexterity"},
    {Energy_Constitution, L"Constitution"},
    {Energy_Charisma, L"Charisma"},
    {EnergyType(0), NULL}
};
