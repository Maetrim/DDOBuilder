// DestinySpendInTree.h
//
// An XML object wrapper that holds information on enhancements trained in a specific tree
#pragma once
#include "SpendInTree.h"

class DestinySpendInTree :
    public SpendInTree
{
    public:
        DestinySpendInTree();
        DestinySpendInTree(const std::string& treeName, size_t version);
};
