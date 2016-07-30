#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMixingView dialog
class CMixingView : public CDockablePaneChildView
{
public:
	CMixingView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMixingView();

// Dialog Data
	enum { IDD = IDD_DIALOG_MIXING };

	virtual void Update(const float deltaSeconds) override;
	void UpdateConfig();
	void SaveConfig();


protected:
	cMathParser m_parser;
	mathscript::cMathInterpreter m_interpreter;
	float m_incTime;
	float m_checkUpdateTime;
	int m_updateFPS; // 1�ʴ� ������Ʈ�� Ƚ��

	int m_symbolCount; // �� ���� ���� �ɺ� ������ �ٸ��ٸ�, SymbolList�� ������Ʈ �Ѵ�.
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
	CEdit m_CommandEditor;
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	CListCtrl m_SymbolList;
	afx_msg void OnBnClickedCheckSymbol();
	BOOL m_IsUpdateSymbolList;
	CStatic m_staticUpdateFPS;
};