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
    Energy_Cold,
    Energy_Electric,
    Energy_Fire,
    Energy_Force,
    Energy_Light,
    Energy_Negative,
    Energy_Poison,
    Energy_Positive,
    Energy_Repair,
    Energy_Rust,
    Energy_Sonic,
};
const XmlLib::enumMapEntry<EnergyType> energyTypeMap[] =
{
    {Energy_Unknown, L"Unknown"},
    {Energy_Acid, L"Acid"},
    {Energy_All, L"All"},
    {Energy_Cold, L"Cold"},
    {Energy_Electric, L"Electric"},
    {Energy_Fire, L"Fire"},
    {Energy_Force, L"Force"},
    {Energy_Light, L"Light"},
    {Energy_Negative, L"Negative"},
    {Energy_Poison, L"Poison"},
    {Energy_Positive, L"Positive"},
    {Energy_Repair, L"Repair"},
    {Energy_Rust, L"Rust"},
    {Energy_Sonic, L"Sonic"},
    {EnergyType(0), NULL}
};
