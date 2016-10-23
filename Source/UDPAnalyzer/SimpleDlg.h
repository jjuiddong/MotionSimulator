#pragma once


// CSimpleDlg dialog
class CSymbolView;
class cMemDumpWindow;

class CSimpleDlg : public CDialogEx
{
public:
	CSimpleDlg(CWnd *pParent, const int type, const CString &windowName);   // standard constructor
	virtual ~CSimpleDlg();
	enum { IDD = IDD_DIALOG_SIMPLE };


public:
	int m_type; // symbol Window, dump Window
	CString m_windowName;
	CSymbolView *m_symbolWindow;
	cMemDumpWindow *m_dumpWindow;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_ANCHOR_MAP();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
