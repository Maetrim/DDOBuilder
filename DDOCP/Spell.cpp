// Spell.cpp
//
#include "StdAfx.h"
#include "Spell.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Spell

namespace
{
    const wchar_t f_saxElementName[] = L"Spell";
    DL_DEFINE_NAMES(Spell_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Spell::Spell() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Spell_PROPERTIES)
}

DL_DEFINE_ACCESS(Spell_PROPERTIES)

XmlLib::SaxContentElementInterface * Spell::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Spell_PROPERTIES)

    return subHandler;
}

void Spell::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Spell_PROPERTIES)
}

void Spell::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Spell_PROPERTIES)
    writer->EndElement();
}

void Spell::AddImage(CImageList * pIL) const
{
    CImage image;
    HRESULT result = LoadImageFile(
            IT_spell,
            m_Icon,
            &image);
    if (result == S_OK)
    {
        CBitmap bitmap;
        bitmap.Attach(image.Detach());
        pIL->Add(&bitmap, RGB(255, 128, 255));  // standard mask color (purple)
    }
}

bool Spell::operator<(const Spell & other) const
{
    // (assumes all spell names are unique)
    // sort by name
    return (Name() < other.Name());
}

size_t Spell::SpellLevel(ClassType ct) const
{
    size_t level = 0;
    switch (ct)
    {
    case Class_Artificer:
        level = Artificer();
        break;
    case Class_Bard:
        level = Bard();
        break;
    case Class_Cleric:
        level = Cleric();
        break;
    case Class_Druid:
        level = Druid();
        break;
    case Class_FavoredSoul:
        level = FavoredSoul();
        break;
    case Class_Paladin:
        level = Paladin();
        break;
    case Class_Ranger:
        level = Ranger();
        break;
    case Class_Sorcerer:
        level = Sorcerer();
        break;
    case Class_Warlock:
        level = Warlock();
        break;
    case Class_Wizard:
        level = Wizard();
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    return level;
}

std::vector<std::string> Spell::Metamagics() const
{
    std::vector<std::string> metas;
    if (HasEmpower())
    {
        metas.push_back("Empower");
    }
    if (HasEmpowerHealing())
    {
        metas.push_back("Empower Healing");
    }
    if (HasEnlarge())
    {
        metas.push_back("Enlarge");
    }
    if (HasExtend())
    {
        metas.push_back("Extend");
    }
    if (HasHeighten())
    {
        metas.push_back("Heighten");
    }
    if (HasMaximize())
    {
        metas.push_back("Maximize");
    }
    if (HasQuicken())
    {
        metas.push_back("Quicken");
    }
    return metas;
}

void Spell::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (!ImageFileExists(IT_spell, Icon()))
    {
        ss << "Spell is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}
