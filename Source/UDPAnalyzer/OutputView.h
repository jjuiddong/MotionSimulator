#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// COutputView dialog

class COutputView : public CDockablePaneChildView
{
public:
	COutputView(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutputView();

// Dialog Data
	enum { IDD = IDD_DIALOG_OUTPUT };

	virtual void Update(const float deltaSeconds) override;
	void UpdateConfig();
	void SaveConfig();


protected:
	float m_incTime;
	bool m_isStart;
	float m_incSerialTime;
	float m_incUDPTime;

	cPlotInputParser m_sendFormatParser;

	mathscript::cMathParser m_rollParser;
	mathscript::cMathParser m_pitchParser;
	mathscript::cMathParser m_yawParser;
	mathscript::cMathParser m_heaveParser;
	mathscript::cMathParser m_roll2Parser;
	mathscript::cMathParser m_pitch2Parser;
	mathscript::cMathParser m_yaw2Parser;
	mathscript::cMathParser m_heave2Parser;

	mathscript::cMathInterpreter m_interpreter;

	network::cUDPClient m_udpSendClient;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	string GetSendIP();


	DECLARE_ANCHOR_MAP();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadioSerial();
	afx_msg void OnBnClickedRadioUdp();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedRadioNone();
	CEdit m_EditUDPPort;
	CIPAddressCtrl m_UDPIP;
	CComPortCombo m_ComPort;
	CComboBox m_BaudRateCombobox;
	CString m_SendFormat;
	CStatic m_SendString;
	CString m_RollCommand;
	CString m_PitchCommand;
	CString m_YawCommand;
	CString m_HeaveCommand;
	CString m_Roll2Command;
	CString m_Pitch2Command;
	CString m_Yaw2Command;
	CString m_Heave2Command;
	int m_SendType;
	afx_msg void OnBnClickedButton3dupdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonUpdateSendformat();
	CButton m_ConnectButton;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedRadioYawRollPitch1();
	afx_msg void OnBnClickedRadioRollYawPitch1();
	int m_radio3DCalcOrder1;
	int m_radio3DCalcOrder2;
	afx_msg void OnBnClickedRadioYawRollPitch2();
	afx_msg void OnBnClickedRadioRollYawPitch2();
	BOOL m_checkSendBinary;
	afx_msg void OnBnClickedCheckSendBinary();
	afx_msg void OnBnClickedRadioQuaternion1();
	afx_msg void OnBnClickedRadioQuaternion2();
};
