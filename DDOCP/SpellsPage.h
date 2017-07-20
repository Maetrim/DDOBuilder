// SpellsPage.h
//
#pragma once

#include "Resource.h"
#include "AbilityTypes.h"
#include "ClassTypes.h"
#include "SpellsControl.h"

class CSpellsPage :
    public CPropertyPage,
    public CharacterObserver
{
    public:
        CSpellsPage(ClassType ct, AbilityType ability, UINT strResourceId);
        ~CSpellsPage();

        void SetCharacter(Character * pCharacter);
        void SetTrainableSpells(const std::vector<size_t> & spellsPerLevel);

    protected:
        virtual void UpdateSpellTrained(Character * charData, const TrainedSpell & spell) override;
        virtual void UpdateSpellRevoked(Character * charData, const TrainedSpell & spell) override;
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;

    private:
        //{{AFX_VIRTUAL(CSpellsPage)
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange(CDataExchange* pDX);
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CSpellsPage)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        //{{AFX_DATA(CSpellsPage)
        enum { IDD = IDD_SPELLS_PAGE };
        CSpellsControl m_spells;
        //}}AFX_DATA

        Character * m_pCharacter;
        ClassType m_classType;
        AbilityType m_abilityType;
};

//{{AFX_INSERT_LOCATION}}
