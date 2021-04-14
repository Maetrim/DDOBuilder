// ArtifactFiligree.cpp
//
#include "StdAfx.h"
#include "ArtifactFiligree.h"

#define DL_ELEMENT ArtifactFiligree

namespace
{
    DL_DEFINE_NAMES(Filigree_PROPERTIES)
    DL_DEFINE_NAMES(ArtifactFiligree_PROPERTIES)
    const wchar_t f_saxElementName[] = L"ArtifactFiligree";
    const unsigned f_verCurrent = 1;
}

ArtifactFiligree::ArtifactFiligree() :
    Filigree(f_saxElementName, f_verCurrent)
{
    DL_INIT(ArtifactFiligree_PROPERTIES)
}

bool ArtifactFiligree::operator==(const ArtifactFiligree& other) const
{
    // comparison in terms of base item
    return Filigree::operator==(other);
}

DL_DEFINE_ACCESS(ArtifactFiligree_PROPERTIES)

XmlLib::SaxContentElementInterface* ArtifactFiligree::StartElement(
    const XmlLib::SaxString& name,
    const XmlLib::SaxAttributes& attributes)
{
    XmlLib::SaxContentElementInterface* subHandler =
        SaxContentElement::StartElement(name, attributes);

    DL_START_MULTIPLE(Filigree_PROPERTIES)
    DL_START_MULTIPLE_END(ArtifactFiligree_PROPERTIES)

    return subHandler;
}

void ArtifactFiligree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Filigree_PROPERTIES)
    DL_END(ArtifactFiligree_PROPERTIES)
}

void ArtifactFiligree::Write(XmlLib::SaxWriter* writer) const
{
    writer->StartElement(ElementName(), VersionAttributes());
    DL_WRITE(Filigree_PROPERTIES)
    DL_WRITE(ArtifactFiligree_PROPERTIES)
    writer->EndElement();
}
