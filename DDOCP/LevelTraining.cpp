// LevelTraining.cpp
//
#include "StdAfx.h"
#include "LevelTraining.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT LevelTraining

namespace
{
    const wchar_t f_saxElementName[] = L"LevelTraining";
    DL_DEFINE_NAMES(LevelTraining_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

LevelTraining::LevelTraining() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_AutomaticFeats(L"AutomaticFeats"),
    m_TrainedFeats(L"TrainedFeats")
{
    DL_INIT(LevelTraining_PROPERTIES)
}

DL_DEFINE_ACCESS(LevelTraining_PROPERTIES)

XmlLib::SaxContentElementInterface * LevelTraining::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(LevelTraining_PROPERTIES)

    return subHandler;
}

void LevelTraining::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(LevelTraining_PROPERTIES)
}

void LevelTraining::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(LevelTraining_PROPERTIES)
    writer->EndElement();
}

void LevelTraining::TrainFeat(
        const std::string & featName,
        TrainableFeatTypes type,
        size_t level)
{
    m_TrainedFeats.TrainFeat(featName, type, level);
}

std::string LevelTraining::RevokeFeat(TrainableFeatTypes type)
{
    return m_TrainedFeats.RevokeFeat(type);
}

void LevelTraining::RevokeAllFeats(TrainableFeatTypes type)
{
    m_TrainedFeats.RevokeAllFeats(type);
}

void LevelTraining::TrainSkill(SkillType skill)
{
    TrainedSkill ts;
    ts.Set_Skill(skill);
    m_TrainedSkills.push_back(ts);
    ASSERT(m_SkillPointsSpent < m_SkillPointsAvailable);
    ++m_SkillPointsSpent;
}

void LevelTraining::RevokeSkill(SkillType skill)
{
    ASSERT(m_SkillPointsSpent > 0);
    bool found = false;
    std::list<TrainedSkill>::iterator it = m_TrainedSkills.begin();
    while (it != m_TrainedSkills.end())
    {
        if ((*it).Skill() == skill)
        {
            // revoke this one
            it = m_TrainedSkills.erase(it);
            found = true;
            break;  // were done
        }
        ++it;
    }
    ASSERT(found);
    --m_SkillPointsSpent;
}

std::string LevelTraining::FeatName(TrainableFeatTypes type) const
{
    return m_TrainedFeats.FeatName(type);
}

const FeatsListObject & LevelTraining::AutomaticFeats() const
{
    return m_AutomaticFeats;
}

void LevelTraining::Set_AutomaticFeats(const FeatsListObject & newValue)
{
    m_AutomaticFeats = newValue;
}
