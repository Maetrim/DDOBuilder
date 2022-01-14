// DestinySpendInTree.cpp
//
#include "StdAfx.h"
#include "DestinySpendInTree.h"

namespace
{
    const wchar_t f_saxElementName[] = L"DestinySpendInTree";
}

DestinySpendInTree::DestinySpendInTree() :
    SpendInTree(f_saxElementName)
{
}

DestinySpendInTree::DestinySpendInTree(
        const std::string& treeName,
        size_t version) :
    SpendInTree(f_saxElementName, treeName, version)
{
}
