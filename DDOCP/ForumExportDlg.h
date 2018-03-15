// ForumExportDlg.h
//
#include "Resource.h"

class CForumExportDlg : public CDialogEx
{
    public:
        CForumExportDlg();

        //{{AFX_DATA(CForumExportDlg)
        enum { IDD = IDD_FORUM_EXPORT_DIALOG };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CForumExportDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CForumExportDlg)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    private:
        CListCtrl m_listConfigureExport;
        CEdit m_editExport;
};

