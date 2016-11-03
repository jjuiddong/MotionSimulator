// PlotView.cpp : implementation file
//

#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "PlotView.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CPlotView dialog

CPlotView::CPlotView(CWnd* pParent /*=NULL*/)
	: CDockablePaneChildView(CPlotView::IDD, pParent)
	, m_incTime(0)
	, m_multiPlotWindows(NULL)
	, m_isStart(false)
	, m_plotId(0)
{

}

CPlotView::~CPlotView()
{
}

void CPlotView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePaneChildView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLOTINPUT, m_PlotInputCommandEditor);
	DDX_Control(pDX, IDC_EDIT_COMMAND, m_PlotCommandEditor);
}



BEGIN_ANCHOR_MAP(CPlotView)
	ANCHOR_MAP_ENTRY(IDC_STATIC_PLOT, ANF_LEFT | ANF_RIGHT | ANF_TOP | ANF_BOTTOM)
	ANCHOR_MAP_ENTRY(IDC_EDIT_PLOTINPUT, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_COMMAND, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_BUTTON_NEWPLOTWINDOW, ANF_RIGHT | ANF_TOP)
END_ANCHOR_MAP()


BEGIN_MESSAGE_MAP(CPlotView, CDockablePaneChildView)
	ON_BN_CLICKED(IDOK, &CPlotView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPlotView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CPlotView::OnBnClickedButtonUpdate)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_NEWPLOTWINDOW, &CPlotView::OnBnClickedButtonNewplotwindow)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPlotView message handlers
BOOL CPlotView::OnInitDialog()
{
	CDockablePaneChildView::OnInitDialog();

	InitAnchors();

	UpdateConfig();

	// Plot창 생성.
	CRect rect;
	GetClientRect(rect);

	m_multiPlotWindows = new CMultiPlotWindow();
	BOOL result = m_multiPlotWindows->Create(NULL, NULL, WS_VISIBLE | WS_CHILD,
		CRect(0, 0, 20, 40), this, AFX_IDW_PANE_FIRST);

	m_multiPlotWindows->SetScrollSizes(MM_TEXT, CSize(rect.Width() - 30, 100));
	m_multiPlotWindows->ShowWindow(SW_SHOW);
	
	return TRUE;
}


void CPlotView::OnBnClickedOk()
{
}


void CPlotView::OnBnClickedCancel()
{
}


void CPlotView::OnBnClickedButtonUpdate()
{
	UpdateData();

	CString command;
	m_PlotCommandEditor.GetWindowTextW(command);
	m_multiPlotWindows->ProcessPlotCommand(command, 4);
	m_multiPlotWindows->SetFixedWidthMode(true);

	CString inputCmd;
	m_PlotInputCommandEditor.GetWindowTextW(inputCmd);
	ParsePlotInputStringFormat(wstr2str((LPCTSTR)inputCmd), m_plotInputParser);

	m_isStart = true;
}


void CPlotView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePaneChildView::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);

	if (m_multiPlotWindows && m_multiPlotWindows->GetSafeHwnd())
	{
		CRect pwr;
		GetDlgItem(IDC_STATIC_PLOT)->GetWindowRect(pwr);
		ScreenToClient(pwr);
		m_multiPlotWindows->MoveWindow(pwr);
	}
}


void CPlotView::Update(const float deltaSeconds)
{
	RET(!m_isStart);

	const float elapseT = 0.033f;
	m_incTime += deltaSeconds;

	if (m_incTime > elapseT)
	{
		for (u_int i = 0; i < m_plotInputParser.size(); ++i)
		{
			const string str = m_plotInputParser[i].Execute();
			m_multiPlotWindows->SetString(str.c_str(), i);
		}

		m_multiPlotWindows->DrawGraph(m_incTime);
		m_incTime = 0;
	}
}


void CPlotView::OnDestroy()
{
	SaveConfig();

	CDockablePaneChildView::OnDestroy();
}

// UI에 설정된 값을 환경변수에 저장한다.
void CPlotView::SaveConfig()
{
	if (m_plotId >= 10)
		return;

	UpdateData();

	// 환경파일 저장
	{
		CString command;
		m_PlotCommandEditor.GetWindowTextW(command);
		g_option.m_plotViewCmd[m_plotId] = wstr2str((LPCTSTR)command);
	}

	{
		CString command;
		m_PlotInputCommandEditor.GetWindowTextW(command);
		g_option.m_plotInputCmd[m_plotId] = wstr2str((LPCTSTR)command);
	}
}


// 전역변수 g_option 정보를 토대로, UI 를 업데이트 한다.
void CPlotView::UpdateConfig()
{
	{
		CString command =
			L"plot1 = 0, 0, 0, 0, 0\r\n"
			L"string1 = %f;\r\n"
			L"name1 = Yaw\r\n"
			L"plot2 = 0, 0, 0, 0, 0\r\n"
			L"string2 = %*f; %f;\r\n"
			L"name2 = Pitch\r\n"
			L"plot3 = 0, 0, 0, 0, 0\r\n"
			L"string3 = %*f; %*f; %f; \r\n"
			L"name3 = Roll\r\n"
			L"plot4 = 0, 0, 0, 0, 0\r\n"
			L"string4 = %*f; %*f; %*f; %f;\r\n"
			L"name4 = Heave\r\n";

		CString cmdStr;
		if ((m_plotId < 10) && !g_option.m_plotViewCmd.empty() && !g_option.m_plotViewCmd[m_plotId].empty())
		{
			cmdStr = str2wstr(g_option.m_plotViewCmd[m_plotId]).c_str();
		}
		else
		{
			cmdStr = command;
		}
		m_PlotCommandEditor.SetWindowTextW(cmdStr);
	}


	//-----------------------------------------------------------------------------------
	{
		CString outCmd = L"$1;$2;$3;";
		CString cmdStr;
		if ((m_plotId < 10) && !g_option.m_plotInputCmd.empty() && !g_option.m_plotInputCmd[m_plotId].empty())
		{
			cmdStr = str2wstr(g_option.m_plotInputCmd[m_plotId]).c_str();
		}
		else
		{
			cmdStr = outCmd;
		}
		m_PlotInputCommandEditor.SetWindowTextW(cmdStr);
	}

	UpdateData(FALSE);
}


// 새 Plot 윈도우를 생성한다.
void CPlotView::OnBnClickedButtonNewplotwindow()
{
	CMainFrame *mainFrm = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
	if (NULL != mainFrm)
	{
		mainFrm->NewPlotWindow();
	}
}


void CPlotView::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDockablePaneChildView::OnClose();
}
