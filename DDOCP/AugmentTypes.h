// AugmentTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum AugmentType
{
    Augment_Unknown = 0,
    // standard augment slots
    Augment_Blue,
    Augment_Green,
    Augment_Orange,
    Augment_Purple,
    Augment_Red,
    Augment_White,
    Augment_Yellow,
    // special crafting augment slots
    // heroic green steel
    Augment_GreensteelItemTier1,
    Augment_GreensteelItemTier2,
    Augment_GreensteelItemTier3,
    Augment_GreensteelWeaponTier1,
    Augment_GreensteelWeaponTier2,
    Augment_GreensteelWeaponTier3,
    // epic green steel
    Augment_EpicGreensteelItemTier1,
    Augment_EpicGreensteelItemTier2,
    Augment_EpicGreensteelItemTier3,
    Augment_EpicGreensteelWeaponTier1,
    Augment_EpicGreensteelWeaponTier2,
    Augment_EpicGreensteelWeaponTier3,
    // heroic slave lords
    Augment_SlavelordsSlot1,
    Augment_SlavelordsSlot2,
    Augment_SlavelordsSlot3,
    Augment_SlavelordsSlot4,
    Augment_SlavelordsSlot5,
    // epic slave lords
    Augment_EpicSlavelordsSlot1,
    Augment_EpicSlavelordsSlot2,
    Augment_EpicSlavelordsSlot3,
    Augment_EpicSlavelordsSlot4,
    Augment_EpicSlavelordsSlot5,
};
const XmlLib::enumMapEntry<AugmentType> augmentTypeMap[] =
{
    {Augment_Unknown, L"Unknown"},
    {Augment_Blue, L"Blue"},
    {Augment_Green, L"Green"},
    {Augment_Orange, L"Orange"},
    {Augment_Purple, L"Purple"},
    {Augment_Red, L"Red"},
    {Augment_White, L"White"},
    {Augment_Yellow, L"Yellow"},
    {Augment_GreensteelItemTier1, L"GreensteelItemTier1"},
    {Augment_GreensteelItemTier2, L"GreensteelItemTier2"},
    {Augment_GreensteelItemTier3, L"GreensteelItemTier3"},
    {Augment_GreensteelWeaponTier1, L"GreensteelWeaponTier1"},
    {Augment_GreensteelWeaponTier2, L"GreensteelWeaponTier2"},
    {Augment_GreensteelWeaponTier3, L"GreensteelWeaponTier3"},
    {Augment_EpicGreensteelItemTier1, L"EpicGreensteelItemTier1"},
    {Augment_EpicGreensteelItemTier2, L"EpicGreensteelItemTier2"},
    {Augment_EpicGreensteelItemTier3, L"EpicGreensteelItemTier3"},
    {Augment_EpicGreensteelWeaponTier1, L"EpicGreensteelWeaponTier1"},
    {Augment_EpicGreensteelWeaponTier2, L"EpicGreensteelWeaponTier2"},
    {Augment_EpicGreensteelWeaponTier3, L"EpicGreensteelWeaponTier3"},
    {Augment_SlavelordsSlot1, L"SlavelordsSlot1"},
    {Augment_SlavelordsSlot2, L"SlavelordsSlot2"},
    {Augment_SlavelordsSlot3, L"SlavelordsSlot3"},
    {Augment_SlavelordsSlot4, L"SlavelordsSlot4"},
    {Augment_SlavelordsSlot5, L"SlavelordsSlot5"},
    {Augment_EpicSlavelordsSlot1, L"EpicSlavelordsSlot1"},
    {Augment_EpicSlavelordsSlot2, L"EpicSlavelordsSlot2"},
    {Augment_EpicSlavelordsSlot3, L"EpicSlavelordsSlot3"},
    {Augment_EpicSlavelordsSlot4, L"EpicSlavelordsSlot4"},
    {Augment_EpicSlavelordsSlot5, L"EpicSlavelordsSlot5"},
    {AugmentType(0), NULL}
};
