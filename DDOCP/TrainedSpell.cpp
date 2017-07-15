// TrainedSpell.cpp
//
#include "StdAfx.h"
#include "TrainedSpell.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT TrainedSpell

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedSpell";
    DL_DEFINE_NAMES(TrainedSpell_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TrainedSpell::TrainedSpell() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(TrainedSpell_PROPERTIES)
}

DL_DEFINE_ACCESS(TrainedSpell_PROPERTIES)

XmlLib::SaxContentElementInterface * TrainedSpell::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(TrainedSpell_PROPERTIES)

    return subHandler;
}

void TrainedSpell::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(TrainedSpell_PROPERTIES)
}

void TrainedSpell::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(TrainedSpell_PROPERTIES)
    writer->EndElement();
}

bool TrainedSpell::operator==(const TrainedSpell & other) const
{
    bool equal = (other.Class() == Class())
            && (other.Level() == Level())
            && (other.SpellName() == SpellName());
    return equal;
}

bool TrainedSpell::operator<(const TrainedSpell & other) const
{
    // return true if elements in order (this < other)
    // first sort by class
    bool ret = false;
    if (Class() < other.Class())
    {
        ret = true; // we are in order
    }
    else if (Class() > other.Class())
    {
        ret = false; // out of order
    }
    else
    {
        // spell is in same class, sort by level
        if (Level() < other.Level())
        {
            ret = true; // in order
        }
        else if (Level() > other.Level())
        {
            ret = false; // out of order
        }
        else
        {
            // spell is in same class/level, sort by name
            ret = (SpellName() < other.SpellName());
        }
    }
    return ret;
}
