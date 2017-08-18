// ImmunitiesPage.h
//
#pragma once

#include "Resource.h"
#include "Character.h"

class ImmunityItem
{
    public:
        ImmunityItem(const std::string & immunity, const std::string & source) :
                m_immunity(immunity), m_source(source) {};
        ~ImmunityItem() {};

        const std::string & Immunity() const { return m_immunity; };
        const std::string & Source() const { return m_source; };

        bool operator==(const ImmunityItem & other) const
        {
            return m_immunity == other.m_immunity
                    && m_source == other.m_source;
        };
    private:
        std::string m_immunity;
        std::string m_source;
};

class CImmunitiesPage :
    public CPropertyPage,
    public CharacterObserver
{
    public:
        CImmunitiesPage();
        ~CImmunitiesPage();

        void SetCharacter(Character * pCharacter);

    protected:
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;

    private:
        //{{AFX_VIRTUAL(CImmunitiesPage)
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange(CDataExchange* pDX);
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CImmunitiesPage)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnEndtrackImmunityList(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        //{{AFX_DATA(CImmunitiesPage)
        enum { IDD = IDD_IMMUNITIES_PAGE };
        CListCtrl m_listImmunity;
        //}}AFX_DATA

        void Populate();
        void AddImmunity(const std::string & immunity, const std::string & source);
        void RevokeImmunity(const std::string & immunity, const std::string & source);
        Character * m_pCharacter;
        std::list<ImmunityItem> m_immunities;
};

//{{AFX_INSERT_LOCATION}}
