// SpellLikeAbilityPage.h
//
#pragma once

#include "Resource.h"
#include "Character.h"
#include "SLAControl.h"

class CSpellLikeAbilityPage :
    public CPropertyPage,
    public CharacterObserver
{
    public:
        CSpellLikeAbilityPage();
        ~CSpellLikeAbilityPage();

        void SetCharacter(Character * pCharacter);
        const CSLAControl * GetSLAControl();

    protected:
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;

    private:
        //{{AFX_VIRTUAL(CSpellLikeAbilityPage)
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange(CDataExchange* pDX);
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CSpellLikeAbilityPage)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        //{{AFX_DATA(CSpellLikeAbilityPage)
        enum { IDD = IDD_SLA_PAGE };
        CSLAControl m_slas;
        //}}AFX_DATA

        Character * m_pCharacter;
};

//{{AFX_INSERT_LOCATION}}
