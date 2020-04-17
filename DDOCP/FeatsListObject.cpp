// FeatsListObject.cpp
//
#include "StdAfx.h"
#include "FeatsListObject.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT FeatsListObject

namespace
{
    DL_DEFINE_NAMES(FeatsListObject_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

FeatsListObject::FeatsListObject(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent)
{
    DL_INIT(FeatsListObject_PROPERTIES)
}

DL_DEFINE_ACCESS(FeatsListObject_PROPERTIES)

XmlLib::SaxContentElementInterface * FeatsListObject::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(FeatsListObject_PROPERTIES)

    return subHandler;
}

void FeatsListObject::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(FeatsListObject_PROPERTIES)
}

void FeatsListObject::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(FeatsListObject_PROPERTIES)
    writer->EndElement();
}

void FeatsListObject::Add(const TrainedFeat & feat)
{
    m_Feats.push_back(feat);
}

void FeatsListObject::TrainFeat(
        const std::string & featName,
        TrainableFeatTypes type,
        size_t level,
        bool featSwapWarning)
{
    TrainedFeat tf;
    tf.Set_FeatName(featName);
    tf.Set_Type(type);
    tf.Set_LevelTrainedAt(level);
    if (featSwapWarning)
    {
        tf.Set_FeatSwapWarning();
    }
    m_Feats.push_back(tf);
}

std::string FeatsListObject::RevokeFeat(TrainableFeatTypes type)
{
    std::string revokedFeat;
    // find the feat in the list and remove it
    bool found = false;
    std::list<TrainedFeat>::iterator it = m_Feats.begin();
    while (it != m_Feats.end())
    {
        if ((*it).Type() == type)
        {
            // revoke this one
            revokedFeat = (*it).FeatName();
            it = m_Feats.erase(it);
            found = true;
            break;  // were done
        }
        ++it;
    }
    // we may have tried to revoke a feat that had not been trained. That's ok!
    return revokedFeat;
}

void FeatsListObject::RevokeAllFeats(TrainableFeatTypes type)
{
    // revoke all trained feats of a specific type
    // e.g. all Automatic feats
    std::list<TrainedFeat>::iterator it = m_Feats.begin();
    while (it != m_Feats.end())
    {
        if ((*it).Type() == type)
        {
            // revoke this one
            it = m_Feats.erase(it);
        }
        else
        {
            // keep this feat, skip to next
            ++it;
        }
    }
}

std::string FeatsListObject::FeatName(TrainableFeatTypes type) const
{
    std::list<TrainedFeat>::const_iterator it = m_Feats.begin();
    while (it != m_Feats.end())
    {
        if ((*it).Type() == type)
        {
            return (*it).FeatName();
        }
        ++it;
    }
    return "";
}

