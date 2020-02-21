// WeaponGroup.cpp
//
#include "stdafx.h"
#include "WeaponGroup.h"

WeaponGroup::WeaponGroup(
        const std::string & name) :
    m_groupName(name)
{
}

WeaponGroup::~WeaponGroup()
{
}

const std::string WeaponGroup::Name() const
{
    return m_groupName;
}

bool WeaponGroup::HasWeapon(WeaponType wt) const
{
    // return true if this weapon type is in the list
    bool bPresent = false;
    std::list<WeaponType>::const_iterator it = m_weaponsInGroup.begin();
    while (bPresent && it != m_weaponsInGroup.end())
    {
        bPresent = ((*it) == wt);
        ++it;
    }
    return bPresent;
}

void WeaponGroup::AddWeapon(WeaponType wt)
{
    m_weaponsInGroup.push_back(wt);
}

void WeaponGroup::RemoveWeapon(WeaponType wt)
{
    // remove the first occurrence from the list if found
    std::list<WeaponType>::iterator it = m_weaponsInGroup.begin();
    while (it != m_weaponsInGroup.end())
    {
        if ((*it) == wt)
        {
            m_weaponsInGroup.erase(it);
            it = m_weaponsInGroup.end();    // were done
        }
        else
        {
            ++it;
        }
    }
}
