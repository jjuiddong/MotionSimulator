
#include "stdafx.h"
#include "Dirt3Controller.h"
#include "MainFrm.h"
#include "resource.h"
#include "MotionWaveView.h"
#include "MixingView.h"
#include "MotionOutputView.h"
#include "UDPInputView.h"
#include "UDPParseView.h"
#include "JoystickView.h"
#include "VarModulationView.h"



cDirt3Controller::cDirt3Controller() :
	m_oldState(cVitconMotionSim2::OFF)
	, m_lastUDPUpdateTime(0)
	, m_state(OFF)
	, m_isLapTimeProgress(false)
	, m_lastLapTime(0)
	, m_timeUpCount(0)
{
}

cDirt3Controller::~cDirt3Controller()
{
}


// 1. ���� ȯ�漳�� ������ �д´�.
// 2. ��� �並 �ʱ�ȭ �ϰ�, Start ���·� �����ϰ� �Ѵ�.
void cDirt3Controller::StartMotionSim(const string &configFileName, const bool isStartMotionSimOut)
{
	if (CMainFrame *pFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd()))
	{
		// UDP View, Mixing View, Output View Start
		// ������ ���� ������ ��Ű��.
		if (pFrm->m_mixingView)
			pFrm->m_mixingView->Start();
		if (pFrm->m_varModulationView)
			pFrm->m_varModulationView->Start();
		if (pFrm->m_udpInputView)
			pFrm->m_udpInputView->Start();
		if (pFrm->m_udpParseView)
			pFrm->m_udpParseView->Start();

		if (isStartMotionSimOut)
			if (pFrm->m_motionOutputView)
				pFrm->m_motionOutputView->Start();

		m_state = READY;
		m_vitconMotionSim.Init(&cController::Get()->GetSerialComm());
		m_vitconMotionSim.On();

		// UDP ������ �������� �⺻���� �����Ǿ� �־��, �ӽ��� �⺻�ڼ��� ���� �� �ִ�.
		script::sFieldData data;
		data.fVal = 1.55f;
		data.type = script::FIELD_TYPE::T_FLOAT;
		script::g_symbols["$7"] = data;
		// 		data.fVal = 0.f;
		// 		data.type = script::FIELD_TYPE::T_FLOAT;
		// 		script::g_symbols["$13"] = data;

		if (m_matchScript.Read("../media/dirt3/dirt3_trackname.txt"))
		{
			dbg::Log("Success Read imagematchscript \n");
		}
		else
		{
			dbg::Log("Error!! Read imagematchscript \n");
			::AfxMessageBox(L"Error!! Read Match Script");
		}

	}
}


void cDirt3Controller::StopMotionSim()
{
	if (CMainFrame *pFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd()))
	{
		// ������ ���� ������ ��Ű��.
// 		if (pFrm->m_motionOutputView)
// 			pFrm->m_motionOutputView->Stop();
		if (pFrm->m_udpInputView)
			pFrm->m_udpInputView->Stop();
		if (pFrm->m_udpParseView)
			pFrm->m_udpParseView->Stop();
		if (pFrm->m_varModulationView)
			pFrm->m_varModulationView->Stop();
		if (pFrm->m_mixingView)
			pFrm->m_mixingView->Stop();

		m_state = OFF_PROCESS;
		m_vitconMotionSim.Off();
	}
}


void cDirt3Controller::Update(const float deltaSeconds)
{
	const float elapseUDPTime = cController::Get()->GetGlobalSeconds() - m_lastUDPUpdateTime;

	switch (m_state)
	{
	case cDirt3Controller::OFF:
	case cDirt3Controller::OFF_PROCESS:
	case cDirt3Controller::READY:
		break;

	case cDirt3Controller::TIMEUPSTOP:
		//if (elapseUDPTime > 5.f)
		if (script::g_symbols["@laptime"].fVal == 0.f)
		{
			// ������ �����ų���,  ��� ���·� �ٲ۴�.
			m_vitconMotionSim.ReadyNoOrigin();
			PlayCountUp();
			m_state = READY;
		}
		break;

	case cDirt3Controller::PLAY:
		if (elapseUDPTime > 0.5f)
		{
			// ������ ���۵� ��, $laptime ���� �����ϴٰ�, 
			// UDP ��Ŷ�� ���̻� ���� �ʴ´ٸ�, ������ Ready ���·� �ٲ۴�.
			if (m_isLapTimeProgress && (script::g_symbols["@laptime"].fVal != 0.f))
			{
				m_vitconMotionSim.ReadyNoOrigin();
				PlayCountUp();
				m_state = READY;
			}
		}

		if (m_vitconMotionSim.GetPlayTime() > cMotionController::Get()->m_config.m_dirt3ViewPlayTime)
		{
			// �÷����� �� �ִ� ���� �ð��� �Ѿ��ٸ�, ������ �����Ѵ�.
			// UDP ������ ������ �� �� ��, 
			m_vitconMotionSim.ReadyNoOrigin();
			PlayCountUp();
			m_state = TIMEUPSTOP;
		}
		break;
	}

	m_vitconMotionSim.Update(deltaSeconds);

	const cVitconMotionSim2::STATE motionSimState = m_vitconMotionSim.GetState();
	if (motionSimState != m_oldState)
	{
		switch (motionSimState)
		{
		case cVitconMotionSim2::OFF:
			m_state = OFF;

			// ServoOff ���·� �ٲ� ��, MotionOutputView�� ���� Stop �ȴ�.
			if (CMainFrame *pFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd()))
			{
				pFrm->m_motionOutputView->Stop();
				if (pFrm->m_motionWaveView)
					pFrm->m_motionWaveView->Stop();
			}
			break;

		case cVitconMotionSim2::READY:
			//m_vitconMotionSim.Play();
			break;

		default:
			break;
		}

		m_oldState = m_vitconMotionSim.GetState();
	}
}


void cDirt3Controller::UpdateUDP(const char *buffer, const int bufferLen)
{
	m_lastUDPUpdateTime = cController::Get()->GetGlobalSeconds();

	switch (m_state)
	{
	case cDirt3Controller::OFF:
	case cDirt3Controller::TIMEUPSTOP:
		break;

	case cDirt3Controller::READY:
	{
		// UDP ������ ������,
		// labtime�� �����ð� �̻� ������ ��,  
		const float curLabTime = script::g_symbols["@laptime"].fVal;
		if (curLabTime > m_lastLapTime)
		{
			++m_timeUpCount;
			
			// 5 frame ���� �ð��� ���� �Ǿ��� ��, ������ �����Ѵ�.
			if (m_timeUpCount > 5)
			{
				if (m_vitconMotionSim.Play())
				{
					m_state = PLAY;
					m_lastLapTime = 0;
					m_timeUpCount = 0;
					m_isLapTimeProgress = false;
				}
			}

		}
	}
	break;

	case cDirt3Controller::PLAY:
	{
		const float curLapTime = script::g_symbols["@laptime"].fVal;
		const float distance = script::g_symbols["@distance"].fVal;
		if ((m_lastLapTime == curLapTime) && (m_lastLapTime == 0.f) && (distance != 0.f))
		{
			// ���̽��� �����ٸ�, ������ Ready ���·� �ٲ۴�.
			m_vitconMotionSim.ReadyNoOrigin();
			PlayCountUp();
			m_state = READY;
		}

		if (!m_isLapTimeProgress)
		{
			if (curLapTime > m_lastLapTime)
			{
				m_isLapTimeProgress = true;
			}
			else
			{
				m_lastLapTime = curLapTime;
			}
		}

	}
	break;
	}

	m_lastLapTime = script::g_symbols["@laptime"].fVal;
}


string cDirt3Controller::GetStateStr() const
{
	switch (m_state)
	{
	case cDirt3Controller::OFF: return "Off";
	case cDirt3Controller::OFF_PROCESS: return "Off Process";
	case cDirt3Controller::READY: return "Ready";
	case cDirt3Controller::PLAY: return "Play";
	case cDirt3Controller::TIMEUPSTOP: return "TimeUpStop";
	default: return "None";
	}	
}


//
// temp.cnt ���Ͽ� ���� �ֱٿ� �÷����� ��¥�� �ð�, �÷��� ī��Ʈ�� �����Ѵ�.
// play count �� ���� �� ��, temp.cnt ���Ͽ��� ��¥�� ������ ���ؼ�, 
// ���� ���� ��¥���� ������ ���̶��, game.csv ���Ͽ� �� ������ �����Ѵ�.
// �׸���, ������ playcount�� �ʱ�ȭ �ϰ�, temp.cnt ���Ͽ� �� ������ �����Ѵ�.
//
// temp.cnt ������ ��¥�� ������ ��¥�� ���ٸ�, playCount, errCount �� 1�� ������
// �����Ѵ�.
//
// temp.cnt ���� ����
// 2016-03-22	PlayCount
//
void cDirt3Controller::PlayCountUp()
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	bool isWriteGameData = false;
	date curDate = second_clock::local_time().date(); // ���� ��¥.
	date oldDate;
	int oldPlayCount = 0;
	int oldErrCount = 0;

	if (GetTempCountData(oldDate, oldPlayCount, oldErrCount))
	{
		if (curDate > oldDate)
		{
			// temp.cnt�� ������ ��¥��, ���纸�� ������ ���̶��..
			// game.csv ���Ͽ� ī��Ʈ�� �����Ѵ�.
			isWriteGameData = true;
		}
	}

	if (isWriteGameData)
	{
		// �ӽ����� ���� �ʱ�ȭ
		ofstream tmpFile("temp.cnt");
		tmpFile << to_iso_extended_string(curDate) << " " << 1 << " " << 0 << endl;

		// ���� ������ �����Ѵ�.
		WritePlayCount(oldDate, oldPlayCount, oldErrCount);
	}
	else
	{
		// playCount�� 1�����ؼ� �����Ѵ�. over wright
		ofstream tmpFile("temp.cnt");
		tmpFile << to_iso_extended_string(curDate) << " " << oldPlayCount + 1 << " " << oldErrCount << endl;
	}

//		WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
// 	WritePlayCount(oldDate, 1, 2);
}


// temp.cnt ���Ͽ��� ��¥�� playcount, errcount ������ �о�´�.
bool cDirt3Controller::GetTempCountData(OUT boost::gregorian::date &playDate, OUT int &playCount, OUT int &errorCount)
{
	using namespace std;
	using namespace boost::gregorian;

	ifstream tempFile("temp.cnt");
	if (!tempFile.is_open())
		return false;

	string dateStr;
	tempFile >> dateStr;
	if (dateStr.empty())
		return false;

	tempFile >> playCount >> errorCount;

	try
	{
		playDate = date(from_simple_string(dateStr));
	}
	catch (std::exception&)
	{
		return false;
	}

	return true;
}


void cDirt3Controller::ClearTempCountData()
{
	using namespace std;
	ofstream tempFile("temp.cnt");
	tempFile << "";
}


struct sWeeks
{
	string w1; // english
	string w2; // chineses
};
sWeeks g_weeks[] = {
	{ "Mon", "��" },
	{ "Tue", "��" },
	{ "Wed", "�" },
	{ "Thu", "��" },
	{ "Fri", "��" },
	{ "Sat", "��" },
	{ "Sun", "��" },
};
const int g_weeksSize = sizeof(g_weeks) / sizeof(sWeeks);
map<string, string> g_weeksMap;

// PlayCount ����
// dateStr : 2016-03-22
void cDirt3Controller::WritePlayCount(const boost::gregorian::date &playDate, const int playCount, const int errorCount)
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	if (g_weeksMap.empty())
	{
		for (int i = 0; i < g_weeksSize; ++i)
			g_weeksMap.insert({ g_weeks[i].w1, g_weeks[i].w2 });
	}

	ofstream excelFile;
	excelFile.open("game.csv", ios_base::app);

	excelFile << to_iso_extended_string(playDate);
	excelFile << "\t";
	excelFile << playDate.day_of_week();
	excelFile << "\t";
	excelFile << g_weeksMap[playDate.day_of_week().as_short_string()];
	excelFile << "\t";
	excelFile << playCount;
	excelFile << "\t";
	excelFile << errorCount;
	excelFile << "\t";
	excelFile << endl;

	excelFile.close();
}


// temp.cnt ���� ������ �ִٸ�, ������ ������ game.csv ���Ͽ� �߰��Ͽ� �����Ѵ�.
// �� ��, temp.cnt ������ �ʱ�ȭ �ȴ�.
void cDirt3Controller::WriteGameData()
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	// temp.cnt ���Ͽ� ����� ������ �о�´�.
	date tempCntDate;
	int playCount = 0, errCount = 0;
	if (!GetTempCountData(tempCntDate, playCount, errCount))
		return; // ������ ������ �����Ƿ�, �Լ��� �����Ѵ�.


	ifstream excelFile;
	excelFile.open("game.csv");

	// ���� ������ ��� �д´�.
	list<string> strList;
	char line[256];
	while (excelFile.getline(line, sizeof(line)))
	{
		strList.push_back(line);
	}
	excelFile.close();


	if (!strList.empty())
	{
		stringstream ss(strList.back());

		string strDate, week1, week2;
		int dataPlayCount = 0, dataErrCount = 0;
		ss >> strDate >> week1 >> week2 >> dataPlayCount >> dataErrCount;

		const date lastDate = date(from_simple_string(strDate));

		// game.csv �� ������ ������ temp.cnt ������ ��¥�� ���ٸ�,
		// game.csv�� ������ �������� PlayCount, ErrCount�� ��������, �߰��Ѵ�.
		if (lastDate == tempCntDate)
		{
			playCount += dataPlayCount;
			errCount += dataErrCount;
			strList.pop_back();

			// remove last data
			ofstream wrFile;
			wrFile.open("game.csv");
			for each (auto &str in strList)
				wrFile << str << endl;
			wrFile.close();
		}
	}

	// game.csv�� �������� �߰��Ѵ�.
	WritePlayCount(tempCntDate, playCount, errCount);

	ClearTempCountData();
}
