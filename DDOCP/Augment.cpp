// Augment.cpp
//
#include "StdAfx.h"
#include "Augment.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Augment

namespace
{
    const wchar_t f_saxElementName[] = L"Augment";
    DL_DEFINE_NAMES(Augment_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Augment::Augment() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Augment_PROPERTIES)
}

DL_DEFINE_ACCESS(Augment_PROPERTIES)

XmlLib::SaxContentElementInterface * Augment::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Augment_PROPERTIES)

    return subHandler;
}

void Augment::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Augment_PROPERTIES)
}

void Augment::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Augment_PROPERTIES)
    writer->EndElement();
}

bool Augment::IsCompatibleWithSlot(const std::string & augmentType) const
{
    // has to be in any of the augment types list to be a match
    bool compatible = false;
    std::list<std::string>::const_iterator it = m_Type.begin();
    while (it != m_Type.end())
    {
        if ((*it) == augmentType)
        {
            compatible = true;
            break;  // no need to check the rest
        }
        ++it;
    }
    return compatible;
}

void Augment::AddImage(CImageList * pIL) const
{
    CImage image;
    if (HasIcon())
    {
        HRESULT result = LoadImageFile(
                IT_augment,
                m_Icon,
                &image);
        if (result == S_OK)
        {
            CBitmap bitmap;
            bitmap.Attach(image.Detach());
            pIL->Add(&bitmap, c_transparentColour);  // standard mask color (purple)
        }
    }
    else
    {
        HRESULT result = LoadImageFile(
                IT_augment,
                "unknown",
                &image);
        if (result == S_OK)
        {
            CBitmap bitmap;
            bitmap.Attach(image.Detach());
            pIL->Add(&bitmap, c_transparentColour);  // standard mask color (purple)
        }
    }
}

void Augment::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (HasIcon())
    {
        if (!ImageFileExists(IT_augment, Icon()))
        {
            ss << "Augment is missing image file \"" << Icon() << "\"\n";
            ok = false;
        }
    }
    // check the spell effects also
    std::list<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&ss);
        ++it;
    }
    if (HasRares())
    {
        ok &= m_Rares.VerifyObject(&ss);
    }

    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}
