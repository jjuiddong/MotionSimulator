#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CShmView dialog
class cMemDumpWindow;

class CShmView : public CDockablePaneChildView
{
public:
	CShmView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShmView();

// Dialog Data
	enum { IDD = IDD_DIALOG_SHM };
	enum STATE { STOP, PLAY, PAUSE, RECORD, };

	virtual void Update(const float deltaSeconds) override;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CheckFPS(const float deltaSeconds);
	STATE ChangeState(const STATE nextState);
	bool Play();
	bool RecordStart();
	void RecordEnd();


public:
	bool m_isStart;
	float m_incTime;
	float m_checkUpdateTime;
	int m_updateFPS; // 1초당 업데이트된 횟수
	int m_incRecordLength;
	cShmMap<script::sFieldData> m_shmSymbols;
	STATE m_state;
	cShmmem m_shmMem;
	cShmmem m_playShmMem;
	cShmmem m_readShmMem;
	cUDPStream m_udpStream;
	cUDPStreamPlayer m_udpPlayer;
	char m_buffer[512];
	cMemDumpWindow *m_dumpWindow;


	DECLARE_ANCHOR_MAP();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonOpen();
	CString m_ShmName;
	CButton m_OpenButton;
	CStatic m_staticUpdateFPS;
	CStatic m_recordLength;
	afx_msg void OnBnClickedButtonRecord();
	CStatic m_playFileName;
	afx_msg void OnBnClickedButtonPlay();
	CStatic m_playTime;
	afx_msg void OnBnClickedCheckRepeat();
	BOOL m_checkRepeat;
	CFileTreeCtrl m_FileTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRefresh();
	CButton m_buttonRecord;
	CButton m_buttonPlay;
	afx_msg void OnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_playSharedMemName;
	CSliderCtrl m_sliderPlay;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL m_IsShmSymbolTable;
	afx_msg void OnBnClickedCheckSymtable();
	int m_readMemorySize;
};
