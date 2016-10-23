#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSymbolView dialog

class CSymbolView : public CDialogEx
{
public:
	CSymbolView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSymbolView();

	enum { IDD = IDD_DIALOG_SYMBOL };
	
	void Update(const float deltaSeconds);


protected:
	float m_incTime;
	float m_checkUpdateTime;
	int m_updateFPS; // Frame Per Seconds

	int m_symbolCount;
	map<string, int> m_symLookup;
	map<string, script::sFieldData> m_cloneSymbols;
	float m_incSymbolUpdateTime;


protected:
	void InitSymbolList();
	void UpdateSymbolList(const float deltaSeconds);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_ANCHOR_MAP();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CListCtrl m_SymbolList;
	CStatic m_staticUpdateFPS;
	BOOL m_IsUpdateSymbolList;
	afx_msg void OnBnClickedCheckSymbol();
};
