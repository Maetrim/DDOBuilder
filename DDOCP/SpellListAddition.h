// SpellListAddition.h
//
#pragma once

#include "ClassTypes.h"
#include <string>

class SpellListAddition
{
    public:
        SpellListAddition(ClassType ct, size_t spellLevel, const std::string& spellName);
        ~SpellListAddition();

        bool AddsToSpellList(ClassType ct, size_t spellLevel) const;
        std::string SpellName() const;
        void AddReference();
        bool RemoveReference(); // returns true if m_count == 0
    private:
        ClassType m_class;
        size_t m_spellLevel;
        std::string m_spellName;
        size_t m_count;
};
