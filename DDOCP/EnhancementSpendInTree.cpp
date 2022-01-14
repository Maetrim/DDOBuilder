// EnhancementSpendInTree.cpp
//
#include "StdAfx.h"
#include "EnhancementSpendInTree.h"

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementSpendInTree";
    const unsigned f_verCurrent = 1;
}

EnhancementSpendInTree::EnhancementSpendInTree() :
    SpendInTree(f_saxElementName)
{
}

EnhancementSpendInTree::EnhancementSpendInTree(
        const std::string& treeName,
        size_t version) :
    SpendInTree(f_saxElementName, treeName, version)
{
}
