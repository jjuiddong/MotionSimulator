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

	virtual void UpdateConfig(bool IsSaveAndValidate = true) override;
	void Mixing(const float deltaSeconds, 
		const float yaw0, const float pitch0, const float roll0, const float heave0, const float speed0,
		OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave, OUT float &speed, 
		OUT float &yaw1, OUT float &pitch1, OUT float &roll1,
		OUT float &norm1, OUT float &norm2, OUT float &norm3);
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Update(const float deltaSeconds) override;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	enum STATE {
		STOP,
		START,
	};

	STATE m_state;
	CMultiPlotWindow *m_multiPlotWindows;
	float m_incTime;
	float m_totalIncTime;
	cMixingConfig m_config;


	DECLARE_ANCHOR_MAP();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonUpdate();
	CRichEditCtrl m_EditCommand;
	afx_msg void OnDestroy();
	CButton m_StartButton;
};
