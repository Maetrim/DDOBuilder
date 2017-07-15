// ItemEditorDialog.h
#pragma once
#include "Resource.h"

class CItemEditorDialog : public CDialog
{
    public:
        CItemEditorDialog(CWnd* pParent);   // standard constructor
        virtual ~CItemEditorDialog();

        // Dialog Data
        enum { IDD = IDD_ITEM_EDITOR_DIALOG };

        DECLARE_DYNAMIC(CItemEditorDialog)

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();

    public:
        DECLARE_MESSAGE_MAP()

    private:
};
