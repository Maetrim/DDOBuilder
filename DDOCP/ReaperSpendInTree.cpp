// ReaperSpendInTree.cpp
//
#include "StdAfx.h"
#include "ReaperSpendInTree.h"

namespace
{
    const wchar_t f_saxElementName[] = L"ReaperSpendInTree";
    const unsigned f_verCurrent = 1;
}

ReaperSpendInTree::ReaperSpendInTree() :
    SpendInTree(f_saxElementName)
{
}

ReaperSpendInTree::ReaperSpendInTree(
        const std::string& treeName,
        size_t version) :
        SpendInTree(f_saxElementName, treeName, version)
{
}
