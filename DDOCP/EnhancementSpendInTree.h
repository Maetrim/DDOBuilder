// EnhancementSpendInTree.h
//
// An XML object wrapper that holds information on enhancements trained in a specific tree
#pragma once
#include "SpendInTree.h"

class EnhancementSpendInTree :
    public SpendInTree
{
    public:
        EnhancementSpendInTree();
        EnhancementSpendInTree(const std::string& treeName, size_t version);
};
