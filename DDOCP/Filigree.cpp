// Filigree.cpp
//
#include "StdAfx.h"
#include "Filigree.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT Filigree

namespace
{
    DL_DEFINE_NAMES(Filigree_PROPERTIES)
}

Filigree::Filigree(const XmlLib::SaxString& elementName, unsigned verCurrent) :
    XmlLib::SaxContentElement(elementName, verCurrent)
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

bool Filigree::operator==(const Filigree & other) const
{
    bool bEqual = 
                m_Name == other.m_Name
                && m_hasRare == other.m_hasRare;
    return bEqual;
}

