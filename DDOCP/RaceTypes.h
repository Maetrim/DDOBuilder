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
    Race_Aasimar,
    Race_AasimarScourge,
    Race_Drow,
    Race_Dwarf,
    Race_Dragonborn,
    Race_Eladrin,
    Race_EladrinChaosmancer,
    Race_Elf,
    Race_Gnome,
    Race_HalfElf,
    Race_Halfling,
    Race_HalfOrc,
    Race_Human,
    Race_BladeForged,
    Race_DeepGnome,
    Race_PurpleDragonKnight,
    Race_RazorclawShifter,
    Race_ShadarKai,
    Race_Shifter,
    Race_Tiefling,
    Race_TieflingScoundrel,
    Race_Morninglord,
    Race_Warforged,
    Race_WoodElf,
    Race_Kobold,
    Race_KoboldShamen,
    Race_Tabaxi,
    Race_TabaxiTrailblazer,
    // not selectable
    Race_Count,
    Race_All
};
const XmlLib::enumMapEntry<RaceType> raceTypeMap[] =
{
    {Race_Unknown, L"Unknown"},
    {Race_Aasimar, L"Aasimar"},
    {Race_AasimarScourge, L"Aasimar Scourge"},
    {Race_Drow, L"Drow"},
    {Race_Dwarf, L"Dwarf"},
    {Race_Dragonborn, L"Dragonborn"},
    {Race_Elf, L"Elf"},
    {Race_Eladrin, L"Eladrin"},
    {Race_EladrinChaosmancer, L"Eladrin Chaosmancer"},
    {Race_Gnome, L"Gnome"},
    {Race_HalfElf, L"Half-Elf"},
    {Race_Halfling, L"Halfling"},
    {Race_HalfOrc, L"Half-Orc"},
    {Race_Human, L"Human"},
    {Race_BladeForged, L"Bladeforged"},
    {Race_DeepGnome, L"Deep Gnome"},
    {Race_PurpleDragonKnight, L"Purple Dragon Knight"},
    {Race_RazorclawShifter, L"Razorclaw Shifter"},
    {Race_ShadarKai, L"Shadar-Kai"},
    {Race_Shifter, L"Shifter"},
    {Race_Tiefling, L"Tiefling"},
    {Race_TieflingScoundrel, L"Tiefling Scoundrel"},
    {Race_Morninglord, L"Morninglord"},
    {Race_Warforged, L"Warforged"},
    {Race_WoodElf, L"Wood Elf"},
    {Race_Kobold, L"Kobold (Theorycraft)"},
    {Race_KoboldShamen, L"Kobold Shamen (Theorycraft)"},
    {Race_Tabaxi, L"Tabaxi"},
    {Race_TabaxiTrailblazer, L"Tabaxi Trailblazer"},
    {Race_All, L"All"},
    {RaceType(0), NULL}
};
