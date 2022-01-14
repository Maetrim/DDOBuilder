// ReaperSpendInTree.h
//
// An XML object wrapper that holds information on enhancements trained in a specific tree
#pragma once
#include "SpendInTree.h"

class ReaperSpendInTree :
    public SpendInTree
{
    public:
        ReaperSpendInTree();
        ReaperSpendInTree(const std::string& treeName, size_t version);
};
