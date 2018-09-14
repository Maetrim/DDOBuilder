// EnhancementSelection.cpp
//
#include "StdAfx.h"
#include "EnhancementSelection.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT EnhancementSelection

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementSelection";
    DL_DEFINE_NAMES(EnhancementSelection_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EnhancementSelection::EnhancementSelection() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_bImageLoaded(false)
{
    DL_INIT(EnhancementSelection_PROPERTIES)
}

DL_DEFINE_ACCESS(EnhancementSelection_PROPERTIES)

XmlLib::SaxContentElementInterface * EnhancementSelection::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EnhancementSelection_PROPERTIES)

    return subHandler;
}

void EnhancementSelection::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EnhancementSelection_PROPERTIES)
}

void EnhancementSelection::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EnhancementSelection_PROPERTIES)
    writer->EndElement();
}

void EnhancementSelection::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    m_RequirementsToTrain.CreateRequirementStrings(charData, requirements, met, level);
}

bool EnhancementSelection::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    if (!ImageFileExists(IT_enhancement, Icon()))
    {
        (*ss) << "EnhancementSelection is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    // check the effects also
    std::list<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(ss);
        ++it;
    }
    // verify its DC objects
    std::list<DC>::const_iterator edcit = m_EffectDC.begin();
    while (edcit != m_EffectDC.end())
    {
        ok &= (*edcit).VerifyObject(ss);
        ++edcit;
    }
    return ok;
}

void EnhancementSelection::RenderIcon(CDC * pDC, const CRect & itemRect) const
{
    if (!m_bImageLoaded)
    {
        // load the display image for this item (never a disabled version)
        LoadImageFile(IT_enhancement, m_Icon, &m_image);
        m_image.SetTransparentColor(c_transparentColour);
        m_bImageLoaded = true;
    }
    m_image.TransparentBlt(
            pDC->GetSafeHdc(),
            itemRect.left,
            itemRect.top,
            itemRect.Width(),
            itemRect.Height());
}

