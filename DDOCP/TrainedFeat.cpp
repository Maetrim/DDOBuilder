// TrainedFeat.cpp
//
#include "StdAfx.h"
#include "TrainedFeat.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT TrainedFeat

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedFeat";
    DL_DEFINE_NAMES(TrainedFeat_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TrainedFeat::TrainedFeat() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_count(1)
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
    // update TFT_Special to correct type
    if (m_Type == TFT_Special)
    {
        // old style feat type, convert to correct type
        const Feat & feat = FindFeat(FeatName());
        switch (feat.Acquire())
        {
        case FeatAcquisition_HeroicPastLife:
            m_Type = TFT_HeroicPastLife;
            break;
        case FeatAcquisition_RacialPastLife:
            m_Type = TFT_RacialPastLife;
            break;
        case FeatAcquisition_IconicPastLife:
            m_Type = TFT_IconicPastLife;
            break;
        case FeatAcquisition_EpicPastLife:
            m_Type = TFT_EpicPastLife;
            break;
        case FeatAcquisition_Special:
            m_Type = TFT_SpecialFeat;
            break;
        default:
            ::OutputDebugString("Failed to translate feat type\n");
            break;
        }
    }
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
    // ignore revoked feat for comparison
    return m_FeatName == other.m_FeatName
            && m_Type == other.m_Type
            && m_LevelTrainedAt == other.m_LevelTrainedAt;
}

size_t TrainedFeat::Count() const
{
    return m_count;
}

void TrainedFeat::IncrementCount()
{
    ++m_count;
}
