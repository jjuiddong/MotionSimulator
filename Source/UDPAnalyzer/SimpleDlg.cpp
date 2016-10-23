// SimpleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "SimpleDlg.h"
#include "afxdialogex.h"
#include "SymbolView.h"


// CSimpleDlg dialog
CSimpleDlg::CSimpleDlg(CWnd *pParent, const int type, const CString &windowName)
	: CDialogEx(CSimpleDlg::IDD, pParent)
	, m_type(type)
	, m_symbolWindow(NULL)
	, m_dumpWindow(NULL)
	, m_windowName(windowName)
{
	
}

CSimpleDlg::~CSimpleDlg()
{
}

void CSimpleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSimpleDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSimpleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSimpleDlg::OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_ANCHOR_MAP(CSimpleDlg)
	ANCHOR_MAP_ENTRY(IDC_STATIC_PICTURE, ANF_LEFT | ANF_RIGHT | ANF_TOP | ANF_BOTTOM)
END_ANCHOR_MAP()

// CSimpleDlg message handlers


void CSimpleDlg::OnBnClickedOk()
{
//	CDialogEx::OnOK();
}
void CSimpleDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CSimpleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitAnchors();

	CRect rect;
	GetClientRect(rect);

	CRect pwr;
	GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(pwr);
	ScreenToClient(pwr);

	if (m_type == 0)
	{
		m_symbolWindow = new CSymbolView();
		m_symbolWindow->Create(CSymbolView::IDD, this);
		m_symbolWindow->MoveWindow(pwr);
		m_symbolWindow->ShowWindow(SW_SHOW);
	}
	else
	{
		m_dumpWindow = new cMemDumpWindow();
		BOOL result = m_dumpWindow->Create(NULL, NULL, WS_VISIBLE | WS_CHILD,
			CRect(0, 0, 20, 40), this, AFX_IDW_PANE_FIRST);
		m_dumpWindow->SetScrollSizes(MM_TEXT, CSize(rect.Width() - 30, 100));
		m_dumpWindow->MoveWindow(pwr);
		m_dumpWindow->ShowWindow(SW_SHOW);
	}

	SetWindowText(m_windowName);

	return TRUE;
}


void CSimpleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);

	if ((m_symbolWindow && m_symbolWindow->GetSafeHwnd()) ||
		(m_dumpWindow && m_dumpWindow->GetSafeHwnd()))
	{
		CRect pwr;
		GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(pwr);
		ScreenToClient(pwr);
		if (m_dumpWindow)
			m_dumpWindow->MoveWindow(pwr);
		if (m_symbolWindow)
			m_symbolWindow->MoveWindow(pwr);
	}
}
