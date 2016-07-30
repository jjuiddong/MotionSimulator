// MixingView.cpp : implementation file
//

#include "stdafx.h"
#include "MotionMonitor.h"
#include "MixingView.h"
#include "afxdialogex.h"
#include "Dirt3Controller.h"


const static CString g_mixingviewPlotCommand = L"\r\n\
plot1 = 0, 0, 0, 0, 0\r\n\
string1 = %f;\r\n\
name1 = Yaw\r\n\
linewidth1 = 2\r\n\
plot2 = 0, 0, 0, 0, 0\r\n\
string2 = %*f; %f;\r\n\
name2 = Pitch\r\n\
linewidth2 = 2\r\n\
plot3 = 0, 0, 0, 0, 0\r\n\
string3 = %*f; %*f; %f;\r\n\
name3 = Roll\r\n\
linewidth3 = 2\r\n\
plot4 = 0, 0, 0, 0, 0\r\n\
string4 = %*f; %*f; %f; \r\n\
name4 = Heave\r\n\
linewidth4 = 2\r\n\
";


// CMixingView dialog
CMixingView::CMixingView(CWnd* pParent /*=NULL*/)
	: CDockablePaneChildView(CMixingView::IDD, pParent)
	, m_multiPlotWindows(NULL)
	, m_incTime(0)
	, m_totalIncTime(0)
	, m_state(STOP)
{
}

CMixingView::~CMixingView()
{
}

void CMixingView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePaneChildView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT2_COMMAND, m_EditCommand);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_StartButton);
}


BEGIN_MESSAGE_MAP(CMixingView, CDockablePaneChildView)
	ON_BN_CLICKED(IDCANCEL, &CMixingView::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMixingView::OnBnClickedOk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CMixingView::OnBnClickedButtonUpdate)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BEGIN_ANCHOR_MAP(CMixingView)
	ANCHOR_MAP_ENTRY(IDC_RICHEDIT2_COMMAND, ANF_LEFT | ANF_TOP | ANF_RIGHT)
	ANCHOR_MAP_ENTRY(IDC_STATIC_GRAPH, ANF_LEFT | ANF_TOP | ANF_RIGHT | ANF_BOTTOM)
END_ANCHOR_MAP()


// CMixingView message handlers
BOOL CMixingView::OnInitDialog()
{
	CDockablePaneChildView::OnInitDialog();

	InitAnchors();

	UpdateConfig();

	// Plot창 생성.
	CRect rect;
	GetClientRect(rect);

	m_multiPlotWindows = new CMultiPlotWindow();
	BOOL result = m_multiPlotWindows->Create(NULL, NULL, WS_VISIBLE | WS_CHILD,
		CRect(0, 0, rect.Width(), 400), this, AFX_IDW_PANE_FIRST);

	m_multiPlotWindows->SetScrollSizes(MM_TEXT, CSize(rect.Width() - 30, 100));
	m_multiPlotWindows->ShowWindow(SW_SHOW);
	m_multiPlotWindows->SetFixedWidthMode(true);

	return TRUE;
}


void CMixingView::UpdateConfig(bool IsSaveAndValidate) //IsSaveAndValidate=true
{
	if (IsSaveAndValidate)
	{
		const CString command =
L"#input = udp\n\
#input = motionwave\n\
input = joystick+udp+motionwave\n\
\n\
mixing_input1_enable = 1\n\
mixing_rate1_all = 1\n\
mixing_rate1_yaw = 0\n\
mixing_rate1_pitch = 0\n\
mixing_rate1_roll = 0\n\
mixing_rate1_heave = 0\n\
\n\
mixing_input2_enable = 0\n\
mixing_rate2_all = 0\n\
mixing_rate2_yaw = 0\n\
mixing_rate2_pitch = 0\n\
mixing_rate2_roll = 0\n\
mixing_rate2_heave = 0\n\
\n\
mixing_input3_enable = 0\n\
mixing_rate3_all = 0\n\
mixing_rate3_yaw = 0\n\
mixing_rate3_pitch = 0\n\
mixing_rate3_roll = 0\n\
mixing_rate3_heave = 0\n\
\n\
mixing_bias_yaw = 0\n\
mixing_bias_pitch = 0\n\
mixing_bias_roll = 0\n\
mixing_bias_heave = 0\n\
\n\
";

		CString cmdStr;
		if (!cMotionController::Get()->m_config.m_mixingModCommand.empty())
		{
			cmdStr = str2wstr(cMotionController::Get()->m_config.m_mixingModCommand).c_str();
		}
		else
		{
			cmdStr = command;
		}
		m_EditCommand.SetWindowTextW(cmdStr);

		m_config.ParseStr(wstr2str((LPCTSTR)cmdStr));

		UpdateData(FALSE);
	}
	else
	{
		UpdateData();

		// 환경파일 저장
		CString command;
		m_EditCommand.GetWindowTextW(command);
		cMotionController::Get()->m_config.m_mixingModCommand = wstr2str((LPCTSTR)command);
	}
}


void CMixingView::OnBnClickedCancel()
{
}
void CMixingView::OnBnClickedOk()
{
}


void CMixingView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePaneChildView::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);

	if (m_multiPlotWindows && m_multiPlotWindows->GetSafeHwnd())
	{
		CRect pwr;
		GetDlgItem(IDC_STATIC_GRAPH)->GetWindowRect(pwr);
		ScreenToClient(pwr);
		m_multiPlotWindows->MoveWindow(pwr);
	}
}


void CMixingView::OnBnClickedButtonUpdate()
{
	if (START == m_state)
	{
		Stop();
	}
	else
	{
		Start();
	}
}

void CMixingView::Update(const float deltaSeconds)
{
	RET(STOP == m_state);

	const float elapseT = 0.033f;
	m_incTime += deltaSeconds;
	m_totalIncTime += deltaSeconds;

	if (m_incTime > elapseT)
	{
		// prev data
		const float yaw0 = cMotionController::Get()->m_yaw;
		const float pitch0 = cMotionController::Get()->m_pitch;
		const float roll0 = cMotionController::Get()->m_roll;
		const float heave0 = cMotionController::Get()->m_heave;
		const float speed0 = cMotionController::Get()->m_speed;

		float yaw, pitch, roll, heave, speed, norm1, norm2, norm3;
		float yaw1, pitch1, roll1;
		Mixing(deltaSeconds, 
			yaw0, pitch0, roll0, heave0, speed0,
			yaw, pitch, roll, heave, speed, 
			yaw1, pitch1, roll1,
			norm1, norm2, norm3);

		cMotionController::Get()->m_yaw = yaw1;
		cMotionController::Get()->m_pitch = pitch1;
		cMotionController::Get()->m_roll = roll1;
		cMotionController::Get()->m_heave = heave;
		cMotionController::Get()->m_speed = speed;

		// save norm
		script::sFieldData data;
		data.type = script::FIELD_TYPE::T_FLOAT;
		data.fVal = norm1;
		script::g_symbols["@norm1"] = data;
		data.fVal = norm2;
		script::g_symbols["@norm2"] = data;
		data.fVal = norm3;
		script::g_symbols["@norm3"] = data;
		//
		
		if (!g_isReleaseMode)
		{
			m_multiPlotWindows->SetXY(0, m_totalIncTime, yaw, 0);
			m_multiPlotWindows->SetXY(1, m_totalIncTime, pitch, 0);
			m_multiPlotWindows->SetXY(2, m_totalIncTime, roll, 0);
			m_multiPlotWindows->SetXY(3, m_totalIncTime, heave, 0);

			m_multiPlotWindows->SetXY(0, m_totalIncTime, yaw1, 1);
			m_multiPlotWindows->SetXY(1, m_totalIncTime, pitch1, 1);
			m_multiPlotWindows->SetXY(2, m_totalIncTime, roll1, 1);

			m_multiPlotWindows->DrawGraph(m_incTime, false);
		}

		m_incTime -= elapseT;
		if (m_incTime > elapseT)
			m_incTime = 0;
	}
}

float calcRate(const float src, const float center, const float rate)
{
	return (src - center) * rate + center;
}


// 입력값을 믹싱해서 리턴한다.
void CMixingView::Mixing(const float deltaSeconds, 
	const float yaw0, const float pitch0, const float roll0, const float heave0, const float speed0,
	OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave, OUT float &speed, 
	OUT float &yaw1, OUT float &pitch1, OUT float &roll1,
	OUT float &norm1, OUT float &norm2, OUT float &norm3)
{
	yaw = yaw1 = 0;
	pitch = pitch1 = 0;
	roll = roll1 = 0;
	heave = 0;
	speed = 0;
	norm1 = 0;
	norm2 = 0;
	norm3 = 0;

	if (m_config.m_inputType | INPUT_JOYSTICK)
	{
		if (m_config.m_input1_enable)
		{
			float joyYaw, joyPitch, joyRoll, joyHeave;
			cMotionController::Get()->m_joystickMod.GetFinal(joyYaw, joyPitch, joyRoll, joyHeave);

			if (m_config.m_rate1_all == 0)
			{
				yaw += calcRate(joyYaw, m_config.m_rate1_center_yaw, m_config.m_rate1_yaw * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(joyPitch, m_config.m_rate1_center_pitch, m_config.m_rate1_pitch * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(joyRoll, m_config.m_rate1_center_roll, m_config.m_rate1_roll * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(joyHeave, m_config.m_rate1_center_heave, m_config.m_rate1_heave * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
			else
			{
				yaw += calcRate(joyYaw, m_config.m_rate1_center_yaw, m_config.m_rate1_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(joyPitch, m_config.m_rate1_center_pitch, m_config.m_rate1_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(joyRoll, m_config.m_rate1_center_roll, m_config.m_rate1_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(joyHeave, m_config.m_rate1_center_heave, m_config.m_rate1_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
		}
	}

	if (m_config.m_inputType | INPUT_UDP)
	{
		if (m_config.m_input2_enable)
		{
			float udpYaw, udpPitch, udpRoll, udpHeave;
			cMotionController::Get()->m_udpMod.GetFinal(udpYaw, udpPitch, udpRoll, udpHeave);

			if (m_config.m_rate2_all == 0)
			{
				yaw += calcRate(udpYaw, m_config.m_rate2_center_yaw, m_config.m_rate2_yaw * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(udpPitch, m_config.m_rate2_center_pitch, m_config.m_rate2_pitch * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(udpRoll, m_config.m_rate2_center_roll, m_config.m_rate2_roll * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(udpHeave, m_config.m_rate2_center_heave, m_config.m_rate2_heave * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
			else
			{
				yaw += calcRate(udpYaw, m_config.m_rate2_center_yaw, m_config.m_rate2_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(udpPitch, m_config.m_rate2_center_pitch, m_config.m_rate2_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(udpRoll, m_config.m_rate2_center_roll, m_config.m_rate2_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(udpHeave, m_config.m_rate2_center_heave, m_config.m_rate2_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
		}
	}

	if (m_config.m_inputType | INPUT_MOTIONWAVE)
	{
		if (m_config.m_input3_enable)
		{
			float mwavYaw, mwavPitch, mwavRoll, mwavHeave;
			cMotionController::Get()->m_mwavMod.GetFinal(mwavYaw, mwavPitch, mwavRoll, mwavHeave);

			if (m_config.m_rate3_all == 0)
			{
				yaw += calcRate(mwavYaw, m_config.m_rate3_center_yaw, m_config.m_rate3_yaw * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(mwavPitch, m_config.m_rate3_center_pitch, m_config.m_rate3_pitch * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(mwavRoll, m_config.m_rate3_center_roll, m_config.m_rate3_roll * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(mwavHeave, m_config.m_rate3_center_heave, m_config.m_rate3_heave * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
			else
			{
				yaw += calcRate(mwavYaw, m_config.m_rate3_center_yaw, m_config.m_rate3_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorYawPower);
				pitch += calcRate(mwavPitch, m_config.m_rate3_center_pitch, m_config.m_rate3_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(mwavRoll, m_config.m_rate3_center_roll, m_config.m_rate3_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				heave += calcRate(mwavHeave, m_config.m_rate3_center_heave, m_config.m_rate3_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
			}
		}
	}


	if (m_config.m_inputType | INPUT_VAR)
	{
		if (m_config.m_input4_enable)
		{
			float varPitch;
			cMotionController::Get()->m_varModulator1.GetFinal(varPitch);
			float varRoll;
			cMotionController::Get()->m_varModulator2.GetFinal(varRoll);
			float varSpeed;
			cMotionController::Get()->m_varModulator3.GetFinal(varSpeed);

			if (m_config.m_rate4_all == 0)
			{
				pitch += calcRate(varPitch, m_config.m_rate4_center_pitch, m_config.m_rate4_pitch * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(varRoll, m_config.m_rate4_center_roll, m_config.m_rate4_roll * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				speed += varSpeed;
			}
			else
			{
				pitch += calcRate(varPitch, m_config.m_rate4_center_pitch, m_config.m_rate4_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				roll += calcRate(varRoll, m_config.m_rate4_center_roll, m_config.m_rate4_all * cMotionController::Get()->m_config.m_dirt3ViewActuatorPower);
				speed += varSpeed;
			}
		}
	}

	yaw += m_config.m_bias_yaw;
	pitch += m_config.m_bias_pitch;
	roll += m_config.m_bias_roll;
	heave += m_config.m_bias_heave;

	if (AXIS_TYPE::AXIS3 == cMotionController::Get()->m_config.m_dirt3ViewAxisType)
	{
		yaw = 10000; // 3축으로만 제어할 때는 Yaw축을 제거한다. (중심 값으로 설정한다.)
	}

	if (pitch > 20000)
		pitch = 20000;
	if (pitch < 0)
		pitch = 0;

	if (roll > 20000)
		roll = 20000;
	if (roll < 0)
		roll = 0;

	if (yaw > 20000)
		yaw = 20000;
	if (yaw < 0)
		yaw = 0;

	if (heave > 14000)
		heave = 14000;
	if (heave < 4000)
		heave = 4000;

	if (speed > 100)
		speed = 100;
	if (speed < 0)
		speed = 0;


	// 최대 변화값 보다 크다면, 평활화 시킨다.
	const Vector3 v0(yaw0, pitch0, roll0);
	const Vector3 v1(yaw, pitch, roll);
	const Vector3 v = v1 - v0;
	norm1 = v.Length();

	float varNorm;
	cMotionController::Get()->m_varModulator4.Update(deltaSeconds, norm1);
	cMotionController::Get()->m_varModulator4.GetFinal(varNorm);

	Vector3 diff = v.Normal() * varNorm;
	yaw1 = yaw0 + diff.x;
	pitch1 = pitch0 + diff.y;
	roll1 = roll0 + diff.z;
	norm2 = varNorm;
	norm3 = diff.Length();
	//
	
	// 게임이 실행 중일 때만, Fan을 돌린다.
	if (cDirt3Controller::Get()->GetState() != cDirt3Controller::PLAY)
	{
		speed = 0;
	}


//
// 	// 최대 변화값 보다 크다면, 평활화 시킨다.
// 	if (norm1 > m_config.m_maxVariance)
// 	{
// 		Vector3 diff = v.Normal() * m_config.m_maxVariance;
// 		yaw1 = yaw0 + diff.x;
// 		pitch1 = pitch0 + diff.y;
// 		roll1 = roll0 + diff.z;
// 		norm2 = m_config.m_maxVariance;
// 		norm3 = diff.Length();
// 	}
// 	else
// 	{
// 		yaw1 = yaw;
// 		pitch1 = pitch;
// 		roll1 = roll;
// 		norm2 = norm1;
// 		norm3 = norm1;
// 	}
}


void CMixingView::OnDestroy()
{
	UpdateConfig(false);
	CDockablePaneChildView::OnDestroy();
}


// 믹싱 계산을 시작한다.
void CMixingView::Start()
{
	UpdateData();

	m_state = START;

	CString command;
	m_EditCommand.GetWindowTextW(command);
	m_config.ParseStr(common::wstr2str((LPCTSTR)command).c_str());

	m_multiPlotWindows->ProcessPlotCommand(g_mixingviewPlotCommand, 2);
	m_multiPlotWindows->SetFixedWidthMode(true);
	m_incTime = 0;

	m_StartButton.SetWindowTextW(L"Stop");

	SetBackgroundColor(g_blueColor);
}


// 믹싱 계산을 멈춘다.
void CMixingView::Stop()
{
	m_state = STOP;
	m_StartButton.SetWindowTextW(L"Start");
	SetBackgroundColor(g_grayColor);
}
