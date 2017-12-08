// SentientJewel.cpp
//
#include "StdAfx.h"
#include "SentientJewel.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SentientJewel

namespace
{
    const wchar_t f_saxElementName[] = L"SentientJewel";
    DL_DEFINE_NAMES(SentientJewel_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SentientJewel::SentientJewel() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SentientJewel_PROPERTIES)
}

DL_DEFINE_ACCESS(SentientJewel_PROPERTIES)

XmlLib::SaxContentElementInterface * SentientJewel::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SentientJewel_PROPERTIES)

    return subHandler;
}

void SentientJewel::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SentientJewel_PROPERTIES)
}

void SentientJewel::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SentientJewel_PROPERTIES)
    writer->EndElement();
}

std::string SentientJewel::Filigree(size_t fi) const
{
    std::string filigree;       // return "" if no filigree
    switch (fi)
    {
    case 0: if (HasFiligree1()) filigree = Filigree1(); break;
    case 1: if (HasFiligree2()) filigree = Filigree2(); break;
    case 2: if (HasFiligree3()) filigree = Filigree3(); break;
    case 3: if (HasFiligree4()) filigree = Filigree4(); break;
    case 4: if (HasFiligree5()) filigree = Filigree5(); break;
    case 5: if (HasFiligree6()) filigree = Filigree6(); break;
    case 6: if (HasFiligree7()) filigree = Filigree7(); break;
    }
    return filigree;
}

void SentientJewel::SetFiligree(size_t fi, const std::string & name)
{
    switch (fi)
    {
    case 0: Set_Filigree1(name); break;
    case 1: Set_Filigree2(name); break;
    case 2: Set_Filigree3(name); break;
    case 3: Set_Filigree4(name); break;
    case 4: Set_Filigree5(name); break;
    case 5: Set_Filigree6(name); break;
    case 6: Set_Filigree7(name); break;
    }
}

