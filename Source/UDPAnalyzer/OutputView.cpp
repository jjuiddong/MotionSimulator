
#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "OutputView.h"
#include "afxdialogex.h"
#include "udpanalyzeroption.h"


// COutputView dialog
COutputView::COutputView(CWnd* pParent /*=NULL*/)
	: CDockablePaneChildView(COutputView::IDD, pParent)
	, m_RollCommand(_T(""))
	, m_PitchCommand(_T(""))
	, m_YawCommand(_T(""))
	, m_HeaveCommand(_T(""))
	, m_Roll2Command(_T(""))
	, m_Pitch2Command(_T(""))
	, m_Yaw2Command(_T(""))
	, m_Heave2Command(_T(""))
	, m_incTime(0)
	, m_isStart(false)
	, m_SendType(2)
	, m_SendFormat(_T(""))
	, m_incSerialTime(0)
	, m_incUDPTime(0)
	, m_radio3DCalcOrder1(0)
	, m_radio3DCalcOrder2(0)
	, m_checkSendBinary(FALSE)
{
}

COutputView::~COutputView()
{
}

void COutputView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePaneChildView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_UDPIP);
	DDX_Control(pDX, IDC_COMBO_COM, m_ComPort);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_BaudRateCombobox);
	DDX_Text(pDX, IDC_EDIT_ROLL, m_RollCommand);
	DDX_Text(pDX, IDC_EDIT_PITCH, m_PitchCommand);
	DDX_Text(pDX, IDC_EDIT_YAW, m_YawCommand);
	DDX_Text(pDX, IDC_EDIT_HEAVE, m_HeaveCommand);
	DDX_Text(pDX, IDC_EDIT_ROLL2, m_Roll2Command);
	DDX_Text(pDX, IDC_EDIT_PITCH2, m_Pitch2Command);
	DDX_Text(pDX, IDC_EDIT_YAW2, m_Yaw2Command);
	DDX_Text(pDX, IDC_EDIT_HEAVE2, m_Heave2Command);
	DDX_Radio(pDX, IDC_RADIO_SERIAL, m_SendType);
	DDX_Text(pDX, IDC_EDIT_SENDCOMMAND, m_SendFormat);
	DDX_Control(pDX, IDC_STATIC_SENDDATA, m_SendString);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_ConnectButton);
	DDX_Control(pDX, IDC_EDIT_PORT, m_EditUDPPort);
	DDX_Radio(pDX, IDC_RADIO_YAW_ROLL_PITCH1, m_radio3DCalcOrder1);
	DDX_Radio(pDX, IDC_RADIO_YAW_ROLL_PITCH2, m_radio3DCalcOrder2);
	DDX_Check(pDX, IDC_CHECK_SEND_BINARY, m_checkSendBinary);
}


BEGIN_ANCHOR_MAP(COutputView)
	ANCHOR_MAP_ENTRY(IDC_BUTTON_UPDATE_SENDFORMAT, ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_SENDCOMMAND, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_SENDDATA, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_3DGROUP, ANF_LEFT | ANF_RIGHT | ANF_TOP)	
	ANCHOR_MAP_ENTRY(IDC_EDIT_ROLL, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_PITCH, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_YAW, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_HEAVE, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_3DGROUP2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_ROLL2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_PITCH2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_YAW2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_EDIT_HEAVE2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_BUTTON_3DUPDATE, ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_CHECK_SEND_BINARY, ANF_RIGHT | ANF_TOP)
END_ANCHOR_MAP()

BEGIN_MESSAGE_MAP(COutputView, CDockablePaneChildView)
	ON_BN_CLICKED(IDOK, &COutputView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COutputView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_SERIAL, &COutputView::OnBnClickedRadioSerial)
	ON_BN_CLICKED(IDC_RADIO_UDP, &COutputView::OnBnClickedRadioUdp)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &COutputView::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_3DUPDATE, &COutputView::OnBnClickedButton3dupdate)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SENDFORMAT, &COutputView::OnBnClickedButtonUpdateSendformat)
	ON_BN_CLICKED(IDC_RADIO_NONE, &COutputView::OnBnClickedRadioNone)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_YAW_ROLL_PITCH1, &COutputView::OnBnClickedRadioYawRollPitch1)
	ON_BN_CLICKED(IDC_RADIO_ROLL_YAW_PITCH1, &COutputView::OnBnClickedRadioRollYawPitch1)
	ON_BN_CLICKED(IDC_RADIO_YAW_ROLL_PITCH2, &COutputView::OnBnClickedRadioYawRollPitch2)
	ON_BN_CLICKED(IDC_RADIO_ROLL_YAW_PITCH2, &COutputView::OnBnClickedRadioRollYawPitch2)
	ON_BN_CLICKED(IDC_CHECK_SEND_BINARY, &COutputView::OnBnClickedCheckSendBinary)
	ON_BN_CLICKED(IDC_RADIO_QUATERNION1, &COutputView::OnBnClickedRadioQuaternion1)
	ON_BN_CLICKED(IDC_RADIO_QUATERNION2, &COutputView::OnBnClickedRadioQuaternion2)
END_MESSAGE_MAP()


BOOL COutputView::OnInitDialog()
{
	CDockablePaneChildView::OnInitDialog();

	InitAnchors();

	//m_ComPort.InitList();

	const int baudRate[] = { 9600, 14400, 19200, 38400, 56000, 57600, 115200 };
	for (int i = 0; i < ARRAYSIZE(baudRate); ++i)
	{
		TCHAR brateName[32];
		wsprintf(brateName, L"%d", baudRate[i]);
		m_BaudRateCombobox.InsertString(m_BaudRateCombobox.GetCount(), brateName);
	}
	m_BaudRateCombobox.SetCurSel(0);

	m_UDPIP.SetAddress(127, 0, 0, 1);
	m_EditUDPPort.SetWindowTextW(L"8888");

	UpdateConfig();

	// Plot창 생성.

	// default type is none
	m_UDPIP.EnableWindow(FALSE);
	m_EditUDPPort.EnableWindow(FALSE);
	m_ComPort.EnableWindow(FALSE);
	m_BaudRateCombobox.EnableWindow(FALSE);
	switch (m_SendType)
	{
	case 0:
		m_ComPort.EnableWindow(TRUE);
		m_BaudRateCombobox.EnableWindow(TRUE);
		break;
	case 1:
		m_UDPIP.EnableWindow(TRUE);
		m_EditUDPPort.EnableWindow(TRUE);
		break;
	}

	UpdateData(FALSE);

	return TRUE;
}


// 전역변수 g_option 정보를 토대로, UI 를 업데이트 한다.
void COutputView::UpdateConfig()
{
	vector<string> ipnums;
	tokenizer(g_option.m_ip, ".", "", ipnums);
	if (ipnums.size() >= 4)
	{
		m_UDPIP.SetAddress(atoi(ipnums[0].c_str()),
			atoi(ipnums[1].c_str()),
			atoi(ipnums[2].c_str()),
			atoi(ipnums[3].c_str()));
	}
	else
	{
		m_UDPIP.SetAddress(127, 0, 0, 1);
	}

	m_EditUDPPort.SetWindowTextW(formatw("%d", g_option.m_port).c_str());
	m_ComPort.InitList(g_option.m_com);
	m_BaudRateCombobox.SetCurSel(g_option.m_baudRate);
	m_SendType = g_option.m_sendType;

	m_SendFormat = g_option.m_sendFormat.empty() ? L"$yaw;$pitch;$roll;" : str2wstr(g_option.m_sendFormat).c_str();
	m_RollCommand = g_option.m_rollCmd.empty() ? L"$8 - 1.55" : str2wstr(g_option.m_rollCmd).c_str();
	m_PitchCommand = g_option.m_pitchCmd.empty() ? L"$7" : str2wstr(g_option.m_pitchCmd).c_str();
	m_YawCommand = g_option.m_yawCmd.empty() ? L"$6" : str2wstr(g_option.m_yawCmd).c_str();
	m_HeaveCommand = g_option.m_heaveCmd.empty() ? L"$5" : str2wstr(g_option.m_heaveCmd).c_str();

	m_Roll2Command = str2wstr(g_option.m_roll2Cmd).c_str();
	m_Pitch2Command = str2wstr(g_option.m_pitch2Cmd).c_str();
	m_Yaw2Command = str2wstr(g_option.m_yaw2Cmd).c_str();
	m_Heave2Command = str2wstr(g_option.m_heave2Cmd).c_str();

	m_rollParser.ParseStr(wstr2str((LPCTSTR)m_RollCommand));
	m_pitchParser.ParseStr(wstr2str((LPCTSTR)m_PitchCommand));
	m_yawParser.ParseStr(wstr2str((LPCTSTR)m_YawCommand));
	m_heaveParser.ParseStr(wstr2str((LPCTSTR)m_HeaveCommand));
	m_roll2Parser.ParseStr(wstr2str((LPCTSTR)m_Roll2Command));
	m_pitch2Parser.ParseStr(wstr2str((LPCTSTR)m_Pitch2Command));
	m_yaw2Parser.ParseStr(wstr2str((LPCTSTR)m_Yaw2Command));
	m_heave2Parser.ParseStr(wstr2str((LPCTSTR)m_Heave2Command));

	m_sendFormatParser.ParseStr(wstr2str((LPCTSTR)m_SendFormat));

	UpdateData(FALSE);
}


// COutputView message handlers
void COutputView::OnBnClickedOk()
{
}

void COutputView::OnBnClickedCancel()
{
}


void COutputView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePaneChildView::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);
}


void COutputView::Update(const float deltaSeconds)
{
	RET(!m_isStart);

	const float elapseT = 0.033f;
	m_incTime += deltaSeconds;

	if (m_incTime > elapseT)
	{
		const string sendStr = m_sendFormatParser.Execute();
		m_SendString.SetWindowTextW(str2wstr(sendStr).c_str());

		{
			// 3D View1
			const float roll = m_interpreter.Excute(m_rollParser.m_stmt);
			const float pitch = m_interpreter.Excute(m_pitchParser.m_stmt);
			const float yaw = m_interpreter.Excute(m_yawParser.m_stmt);
			const float heave = m_interpreter.Excute(m_heaveParser.m_stmt);

			// radian normalize
			// -pi ~ +pi 내의 각도로 정규화한 각도값(radian) chRoll, chPitch,chYaw 값으로 정보를 업데이트한다.
			Quaternion rotr;
			rotr.Euler2(Vector3(roll, 0, 0));
			Quaternion rotp;
			rotp.Euler2(Vector3(0, 0, pitch));
			Quaternion roty;
			roty.Euler2(Vector3(0, yaw, 0));

			Quaternion rot;
			switch (m_radio3DCalcOrder1)
			{
			case 0: rot = roty * rotr * rotp; break; // Yaw x Roll x Pitch
			case 1: rot = rotr * roty * rotp; break; // Roll x Yaw x Pitch
			case 2: rot = Quaternion(roll, pitch, yaw, heave); break;
			}

			if (m_radio3DCalcOrder1 == 2) // quaternion
			{
				g_3dView->GetCar().m_tm = rot.GetMatrix();
			}
			else
			{
				Vector3 euler = rot.Euler();
				const float chRoll = euler.x;
				const float chYaw = euler.y;
				const float chPitch = euler.z;
				g_3dView->GetCar().SetEulerAngle(chRoll, chPitch, chYaw, heave);
			}
			//
		}


		{
			// 3D View2
			const float roll = m_interpreter.Excute(m_roll2Parser.m_stmt);
			const float pitch = m_interpreter.Excute(m_pitch2Parser.m_stmt);
			const float yaw = m_interpreter.Excute(m_yaw2Parser.m_stmt);
			const float heave = m_interpreter.Excute(m_heave2Parser.m_stmt);

			// radian normalize
			// -pi ~ +pi 내의 각도로 정규화한 각도값(radian) chRoll, chPitch,chYaw 값으로 정보를 업데이트한다.
			Quaternion rotr;
			rotr.Euler2(Vector3(roll, 0, 0));
			Quaternion rotp;
			rotp.Euler2(Vector3(0, 0, pitch));
			Quaternion roty;
			roty.Euler2(Vector3(0, yaw, 0));

			Quaternion rot;
			switch (m_radio3DCalcOrder2)
			{
			case 0: rot = roty * rotr * rotp; break; // Yaw x Roll x Pitch
			case 1: rot = rotr * roty * rotp; break; // Roll x Yaw x Pitch
			case 2: rot = Quaternion(roll, pitch, yaw, heave); break;
			}

			if (m_radio3DCalcOrder2 == 2) // quaternion
			{
				g_3dView2->GetCar().m_tm = rot.GetMatrix();
			}
			else
			{
				Vector3 euler = rot.Euler();
				const float chRoll = euler.x;
				const float chYaw = euler.y;
				const float chPitch = euler.z;
				g_3dView2->GetCar().SetEulerAngle(chRoll, chPitch, chYaw, heave);
			}
			//
		}

		m_incTime = 0;
	}

	// 시리얼 통신을 통해, 데이타를 전송한다.
	if (0 == m_SendType) // Serial
	{
		m_incSerialTime += deltaSeconds;

		// 시리얼 포트로 모션 시뮬레이터 장비에 모션 정보를 전송한다.
		if (cController::Get()->GetSerialComm().IsOpen())
		{
			if (m_incSerialTime > elapseT)
			{
				if (m_checkSendBinary)
				{
					BYTE buffer[512];
					const int sendLen = m_sendFormatParser.ExecuteBinary(buffer, sizeof(buffer));
					cController::Get()->GetSerialComm().SendData(buffer, sendLen);
				}
				else
				{
					const string sendStr = m_sendFormatParser.Execute();
					cController::Get()->GetSerialComm().SendData((BYTE*)sendStr.c_str(), sendStr.size());
				}

				m_incSerialTime = 0;
			}
		}
	}
	else if (1 == m_SendType) // UDP
	{
		m_incUDPTime += deltaSeconds;

		if (m_incUDPTime > elapseT)
		{
			if (m_checkSendBinary)
			{
				BYTE buffer[512];
				const int sendLen = m_sendFormatParser.ExecuteBinary(buffer, sizeof(buffer));
				m_udpSendClient.SendData(buffer, sendLen);
			}
			else
			{
				const string sendStr = m_sendFormatParser.Execute();
				m_udpSendClient.SendData((BYTE*)sendStr.c_str(), sendStr.size());
			}

			m_incUDPTime = 0;
		}
	}

}


void COutputView::OnBnClickedRadioSerial()
{
	UpdateData();

	m_UDPIP.EnableWindow(FALSE);
	m_EditUDPPort.EnableWindow(FALSE);
	m_ComPort.EnableWindow(TRUE);
	m_BaudRateCombobox.EnableWindow(TRUE);
}


void COutputView::OnBnClickedRadioUdp()
{
	UpdateData();	

	m_UDPIP.EnableWindow(TRUE);
	m_EditUDPPort.EnableWindow(TRUE);
	m_ComPort.EnableWindow(FALSE);
	m_BaudRateCombobox.EnableWindow(FALSE);
}


void COutputView::OnBnClickedRadioNone()
{
	UpdateData();

	m_UDPIP.EnableWindow(FALSE);
	m_EditUDPPort.EnableWindow(FALSE);
	m_ComPort.EnableWindow(FALSE);
	m_BaudRateCombobox.EnableWindow(FALSE);
}


void COutputView::OnBnClickedButtonConnect()
{
	UpdateData();

	if (0 == m_SendType) // Serial
	{
		const bool isConnect = cController::Get()->GetSerialComm().IsOpen();

		if (isConnect)
		{
			cController::Get()->CloseSerial();
			m_ConnectButton.SetWindowTextW(L"Start");
			m_isStart = false;
		}
		else
		{
			const int portNumber = m_ComPort.GetPortNum();
			CString baudRate;
			m_BaudRateCombobox.GetWindowTextW(baudRate);

			if (cController::Get()->ConnectSerial(portNumber, _wtoi(baudRate)))
			{
				m_ConnectButton.SetWindowTextW(L"Stop");
				m_isStart = true;
			}
		}
	}
	else if (1 == m_SendType) // UDP
	{
		if (m_isStart)
		{
			m_isStart = false;
			m_udpSendClient.Close();
			m_ConnectButton.SetWindowTextW(L"Start");
		}
		else
		{
			const string ip = GetSendIP();

			CString udpPortStr;
			m_EditUDPPort.GetWindowTextW(udpPortStr);
			const int udpPort = _wtoi((LPCTSTR)udpPortStr);
			if (!m_udpSendClient.Init(ip, udpPort))
			{
 				::AfxMessageBox(L"클라이언트 접속 에러 !!");
				return;
			}

			m_isStart = true;
			m_ConnectButton.SetWindowTextW(L"Stop");
		}
	}
	else
	{
		m_isStart = !m_isStart;

		if (m_isStart)
			m_ConnectButton.SetWindowTextW(L"Stop");
		else
			m_ConnectButton.SetWindowTextW(L"Start");
	}

	if (m_isStart)
		SetBackgroundColor(g_blueColor);
	else
		SetBackgroundColor(g_grayColor);
}


void COutputView::OnBnClickedButton3dupdate()
{
	UpdateData();

	m_isStart = true;

	m_rollParser.ParseStr(wstr2str((LPCTSTR)m_RollCommand));
	m_pitchParser.ParseStr(wstr2str((LPCTSTR)m_PitchCommand));
	m_yawParser.ParseStr(wstr2str((LPCTSTR)m_YawCommand));
	m_heaveParser.ParseStr(wstr2str((LPCTSTR)m_HeaveCommand));

	m_roll2Parser.ParseStr(wstr2str((LPCTSTR)m_Roll2Command));
	m_pitch2Parser.ParseStr(wstr2str((LPCTSTR)m_Pitch2Command));
	m_yaw2Parser.ParseStr(wstr2str((LPCTSTR)m_Yaw2Command));
	m_heave2Parser.ParseStr(wstr2str((LPCTSTR)m_Heave2Command));
}


// 송신 클라이언트가 접속할 IP 주소를 리턴한다.
string COutputView::GetSendIP()
{
	DWORD address;
	m_UDPIP.GetAddress(address);
	std::stringstream ss;
	ss << ((address & 0xff000000) >> 24) << "."
		<< ((address & 0x00ff0000) >> 16) << "."
		<< ((address & 0x0000ff00) >> 8) << "."
		<< (address & 0x000000ff);
	const string ip = ss.str();
	return ip;
}


// 전송될 스트링 포맷 업데이트
void COutputView::OnBnClickedButtonUpdateSendformat()
{
	UpdateData();
	m_sendFormatParser.ParseStr(wstr2str((LPCTSTR)m_SendFormat));
}


void COutputView::OnDestroy()
{
	SaveConfig();
	CDockablePaneChildView::OnDestroy();
}


// UI에 설정된 값을 환경변수에 저장한다.
void COutputView::SaveConfig()
{
	UpdateData();

	g_option.m_ip = GetSendIP();

	CString udpPort;
	m_EditUDPPort.GetWindowTextW(udpPort);
	g_option.m_port = _wtoi(udpPort);

	g_option.m_com = m_ComPort.GetPortNum();
	g_option.m_baudRate = m_BaudRateCombobox.GetCurSel();
	g_option.m_sendType = m_SendType;
	g_option.m_sendFormat = wstr2str((LPCTSTR)m_SendFormat);
	g_option.m_rollCmd = wstr2str((LPCTSTR)m_RollCommand);
	g_option.m_pitchCmd = wstr2str((LPCTSTR)m_PitchCommand);
	g_option.m_yawCmd = wstr2str((LPCTSTR)m_YawCommand);
	g_option.m_heaveCmd = wstr2str((LPCTSTR)m_HeaveCommand);
	g_option.m_roll2Cmd = wstr2str((LPCTSTR)m_Roll2Command);
	g_option.m_pitch2Cmd = wstr2str((LPCTSTR)m_Pitch2Command);
	g_option.m_yaw2Cmd = wstr2str((LPCTSTR)m_Yaw2Command);
	g_option.m_heave2Cmd = wstr2str((LPCTSTR)m_Heave2Command);
}


void COutputView::OnBnClickedRadioYawRollPitch1()
{
	UpdateData();
}
void COutputView::OnBnClickedRadioRollYawPitch1()
{
	UpdateData();
}
void COutputView::OnBnClickedRadioYawRollPitch2()
{
	UpdateData();
}
void COutputView::OnBnClickedRadioRollYawPitch2()
{
	UpdateData();
}
void COutputView::OnBnClickedCheckSendBinary()
{
	UpdateData();
}
void COutputView::OnBnClickedRadioQuaternion1()
{
	UpdateData();
}
void COutputView::OnBnClickedRadioQuaternion2()
{
	UpdateData();
}
