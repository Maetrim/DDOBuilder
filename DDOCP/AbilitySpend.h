// AbilitySpend.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "AbilityTypes.h"

class AbilitySpend :
    public XmlLib::SaxContentElement
{
    public:
        AbilitySpend();
        void Write(XmlLib::SaxWriter * writer) const;

        size_t PointsSpent() const; // evaluates how many points have been spent
        bool CanSpendOnAbility(AbilityType ability) const;
        bool CanRevokeSpend(AbilityType ability) const;
        size_t NextPointsSpentCost(AbilityType ability) const;

        size_t GetAbilitySpend(AbilityType ability) const;
        void SpendOnAbility(AbilityType ability);
        void RevokeSpendOnAbility(AbilityType ability);
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // spends are in base ability points, range from 0->10
        #define AbilitySpend_PROPERTIES(_) \
                DL_SIMPLE(_, size_t, AvailableSpend, 28) \
                DL_SIMPLE(_, size_t, StrSpend, 0) \
                DL_SIMPLE(_, size_t, DexSpend, 0) \
                DL_SIMPLE(_, size_t, ConSpend, 0) \
                DL_SIMPLE(_, size_t, IntSpend, 0) \
                DL_SIMPLE(_, size_t, WisSpend, 0) \
                DL_SIMPLE(_, size_t, ChaSpend, 0)

        DL_DECLARE_ACCESS(AbilitySpend_PROPERTIES)
        DL_DECLARE_VARIABLES(AbilitySpend_PROPERTIES)
    private:
        size_t GetAbilitySpendPoints(AbilityType ability) const;

        friend class Character;
};
