// TrainedFeat.cpp
//
#include "StdAfx.h"
#include "TrainedFeat.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT TrainedFeat

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedFeat";
    DL_DEFINE_NAMES(TrainedFeat_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TrainedFeat::TrainedFeat() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(TrainedFeat_PROPERTIES)
}

DL_DEFINE_ACCESS(TrainedFeat_PROPERTIES)

XmlLib::SaxContentElementInterface * TrainedFeat::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(TrainedFeat_PROPERTIES)

    return subHandler;
}

void TrainedFeat::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(TrainedFeat_PROPERTIES)
}

void TrainedFeat::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(TrainedFeat_PROPERTIES)
    writer->EndElement();
}

bool TrainedFeat::operator<(const TrainedFeat & other) const
{
    if (Type() == other.Type())
    {
        return (FeatName() < other.FeatName());
    }
    else
    {
        return (Type() < other.Type());
    }
}

bool TrainedFeat::operator==(const TrainedFeat & other) const
{
    return m_FeatName == other.m_FeatName
            && m_Type == other.m_Type
            && m_LevelTrainedAt == other.m_LevelTrainedAt;
}

