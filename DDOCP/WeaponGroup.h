// WeaponGroup.h
//
// A class that handles list of weapons that belong to a specific grouping
// grouping will be things like "Light Weapons", "Favored Weapons", "Focus Weapons"
#pragma once

#include "WeaponTypes.h"
#include <list>

class WeaponGroup
{
    public:
        WeaponGroup(const std::string & name);
        virtual ~WeaponGroup();

        const std::string Name() const;
        bool HasWeapon(WeaponType wt) const;
        void AddWeapon(WeaponType wt);
        void RemoveWeapon(WeaponType wt);

    private:
        std::string m_groupName;
        std::list<WeaponType> m_weaponsInGroup;
};
