// DCButton.h
//

#pragma once

#include "Resource.h"
#include <string>
#include "DC.h"
#include "Character.h"

// this window handles selection of stances and enhancement sub-option
class CDCButton :
    public CStatic,
    public CharacterObserver
{
    public:
        CDCButton(Character * charData, const DC & dc);

        const DC & GetDCItem() const;

        void AddStack();
        void RevokeStack();
        size_t NumStacks() const;
        bool IsYou(const DC & dc);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CDCButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CDCButton)
        //}}AFX_VIRTUAL

        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateAbilityValueChanged(Character * charData, AbilityType ability) override;
        virtual void UpdateAbilityTomeChanged(Character * charData, AbilityType ability) override;
        virtual void UpdateRaceChanged(Character * charData, RaceType race) override;
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementTreeReset(Character * charData) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateGrantedFeatsChanged(Character * charData) override;
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) override;

        //{{AFX_MSG(CDCButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        Character * m_pCharacter;
        DC m_dc;
        CImage m_image;
        size_t m_stacks;
};

//{{AFX_INSERT_LOCATION}}
