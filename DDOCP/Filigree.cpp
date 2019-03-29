// Filigree.cpp
//
#include "StdAfx.h"
#include "Filigree.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT Filigree

namespace
{
    const wchar_t f_saxElementName[] = L"Filigree";
    DL_DEFINE_NAMES(Filigree_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Filigree::Filigree() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Filigree_PROPERTIES)
}

DL_DEFINE_ACCESS(Filigree_PROPERTIES)

XmlLib::SaxContentElementInterface * Filigree::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Filigree_PROPERTIES)

    return subHandler;
}

void Filigree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Filigree_PROPERTIES)
}

void Filigree::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Filigree_PROPERTIES)
    writer->EndElement();
}
