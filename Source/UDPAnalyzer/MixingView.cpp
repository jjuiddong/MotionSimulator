// MixingView.cpp : implementation file
//

#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "MixingView.h"
#include "afxdialogex.h"
#include "SimpleDlg.h"
#include "SymbolView.h"


// CMixingView dialog
CMixingView::CMixingView(CWnd* pParent /*=NULL*/)
	: CDockablePaneChildView(CMixingView::IDD, pParent)
	, m_incTime(0)
	, m_incSymbolUpdateTime(0)
	, m_IsUpdateSymbolList(TRUE)
	, m_symbolCount(0)
	, m_checkUpdateTime(0)
	, m_updateFPS(0)
	, m_dlgSymbolList(NULL)
{
}

CMixingView::~CMixingView()
{
	SAFE_DELETE(m_dlgSymbolList);
}

void CMixingView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePaneChildView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMMAND, m_CommandEditor);
	DDX_Control(pDX, IDC_LIST_SYMBOL, m_SymbolList);
	DDX_Check(pDX, IDC_CHECK_SYMBOL, m_IsUpdateSymbolList);
	DDX_Control(pDX, IDC_STATIC_FPS, m_staticUpdateFPS);
}

BEGIN_ANCHOR_MAP(CMixingView)
	ANCHOR_MAP_ENTRY(IDC_EDIT_COMMAND, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_BUTTON_DOCKING, ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_LIST_SYMBOL, ANF_LEFT | ANF_RIGHT | ANF_TOP | ANF_BOTTOM)
END_ANCHOR_MAP()

BEGIN_MESSAGE_MAP(CMixingView, CDockablePaneChildView)
	ON_BN_CLICKED(IDOK, &CMixingView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMixingView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CMixingView::OnBnClickedButtonUpdate)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_SYMBOL, &CMixingView::OnBnClickedCheckSymbol)
	ON_BN_CLICKED(IDC_BUTTON_DOCKING, &CMixingView::OnBnClickedButtonDocking)
END_MESSAGE_MAP()


// CMixingView message handlers

BOOL CMixingView::OnInitDialog()
{
	CDockablePaneChildView::OnInitDialog();

	InitAnchors();

	UpdateConfig();

	m_SymbolList.InsertColumn(0, L"Name");
	m_SymbolList.InsertColumn(1, L"Value");
	m_SymbolList.SetColumnWidth(0, 100);
	m_SymbolList.SetColumnWidth(1, 200);

	return TRUE;
}


// 전역변수 g_option 정보를 토대로, UI 를 업데이트 한다.
void CMixingView::UpdateConfig()
{
	CString command =
		L"$var1 = $1 + $2 + $3\r\n"
		L"$var2 = $1 + $2 + $3\r\n"
		L"$var3 = $1 + $2 + $3\r\n";

	CString cmdStr;
	if (!g_option.m_mixingCmd.empty())
	{
		cmdStr = str2wstr(g_option.m_mixingCmd).c_str();
	}
	else
	{
		cmdStr = command;
	}
	m_CommandEditor.SetWindowTextW(cmdStr);

//	m_parser.ParseStr(wstr2str((LPCTSTR)cmdStr));

	UpdateData(FALSE);
}


void CMixingView::OnBnClickedOk()
{
}


void CMixingView::OnBnClickedCancel()
{
}


void CMixingView::OnBnClickedButtonUpdate()
{
	UpdateData();

	CString text;
	m_CommandEditor.GetWindowTextW(text);
	m_parser.ParseStr(wstr2str((LPCTSTR)text));

	script::ClearSymbols();
}


void CMixingView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePaneChildView::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);
}


void CMixingView::Update(const float deltaSeconds)
{
	m_incTime += deltaSeconds;

	if (m_dlgSymbolList)
		m_dlgSymbolList->m_symbolWindow->Update(deltaSeconds);

	if (m_incTime > 0.033f)
	{
		m_interpreter.Excute(m_parser.m_stmt);

		if (m_IsUpdateSymbolList && IsWindowVisible())
		{
			if (script::g_symbols.size() != m_symbolCount)
			{
				InitSymbolList();
				m_symbolCount = script::g_symbols.size();
			}
			else
			{
				UpdateSymbolList(m_incTime);
			}
		}

		m_incTime = 0;
	}
}


void CMixingView::OnDestroy()
{
	SaveConfig();
	CDockablePaneChildView::OnDestroy();
}

// UI에 설정된 값을 환경변수에 저장한다.
void CMixingView::SaveConfig()
{
	UpdateData();

	// 환경파일 저장
	CString command;
	m_CommandEditor.GetWindowTextW(command);
	g_option.m_mixingCmd = wstr2str((LPCTSTR)command);
}


void CMixingView::OnBnClickedCheckSymbol()
{
	UpdateData();	
}


// 심볼리스트를 생성한다.
// 복사본도 같이 생성한다.
void CMixingView::InitSymbolList()
{
	m_SymbolList.DeleteAllItems();
	m_symLookup.clear();

	int item = 0;
	for each (auto sym in script::g_symbols)
	{
		m_SymbolList.InsertItem(item, str2wstr(sym.first).c_str());
		m_symLookup[sym.first] = item;
		++item;
	}

	// 똑같은 값을 복사해 놓는다.
	m_cloneSymbols = script::g_symbols;
}


void CMixingView::UpdateSymbolList(const float deltaSeconds)
{
	using namespace script;
	
	m_incSymbolUpdateTime += deltaSeconds;
	m_checkUpdateTime += deltaSeconds;
	++m_updateFPS;
	if (m_checkUpdateTime > 1)
	{
		CString str;
		str.Format(L"Update FPS : %d", m_updateFPS);
		m_staticUpdateFPS.SetWindowTextW(str);
		m_checkUpdateTime = 0;
		m_updateFPS = 0;
	}

	const bool isAllUpdate = m_incSymbolUpdateTime > 3.f;
	if (isAllUpdate)
		m_incSymbolUpdateTime = 0;

	for each (auto sym in script::g_symbols)
	{
		// 그 전 데이타와 다를 때만 업데이트 한다.
		if (!isAllUpdate && (m_cloneSymbols[sym.first] == sym.second))
			continue;

		CString str;
		switch (sym.second.type)
		{
		case FIELD_TYPE::T_BOOL: str = (sym.second.bVal ? L"true" : L"false"); break;
		case FIELD_TYPE::T_SHORT:
		case FIELD_TYPE::T_UINT:
		case FIELD_TYPE::T_INT: str.Format(L"%d", sym.second.iVal); break;
		case FIELD_TYPE::T_FLOAT: str.Format(L"%f", sym.second.fVal); break;
		case FIELD_TYPE::T_DOUBLE: str.Format(L"%f", sym.second.dVal); break;
		default:
			break;
		}

		const int item = m_symLookup[sym.first];
		m_SymbolList.SetItemText(item, 1, str);
	}
}


void CMixingView::OnBnClickedButtonDocking()
{
	if (!m_dlgSymbolList)
	{
		m_dlgSymbolList = new CSimpleDlg(this, 0, L"SymbolList Window");
		m_dlgSymbolList->Create(CSimpleDlg::IDD, this);
	}
	m_dlgSymbolList->ShowWindow(SW_SHOW);
}

