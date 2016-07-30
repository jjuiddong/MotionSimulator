#pragma once
#include "afxwin.h"


// CVarModulationView dialog
class CVarModulationView : public CDockablePaneChildView
{
public:
	CVarModulationView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVarModulationView();

// Dialog Data
	enum { IDD = IDD_DIALOG_MODULATION };

	virtual void UpdateConfig(bool IsSaveAndValidate = true) override;
	virtual void Update(const float deltaSeconds) override;
	void Start();
	void Stop();


protected:
	void UpdateVariable();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	enum STATE{
		STOP,
		START,
	};

	STATE m_state;
	cMathParser m_inputVarParser1;
	cMathParser m_inputVarParser2;
	cMathParser m_inputVarParser3;
	cMathParser m_inputVarParser4;
	mathscript::cMathInterpreter m_interpreter;

	string m_inputVar1;
	string m_outputVar1;
	string m_inputVar2;
	string m_outputVar2;
	string m_inputVar3;
	string m_outputVar3;
	string m_inputVar4;
	string m_outputVar4;

	float m_incTime;
	int m_scrollSize;
	int m_scrollPos;

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_editInputVar;
	CEdit m_editInputVar2;
	CEdit m_editInputVar3;
	CEdit m_editInputVar4;
	CEdit m_editOutputVar;
	CEdit m_editOutputVar2;
	CEdit m_editOutputVar3;
	CEdit m_editOutputVar4;
	CEdit m_editScript;
	CEdit m_editScript2;
	CEdit m_editScript3;
	CEdit m_editScript4;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonStart();
	CButton m_StartButton;
	CButton m_UpdateButton;
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};