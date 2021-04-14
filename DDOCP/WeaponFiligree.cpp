// WeaponFiligree.cpp
//
#include "StdAfx.h"
#include "WeaponFiligree.h"

#define DL_ELEMENT WeaponFiligree

namespace
{
    DL_DEFINE_NAMES(Filigree_PROPERTIES)
    DL_DEFINE_NAMES(WeaponFiligree_PROPERTIES)
    const wchar_t f_saxElementName[] = L"Filigree";
    const unsigned f_verCurrent = 1;
}

WeaponFiligree::WeaponFiligree() :
    Filigree(f_saxElementName, f_verCurrent)
{
    DL_INIT(WeaponFiligree_PROPERTIES)
}

bool WeaponFiligree::operator==(const WeaponFiligree& other) const
{
    // comparison in terms of base item
    return Filigree::operator==(other);
}

DL_DEFINE_ACCESS(WeaponFiligree_PROPERTIES)

XmlLib::SaxContentElementInterface* WeaponFiligree::StartElement(
    const XmlLib::SaxString& name,
    const XmlLib::SaxAttributes& attributes)
{
    XmlLib::SaxContentElementInterface* subHandler =
        SaxContentElement::StartElement(name, attributes);

    DL_START_MULTIPLE(Filigree_PROPERTIES)
    DL_START_MULTIPLE_END(WeaponFiligree_PROPERTIES)

    return subHandler;
}

void WeaponFiligree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Filigree_PROPERTIES)
    DL_END(WeaponFiligree_PROPERTIES)
}

void WeaponFiligree::Write(XmlLib::SaxWriter* writer) const
{
    writer->StartElement(ElementName(), VersionAttributes());
    DL_WRITE(Filigree_PROPERTIES)
    DL_WRITE(WeaponFiligree_PROPERTIES)
    writer->EndElement();
}
