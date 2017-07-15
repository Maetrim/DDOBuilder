// RaceTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum RaceType
{
    Race_Unknown = 0,
    Race_Drow,
    Race_Dwarf,
    Race_Dragonborn,
    Race_Elf,
    Race_Gnome,
    Race_HalfElf,
    Race_Halfling,
    Race_HalfOrc,
    Race_Human,
    Race_BladeForged,
    Race_DeepGnome,
    Race_PurpleDragonKnight,
    Race_ShadarKai,
    Race_Morninglord,
    Race_Warforged,
    // not selectable
    Race_Count,
    Race_All
};
const XmlLib::enumMapEntry<RaceType> raceTypeMap[] =
{
    {Race_Unknown, L"Unknown"},
    {Race_Drow, L"Drow"},
    {Race_Dwarf, L"Dwarf"},
    {Race_Dragonborn, L"Dragonborn"},
    {Race_Elf, L"Elf"},
    {Race_Gnome, L"Gnome"},
    {Race_HalfElf, L"Half-Elf"},
    {Race_Halfling, L"Halfling"},
    {Race_HalfOrc, L"Half-Orc"},
    {Race_Human, L"Human"},
    {Race_BladeForged, L"Bladeforged"},
    {Race_DeepGnome, L"Deep Gnome"},
    {Race_PurpleDragonKnight, L"Purple Dragon Knight"},
    {Race_ShadarKai, L"Shadar-Kai"},
    {Race_Morninglord, L"Morninglord"},
    {Race_Warforged, L"Warforged"},
    {Race_All, L"All"},
    {RaceType(0), NULL}
};
