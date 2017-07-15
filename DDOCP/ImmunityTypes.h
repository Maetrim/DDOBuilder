// ImmunityTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// immunities
enum ImmunityType
{
    Immunity_Unknown = 0,
    Immunity_Blindness,
    Immunity_CrushingDespair,
    Immunity_Disease,
    Immunity_DiseaseMagical,
    Immunity_DiseaseNatural,
    Immunity_DiseaseSuperNatural,
    Immunity_EnergyDrain,
    Immunity_Exhaustion,
    Immunity_Fear,
    Immunity_Knockdown,
    Immunity_MagicMissiles,
    Immunity_NaturalPoison,
    Immunity_Nausea,
    Immunity_Paralysis,
    Immunity_Poison,
    Immunity_Sickness,
    Immunity_Sleep,
    Immunity_SlipperySurfaces,
    Immunity_WavesOfExhaustion,
    Immunity_WaveOfFatigue,
};
const XmlLib::enumMapEntry<ImmunityType> immunityTypeMap[] =
{
    {Immunity_Unknown, L"Unknown"},
    {Immunity_Blindness, L"Blindness"},
    {Immunity_CrushingDespair, L"Crushing Despair"},
    {Immunity_Disease, L"Disease"},
    {Immunity_DiseaseMagical, L"Magical Disease"},
    {Immunity_DiseaseNatural, L"Natural Disease"},
    {Immunity_DiseaseSuperNatural, L"Super-Natural Disease"},
    {Immunity_EnergyDrain, L"Energy Drain"},
    {Immunity_Exhaustion, L"Exhaustion"},
    {Immunity_Fear, L"Fear"},
    {Immunity_Knockdown, L"Knockdown"},
    {Immunity_MagicMissiles, L"Magic Missiles"},
    {Immunity_NaturalPoison, L"Natural Poison"},
    {Immunity_Nausea, L"Nausea"},
    {Immunity_Paralysis, L"Paralysis"},
    {Immunity_Poison, L"Poison"},
    {Immunity_Sickness, L"Sickness"},
    {Immunity_Sleep, L"Sleep"},
    {Immunity_SlipperySurfaces, L"Slippery Surfaces"},
    {Immunity_WavesOfExhaustion, L"Waves of Exhaustion"},
    {Immunity_WaveOfFatigue, L"Waves of Fatigue"},
    {ImmunityType(0), NULL}
};
