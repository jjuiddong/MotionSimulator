//
//
// Dirt3 ��� ��Ʈ�ѷ�
//
//
#pragma once



class cDirt3Controller : public common::cSingleton<cDirt3Controller>
{
public:
	cDirt3Controller();
	virtual ~cDirt3Controller();

	enum STATE {
		OFF,				// �����ġ Off
		OFF_PROCESS,  // �����ġ On, ��� ��ġ ���� �غ� ��
		READY,		// �����ġ On, UDP ������ �޾� ��� ��ġ�� ������ �غ� �� ����
		PLAY,			// �����ġ On, UDP ������ �޾� ��� ��ġ�� �����̴� ����
		TIMEUPSTOP,	// �����ġ On, Ư�� ���ǿ� ���� �����ġ�� Stop ���� ���� (�ð� �ʰ�, ���� Stop)
	};

	void StartMotionSim(const string &configFileName, const bool isStartMotionSimOut=false);
	void StopMotionSim();
	void Update(const float deltaSeconds);
	void UpdateUDP(const char *buffer, const int bufferLen);
	cVitconMotionSim2& GetMotionSim();
	STATE GetState() const;
	string GetStateStr() const;
	void WriteGameData();


protected:
	void PlayCountUp();
	void ErrorCountUp();
	void WritePlayCount(const boost::gregorian::date &playDate, const int playCount, const int errorCount);
	bool GetTempCountData(OUT boost::gregorian::date &playDate, OUT int &playCount, OUT int &errorCount);
	void ClearTempCountData();

	
protected:
	STATE m_state;
	cVitconMotionSim2 m_vitconMotionSim;
	cVitconMotionSim2::STATE m_oldState;
	cvproc::imagematch::cMatchScript2 m_matchScript;

	

	float m_lastUDPUpdateTime; // ���� �ֱٿ� UDP ��Ŷ�� ���� �ð��� �����Ѵ�.
	bool m_isLapTimeProgress; // @laptime �� �����Ǳ� �����ϸ�, true �� �ȴ�.
	float m_lastLapTime; // �� �������� lap time
	int m_timeUpCount; // ���� Ƚ�� �̻� �ð��� �����Ǵ� ���� üũ�Ѵ�.
};


inline cVitconMotionSim2& cDirt3Controller::GetMotionSim() { return m_vitconMotionSim; }
inline cDirt3Controller::STATE cDirt3Controller::GetState() const { return m_state;  }
