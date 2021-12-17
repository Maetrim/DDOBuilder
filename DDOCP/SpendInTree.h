// SpendInTree.h
//
// An XML object wrapper that holds information on enhancements trained in a specific tree
#pragma once
#include "XmlLib\DLMacros.h"
#include "TrainedEnhancement.h"

class SpendInTree :
    public XmlLib::SaxContentElement
{
    public:
        SpendInTree(const XmlLib::SaxString & elementName);
        SpendInTree(const XmlLib::SaxString & elementName, const std::string& treeName, size_t version);
        void Write(XmlLib::SaxWriter * writer) const;

        void SetTree(const std::string& treeName, size_t treeVersion);

        size_t TrainedRanks(const std::string& enhancementName, std::string* pSelection) const;
        bool CanRevokeAtTier(size_t minSpent, size_t cost) const;
        bool HasTrainedDependants(
                const std::string& enhancementName,
                size_t ranksTrained) const;
        size_t TrainEnhancement(
                const std::string& enhancementName,
                const std::string& selection,
                const std::vector<size_t>& cost,
                size_t minSpent,
                bool isTier5,
                size_t * ranks); // returns actual cost, may differ to what is passed in
        int RevokeEnhancement(
                const std::string& revokedEnhancement,
                std::string * revokedEnhancementSelection,
                size_t * ranks); // revokes last enhancement trained in tree (if any)
        bool HasTier5() const;
        void SetSpent(size_t apsSpent);
        size_t Spent() const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SpendInTree_PROPERTIES(_) \
                DL_STRING(_, TreeName) \
                DL_SIMPLE(_, size_t, TreeVersion, 0) \
                DL_OBJECT_LIST(_, TrainedEnhancement, Enhancements)

        DL_DECLARE_ACCESS(SpendInTree_PROPERTIES)
        DL_DECLARE_VARIABLES(SpendInTree_PROPERTIES)
    private:
        bool EnoughPointsSpentAtLowerTiers(size_t minSpent, size_t cost) const;

        size_t m_pointsSpent;       // run time only as prices can change between runs on enhancement updates

        friend class Character;
};
