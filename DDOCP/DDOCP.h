// DDOCP.h
//
#pragma once
#include <list>
#include "Augment.h"
#include "EnhancementTree.h"
#include "Feat.h"
#include "GuildBuff.h"
#include "Item.h"
#include "Spell.h"

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "CustomContextMenuManager.h"

class CDDOCPApp :
    public CWinAppEx
{
    public:
        CDDOCPApp();

        UINT  m_nAppLook;
        BOOL  m_bHiColorIcons;
        int m_originalShowState;

        virtual BOOL InitInstance();
        virtual int ExitInstance();

        const std::list<Feat> & AllFeats() const;
        const std::list<Feat> & StandardFeats() const;
        const std::list<Feat> & HeroicPastLifeFeats() const;
        const std::list<Feat> & RacialPastLifeFeats() const;
        const std::list<Feat> & IconicPastLifeFeats() const;
        const std::list<Feat> & EpicPastLifeFeats() const;
        const std::list<Feat> & SpecialFeats() const;

        const std::list<EnhancementTree> & EnhancementTrees() const;
        const std::list<Spell> & Spells() const;
        const std::list<Item> & Items() const;
        const std::list<Augment> & Augments() const;
        const std::list<GuildBuff> & GuildBuffs() const;

        virtual void PreLoadState();
        virtual void LoadCustomState();
        virtual void SaveCustomState();

        afx_msg void OnAppAbout();
        virtual BOOL LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd) override;
        DECLARE_MESSAGE_MAP()

    private:
        void LoadData();
        void LoadFeats(const std::string & path);
        void LoadEnhancements(const std::string & path);
        void LoadSpells(const std::string & path);
        void LoadItems(const std::string & path);
        void LoadAugments(const std::string & path);
        void LoadGuildBuffs(const std::string & path);
        void VerifyFeats();     // verify that all feats are consistent
        void SeparateFeats();   // breaks them out into groups based on type
        void VerifyEnhancements();     // verify that all enhancements are consistent
        void VerifyAugments();     // verify that all augments are consistent
        void VerifySpells();    // verify all spells are consistent
        void VerifyItems();    // verify all items are consistent
        CCustomContextMenuManager m_ourMenuManager; // construction of object replaces default implementation created in InitContextMenuManager
        // global data loaded at start up
        std::list<Feat> m_allFeats;
        std::list<Feat> m_standardFeats;
        std::list<Feat> m_heroicPastLifeFeats;
        std::list<Feat> m_racialPastLifeFeats;
        std::list<Feat> m_iconicPastLifeFeats;
        std::list<Feat> m_epicPastLifeFeats;
        std::list<Feat> m_specialFeats;
        std::list<EnhancementTree> m_enhancementTrees;
        std::list<Spell> m_spells;
        std::list<Item> m_items;
        std::list<Augment> m_augments;
        std::list<GuildBuff> m_guildBuffs;
};

extern CDDOCPApp theApp;
