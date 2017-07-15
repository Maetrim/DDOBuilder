// EquipmentSlot.cpp
//
#include "StdAfx.h"
#include "EquipmentSlot.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EquipmentSlot

namespace
{
    const wchar_t f_saxElementName[] = L"EquipmentSlot";
    DL_DEFINE_NAMES(EquipmentSlot_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EquipmentSlot::EquipmentSlot() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(EquipmentSlot_PROPERTIES)
}

DL_DEFINE_ACCESS(EquipmentSlot_PROPERTIES)

XmlLib::SaxContentElementInterface * EquipmentSlot::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EquipmentSlot_PROPERTIES)

    return subHandler;
}

void EquipmentSlot::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EquipmentSlot_PROPERTIES)
}

void EquipmentSlot::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EquipmentSlot_PROPERTIES)
    writer->EndElement();
}
