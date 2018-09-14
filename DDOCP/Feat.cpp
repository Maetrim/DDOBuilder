// Feat.cpp
//
#include "StdAfx.h"
#include "Feat.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Feat

namespace
{
    const wchar_t f_saxElementName[] = L"Feat";
    DL_DEFINE_NAMES(Feat_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Feat::Feat() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Feat_PROPERTIES)
}

DL_DEFINE_ACCESS(Feat_PROPERTIES)

XmlLib::SaxContentElementInterface * Feat::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Feat_PROPERTIES)

    return subHandler;
}

void Feat::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Feat_PROPERTIES)
    if (!m_hasMaxTimesAcquire)
    {
        // default to 1for feats its not specified for
        m_hasMaxTimesAcquire = true;
        m_MaxTimesAcquire = 1;
    }
}

void Feat::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Feat_PROPERTIES)
    writer->EndElement();
}

void Feat::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    m_RequirementsToTrain.CreateRequirementStrings(charData, requirements, met, level);
}

bool Feat::operator<(const Feat & other) const
{
    // (assumes all feat names are unique)
    // sort by name
    return (Name() < other.Name());
}

void Feat::AddImage(CImageList * pIL) const
{
    CImage image;
    HRESULT result = LoadImageFile(
            IT_feat,
            m_Icon,
            &image);
    if (result == S_OK)
    {
        CBitmap bitmap;
        bitmap.Attach(image.Detach());
        pIL->Add(&bitmap, c_transparentColour);  // standard mask color (purple)
    }
}

void Feat::VerifyObject(
        const std::list<EnhancementTree> & allTrees,
        const std::list<Feat> & allfeats) const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    // check this feat out for any inconsistencies
    if (HasGroup())
    {
        ok &= m_Group.VerifyObject(&ss);
    }
    ok &= m_RequirementsToTrain.VerifyObject(&ss, allTrees, allfeats);
    std::list<AutomaticAcquisition>::const_iterator it = m_AutomaticAcquireAt.begin();
    while (it != m_AutomaticAcquireAt.end())
    {
        ok &= (*it).VerifyObject(&ss);
        ++it;
    }
    // check the effects also
    std::list<Effect>::const_iterator eit = m_Effects.begin();
    while (eit != m_Effects.end())
    {
        ok &= (*eit).VerifyObject(&ss);
        ++eit;
    }
    // verify its DC objects
    std::list<DC>::const_iterator edcit = m_EffectDC.begin();
    while (edcit != m_EffectDC.end())
    {
        ok &= (*edcit).VerifyObject(&ss);
        ++edcit;
    }

    if (!ImageFileExists(IT_feat, Icon()))
    {
        ss << "Feat is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}

