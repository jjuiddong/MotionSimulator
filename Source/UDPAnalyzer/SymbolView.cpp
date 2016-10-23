// SymbolView.cpp : implementation file
//

#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "SymbolView.h"
#include "afxdialogex.h"


// CSymbolView dialog
CSymbolView::CSymbolView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SYMBOL, pParent)
	, m_incTime(0)
	, m_incSymbolUpdateTime(0)
	, m_symbolCount(0)
	, m_checkUpdateTime(0)
	, m_updateFPS(0)
	, m_IsUpdateSymbolList(TRUE)
{

}

CSymbolView::~CSymbolView()
{
}

void CSymbolView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SYMBOL, m_SymbolList);
	DDX_Control(pDX, IDC_STATIC_FPS, m_staticUpdateFPS);
	DDX_Check(pDX, IDC_CHECK_SYMBOL, m_IsUpdateSymbolList);
}


BEGIN_MESSAGE_MAP(CSymbolView, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSymbolView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSymbolView::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SYMBOL, &CSymbolView::OnBnClickedCheckSymbol)
END_MESSAGE_MAP()


BEGIN_ANCHOR_MAP(CSymbolView)
	ANCHOR_MAP_ENTRY(IDC_LIST_SYMBOL, ANF_LEFT | ANF_RIGHT | ANF_TOP | ANF_BOTTOM)
END_ANCHOR_MAP()


// CSymbolView message handlers
void CSymbolView::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}

void CSymbolView::OnBnClickedCancel()
{
	//CDialogEx::OnCancel();
}


BOOL CSymbolView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitAnchors();

	m_SymbolList.InsertColumn(0, L"Name");
	m_SymbolList.InsertColumn(1, L"Value");
	m_SymbolList.SetColumnWidth(0, 100);
	m_SymbolList.SetColumnWidth(1, 200);

	return TRUE;
}


void CSymbolView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);
}

void CSymbolView::OnBnClickedCheckSymbol()
{
	UpdateData();
}


void CSymbolView::Update(const float deltaSeconds)
{
	if (!IsWindowVisible())
		return;

	m_incTime += deltaSeconds;

	if (m_incTime > 0.033f)
	{
		// Update Symbol Information
		if (m_IsUpdateSymbolList)
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


// 심볼리스트를 생성한다.
// 복사본도 같이 생성한다.
void CSymbolView::InitSymbolList()
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


void CSymbolView::UpdateSymbolList(const float deltaSeconds)
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
		// update if different from previous data
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
