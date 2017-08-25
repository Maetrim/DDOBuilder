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
    Augment_Yellow,
    Augment_Colorless,
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
    Augment_SlavelordsPrefix,
    Augment_SlavelordsSuffix,
    Augment_SlavelordsExtra,
    Augment_SlavelordsBonus,
    Augment_SlavelordsSetBonus,
    Augment_SlavelordsAugmentSlot,
    // epic slave lords
    Augment_LegendarySlavelordsPrefix,
    Augment_LegendarySlavelordsSuffix,
    Augment_LegendarySlavelordsExtra,
    Augment_LegendarySlavelordsBonus,
    Augment_LegendarySlavelordsSetBonus,
    Augment_LegendarySlavelordsAugmentSlot,
    // specials
    Augment_Erudition,
    Augment_Prowess,
    Augment_Subterfuge,
    Augment_Avithoul,
    Augment_DunRobarPrefix,
    Augment_DunRobarSuffix,
    Augment_Szind,
    Augment_GoatskinOptional,
    Augment_SkullduggeryVariant,
    Augment_ShadesHood,
    Augment_BlackDragonHelmVariant,
    Augment_BlueDragonHelmVariant,
    Augment_WhiteDragonHelmVariant,
    Augment_PrisonersManacles,
    Augment_IntricateFieldOptics,
    Augment_PrismaticCloak,
};
const XmlLib::enumMapEntry<AugmentType> augmentTypeMap[] =
{
    {Augment_Unknown, L"Unknown"},
    {Augment_Blue, L"Blue"},
    {Augment_Green, L"Green"},
    {Augment_Orange, L"Orange"},
    {Augment_Purple, L"Purple"},
    {Augment_Red, L"Red"},
    {Augment_Yellow, L"Yellow"},
    {Augment_Colorless, L"Colorless"},
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
    {Augment_SlavelordsPrefix, L"Slavelords Prefix"},
    {Augment_SlavelordsSuffix, L"Slavelords Suffix"},
    {Augment_SlavelordsExtra, L"Slavelords Extra"},
    {Augment_SlavelordsBonus, L"Slavelords Bonus"},
    {Augment_SlavelordsSetBonus, L"Slavelords Set Bonus"},
    {Augment_SlavelordsAugmentSlot, L"Slavelords Augment Slot"},

    {Augment_LegendarySlavelordsPrefix, L"Legendary Slavelords Prefix"},
    {Augment_LegendarySlavelordsSuffix, L"Legendary Slavelords Suffix"},
    {Augment_LegendarySlavelordsExtra, L"Legendary Slavelords Extra"},
    {Augment_LegendarySlavelordsBonus, L"Legendary Slavelords Bonus"},
    {Augment_LegendarySlavelordsSetBonus, L"Legendary Slavelords Set Bonus"},
    {Augment_LegendarySlavelordsAugmentSlot, L"Legendary Slavelords Augment Slot"},

    {Augment_Erudition, L"Erudition"},
    {Augment_Prowess, L"Prowess"},
    {Augment_Subterfuge, L"Subterfuge"},
    {Augment_Avithoul, L"Avithoul"},
    {Augment_DunRobarPrefix, L"Dun'Robar Prefix"},
    {Augment_DunRobarSuffix,L"Dun'Robar Suffix"},
    {Augment_Szind, L"Szind"},
    {Augment_GoatskinOptional, L"Goatskin Optional"},
    {Augment_SkullduggeryVariant, L"Skullduggery Variant"},
    {Augment_ShadesHood, L"ShadesHood"},
    {Augment_BlackDragonHelmVariant, L"Black Dragon Helm Variant"},
    {Augment_BlueDragonHelmVariant, L"Blue Dragon Helm Variant"},
    {Augment_WhiteDragonHelmVariant, L"White Dragon Helm Variant"},
    {Augment_PrisonersManacles, L"Manacles Variant"},
    {Augment_IntricateFieldOptics, L"Field Optics Variant"},
    {Augment_PrismaticCloak, L"Cloak Variant"},
    {AugmentType(0), NULL}
};
