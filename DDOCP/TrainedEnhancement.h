// TrainedEnhancement.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class TrainedEnhancement :
    public XmlLib::SaxContentElement
{
    public:
        TrainedEnhancement();
        void Write(XmlLib::SaxWriter * writer) const;

        void AddRank(size_t buyIndex, bool isTier5);
        bool IsYourBuyIndex(size_t buyIndex) const;
        void RevokeRank();

        void SetCost(size_t cost);
        size_t Cost() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedEnhancement_PROPERTIES(_) \
                DL_STRING(_, EnhancementName) \
                DL_OPTIONAL_STRING(_, Selection) \
                DL_SIMPLE(_, size_t, Ranks, 0) \
                DL_FLAG(_, IsTier5) \
                DL_VECTOR(_, size_t, BuyIndex)

        DL_DECLARE_ACCESS(TrainedEnhancement_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedEnhancement_PROPERTIES)

        size_t m_cost;      // cached only as may change on enhancements update

        friend class EnhancementSpendInTree;
        friend class EpicDestinySpendInTree;
        friend class ReaperSpendInTree;
};
