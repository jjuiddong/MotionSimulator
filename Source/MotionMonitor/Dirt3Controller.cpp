
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


// 1. 게임 환경설정 파일을 읽는다.
// 2. 모든 뷰를 초기화 하고, Start 상태로 동작하게 한다.
void cDirt3Controller::StartMotionSim(const string &configFileName, const bool isStartMotionSimOut)
{
	if (CMainFrame *pFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd()))
	{
		// UDP View, Mixing View, Output View Start
		// 안전을 위해 순서를 지키자.
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

		// UDP 정보가 오기전에 기본값이 설정되어 있어야, 머신이 기본자세를 취할 수 있다.
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
		// 안전을 위해 순서를 지키자.
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
			// 게임이 끝나거나면,  대기 상태로 바꾼다.
			m_vitconMotionSim.ReadyNoOrigin();
			PlayCountUp();
			m_state = READY;
		}
		break;

	case cDirt3Controller::PLAY:
		if (elapseUDPTime > 0.5f)
		{
			// 게임이 시작된 후, $laptime 값이 증가하다가, 
			// UDP 패킷이 더이상 오지 않는다면, 게임을 Ready 상태로 바꾼다.
			if (m_isLapTimeProgress && (script::g_symbols["@laptime"].fVal != 0.f))
			{
				m_vitconMotionSim.ReadyNoOrigin();
				PlayCountUp();
				m_state = READY;
			}
		}

		if (m_vitconMotionSim.GetPlayTime() > cMotionController::Get()->m_config.m_dirt3ViewPlayTime)
		{
			// 플레이할 수 있는 게임 시간을 넘었다면, 게임을 종료한다.
			// UDP 전송이 완전히 끝 난 후, 
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

			// ServoOff 상태로 바뀔 때, MotionOutputView도 같이 Stop 된다.
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
		// UDP 정보가 들어오고,
		// labtime이 일정시간 이상 증가될 때,  
		const float curLabTime = script::g_symbols["@laptime"].fVal;
		if (curLabTime > m_lastLapTime)
		{
			++m_timeUpCount;
			
			// 5 frame 동안 시간이 증가 되었을 때, 게임을 시작한다.
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
			// 레이스가 끝났다면, 게임을 Ready 상태로 바꾼다.
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
// temp.cnt 파일에 가장 최근에 플레이한 날짜와 시간, 플레이 카운트를 저장한다.
// play count 가 증가 될 때, temp.cnt 파일에서 날짜를 가져와 비교해서, 
// 만약 오늘 날짜보다 오래된 것이라면, game.csv 파일에 그 정보를 저장한다.
// 그리고, 현재의 playcount를 초기화 하고, temp.cnt 파일에 새 정보를 저장한다.
//
// temp.cnt 파일의 날짜가 오늘의 날짜와 같다면, playCount, errCount 에 1을 증가해
// 저장한다.
//
// temp.cnt 파일 포맷
// 2016-03-22	PlayCount
//
void cDirt3Controller::PlayCountUp()
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	bool isWriteGameData = false;
	date curDate = second_clock::local_time().date(); // 오늘 날짜.
	date oldDate;
	int oldPlayCount = 0;
	int oldErrCount = 0;

	if (GetTempCountData(oldDate, oldPlayCount, oldErrCount))
	{
		if (curDate > oldDate)
		{
			// temp.cnt에 쓰여진 날짜가, 현재보다 오래된 것이라면..
			// game.csv 파일에 카운트를 저장한다.
			isWriteGameData = true;
		}
	}

	if (isWriteGameData)
	{
		// 임시파일 내용 초기화
		ofstream tmpFile("temp.cnt");
		tmpFile << to_iso_extended_string(curDate) << " " << 1 << " " << 0 << endl;

		// 예전 정보를 저장한다.
		WritePlayCount(oldDate, oldPlayCount, oldErrCount);
	}
	else
	{
		// playCount를 1증가해서 저장한다. over wright
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


// temp.cnt 파일에서 날짜와 playcount, errcount 정보를 읽어온다.
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
	{ "Mon", "月" },
	{ "Tue", "火" },
	{ "Wed", "水" },
	{ "Thu", "木" },
	{ "Fri", "金" },
	{ "Sat", "土" },
	{ "Sun", "日" },
};
const int g_weeksSize = sizeof(g_weeks) / sizeof(sWeeks);
map<string, string> g_weeksMap;

// PlayCount 저장
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


// temp.cnt 파일 내용이 있다면, 정보를 가져와 game.csv 파일에 추가하여 저장한다.
// 그 후, temp.cnt 파일은 초기화 된다.
void cDirt3Controller::WriteGameData()
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	// temp.cnt 파일에 저장된 정보를 읽어온다.
	date tempCntDate;
	int playCount = 0, errCount = 0;
	if (!GetTempCountData(tempCntDate, playCount, errCount))
		return; // 저장할 정보가 없으므로, 함수를 종료한다.


	ifstream excelFile;
	excelFile.open("game.csv");

	// 라인 단위로 모두 읽는다.
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

		// game.csv 의 마지막 정보가 temp.cnt 파일의 날짜와 같다면,
		// game.csv의 마지막 정보에서 PlayCount, ErrCount를 증가시켜, 추가한다.
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

	// game.csv에 새정보를 추가한다.
	WritePlayCount(tempCntDate, playCount, errCount);

	ClearTempCountData();
}
