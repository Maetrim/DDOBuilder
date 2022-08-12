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

        void AddRank(bool isTier5);
        void RevokeRank();

        void SetCost(const std::vector<size_t>& cost);
        size_t Cost(size_t rank) const;
        size_t TotalCost() const;

        void SetRequiredAps(size_t requiredAps);
        size_t RequiredAps() const;

        bool HasRequirementOf(const std::string& dependentOnEnhancementName,
                const std::string& selection) const;

        bool operator<(const TrainedEnhancement& other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedEnhancement_PROPERTIES(_) \
                DL_STRING(_, EnhancementName) \
                DL_OPTIONAL_STRING(_, Selection) \
                DL_SIMPLE(_, size_t, Ranks, 0) \
                DL_FLAG(_, IsTier5)

        DL_DECLARE_ACCESS(TrainedEnhancement_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedEnhancement_PROPERTIES)

        std::vector<size_t> m_cost;      // cached only as may change on enhancements update
        size_t m_requiredAps;

        friend class SpendInTree;
};
