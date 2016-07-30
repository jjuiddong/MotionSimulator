// ShmView.cpp : implementation file
//

#include "stdafx.h"
#include "UDPAnalyzer.h"
#include "ShmView.h"
#include "afxdialogex.h"


// CShmView dialog
const static string g_tempShmStreamFileName = "../media/shmstream_temp.shm";


CShmView::CShmView(CWnd* pParent /*=NULL*/)
	: CDockablePaneChildView(IDD_DIALOG_SHM, pParent)
	, m_ShmName(_T(""))
	, m_isStart(false)
	, m_incTime(0)
	, m_checkUpdateTime(0)
	, m_updateFPS(0)
	, m_checkRepeat(FALSE)
	, m_state(STOP)
	, m_playSharedMemName(_T(""))
	, m_incRecordLength(0)
	, m_dumpWindow(0)
	, m_IsShmSymbolTable(FALSE)
	, m_readMemorySize(128)
{
}

CShmView::~CShmView()
{
}

void CShmView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePaneChildView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SHMNAME, m_ShmName);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_OpenButton);
	DDX_Control(pDX, IDC_STATIC_FPS, m_staticUpdateFPS);
	DDX_Control(pDX, IDC_STATIC_RECORD_TIME, m_recordLength);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_playFileName);
	DDX_Control(pDX, IDC_STATIC_PLAYTIME, m_playTime);
	DDX_Check(pDX, IDC_CHECK_REPEAT, m_checkRepeat);
	DDX_Control(pDX, IDC_TREE_FILE, m_FileTree);
	DDX_Control(pDX, IDC_BUTTON_RECORD, m_buttonRecord);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_buttonPlay);
	DDX_Text(pDX, IDC_EDIT1, m_playSharedMemName);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderPlay);
	DDX_Check(pDX, IDC_CHECK_SYMTABLE, m_IsShmSymbolTable);
	DDX_Text(pDX, IDC_EDIT_READ_MEMSIZE, m_readMemorySize);
}


BEGIN_ANCHOR_MAP(CShmView)
	ANCHOR_MAP_ENTRY(IDC_TREE_FILE, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_FPS, ANF_RIGHT | ANF_TOP)
 	ANCHOR_MAP_ENTRY(IDC_BUTTON_REFRESH, ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_GROUP1, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_STATIC_GROUP2, ANF_LEFT | ANF_RIGHT | ANF_TOP)
	ANCHOR_MAP_ENTRY(IDC_SLIDER1, ANF_LEFT | ANF_RIGHT | ANF_TOP )
	ANCHOR_MAP_ENTRY(IDC_STATIC_DUMP, ANF_LEFT | ANF_RIGHT | ANF_TOP | ANF_BOTTOM)
END_ANCHOR_MAP()


BEGIN_MESSAGE_MAP(CShmView, CDockablePaneChildView)
	ON_BN_CLICKED(IDOK, &CShmView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CShmView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CShmView::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CShmView::OnBnClickedButtonRecord)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CShmView::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_CHECK_REPEAT, &CShmView::OnBnClickedCheckRepeat)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CShmView::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, &CShmView::OnSelchangedTreeFile)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SYMTABLE, &CShmView::OnBnClickedCheckSymtable)
END_MESSAGE_MAP()


// CShmView message handlers
void CShmView::OnBnClickedOk()
{
}
void CShmView::OnBnClickedCancel()
{
}


BOOL CShmView::OnInitDialog()
{
	CDockablePaneChildView::OnInitDialog();

	InitAnchors();

	list<string> extList;
	extList.push_back("shm");
	m_FileTree.Update("../media/", extList);
	m_FileTree.ExpandAll();

	CRect rect;
	GetClientRect(rect);

	m_dumpWindow = new cMemDumpWindow();
	BOOL result = m_dumpWindow->Create(NULL, NULL, WS_VISIBLE | WS_CHILD,
		CRect(0, 0, 20, 40), this, AFX_IDW_PANE_FIRST);
	m_dumpWindow->SetScrollSizes(MM_TEXT, CSize(rect.Width() - 30, 100));
	m_dumpWindow->ShowWindow(SW_SHOW);

	return TRUE;
}


void CShmView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePaneChildView::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	HandleAnchors(&rcWnd);

	if (m_dumpWindow && m_dumpWindow->GetSafeHwnd())
	{
		CRect pwr;
		GetDlgItem(IDC_STATIC_DUMP)->GetWindowRect(pwr);
		ScreenToClient(pwr);
		m_dumpWindow->MoveWindow(pwr);
	}
}



void CShmView::OnBnClickedButtonOpen()
{
	if (m_isStart)
	{
		m_isStart = false;
		SetBackgroundColor(g_grayColor);
		m_OpenButton.SetWindowTextW(L"Open");
	}
	else
	{
		UpdateData();
		if (m_ShmName.IsEmpty())
		{
			::AfxMessageBox(L"Enter Shared Memory Name \n");
			return;
		}

		const string shmName = wstr2str(m_ShmName.GetBuffer(0));

		if (!m_shmMem.Init(shmName))
		{
			::AfxMessageBox(L"Error!! Open Shared Memory Name \n");
			return;
		}

		if (m_readMemorySize <= 0)
		{
			::AfxMessageBox(L"Error!! Too Low Memory Size\n");
			return;
		}

		if (m_IsShmSymbolTable)
		{
			if (!m_shmSymbols.Open(shmName))
			{
				::AfxMessageBox(L"Error!! Open Shared Memory Name \n");
				return;
			}
		}
		else
		{
			if (!m_readShmMem.Init(shmName, m_readMemorySize))
			{
				::AfxMessageBox(L"Error!! Open Shared Memory Failed");
				return;
			}
		}

		m_isStart = true;
		m_OpenButton.SetWindowTextW(L"Close");
		SetBackgroundColor(g_blueColor);
	}	
}


void CShmView::CheckFPS(const float deltaSeconds)
{
	m_checkUpdateTime += deltaSeconds;
	++m_updateFPS;
	if (m_checkUpdateTime > 1.f)
	{
		CString str;
		str.Format(L"Update FPS : %d", m_updateFPS);
		m_staticUpdateFPS.SetWindowTextW(str);
		m_checkUpdateTime = 0;
		m_updateFPS = 0;
	}
}


void CShmView::Update(const float deltaSeconds)
{
	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return;
	CheckFPS(m_incTime);
	m_incTime = 0;

	// Read Shared Memory
	if (m_isStart)
	{
		if (m_IsShmSymbolTable)
		{
			auto it = m_shmSymbols.begin();
			while (m_shmSymbols.end() != it)
			{
				script::g_symbols[it->first.c_str()] = it->second;
				++it;
			}
		}
		else
		{
			if (m_dumpWindow)
				m_dumpWindow->UpdateDump((char*)m_readShmMem.m_memPtr, m_readMemorySize);
		}
	}

	switch (m_state)
	{
	case RECORD:
		if (m_readShmMem.IsOpen())
		{
			 const int bufferLen = m_udpStream.Write((char*)m_readShmMem.m_memPtr, m_readShmMem.m_memoryByteSyze);
			 m_incRecordLength += bufferLen;

// 			if (m_dumpWindow)
// 				m_dumpWindow->UpdateDump((char*)m_readShmMem.m_memPtr, bufferLen);

			CString lenStr;
			lenStr.Format(L"%d", m_incRecordLength);
			m_recordLength.SetWindowTextW(lenStr);
		}
		break;

	case PLAY:
	{
		if (!m_udpPlayer.IsPlay())
			break;

		const int bufferLen = m_udpPlayer.Update(m_buffer, sizeof(m_buffer));
		if (bufferLen > 0)
		{
			if (m_playShmMem.IsOpen())
			{
				memcpy(m_playShmMem.m_memPtr, m_buffer, bufferLen);				
			}

			if (m_dumpWindow)
				m_dumpWindow->UpdateDump((char*)m_buffer, bufferLen);

 			const int curPos = m_udpPlayer.GetCurrentPlayElementIndex();
 			m_sliderPlay.SetPos(curPos);
 
 			CString playPosStr;
 			playPosStr.Format(L"%d / %d", curPos, m_udpPlayer.GetTotalElementSize());
			m_playTime.SetWindowTextW(playPosStr);
		}
		else if (bufferLen < 0) // 종료
		{
			if (m_checkRepeat)
			{
 				ChangeState(STOP);
 				ChangeState(PLAY);
			}
			else
			{
				ChangeState(STOP);
			}
		}
	}
	break;	
	default:
		break;
	}
}


void CShmView::OnBnClickedButtonRecord()
{
	UpdateData();
	ChangeState((m_state == RECORD) ? STOP : RECORD);
}


void CShmView::OnBnClickedButtonPlay()
{
	UpdateData();
	ChangeState((m_state == PLAY) ? STOP : PLAY);
}


void CShmView::OnBnClickedCheckRepeat()
{
	UpdateData();
}


void CShmView::OnBnClickedCheckSymtable()
{
	UpdateData();
}


void CShmView::OnBnClickedButtonRefresh()
{
	list<string> extList;
	extList.push_back("shm");
	m_FileTree.Update("../media/", extList);
	m_FileTree.ExpandAll();
}


void CShmView::OnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_FileTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty()
		|| (fileName == "../media")
		|| (fileName == "..")
		|| (fileName == "../media/udpanalyzer"))
	{
		//m_FileInfoTree.DeleteAllItems();
		m_playFileName.SetWindowTextW(L"");
		return;
	}

	cUDPStream udpStream;
	if (udpStream.Open(false, fileName))
	{
		m_playFileName.SetWindowTextW(str2wstr(fileName).c_str());
		m_sliderPlay.SetRange(0, udpStream.m_totalElementSize);

		CString playPosStr;
		playPosStr.Format(L"%d / %d", 1, m_udpPlayer.GetTotalElementSize());
		m_playTime.SetWindowTextW(playPosStr);
	}
	else
	{
		AfxMessageBox(L"Shm Stream 파일이 아닙니다.");
	}
}


bool CShmView::Play()
{
	UpdateData();

	const string fileName = m_FileTree.GetSelectFilePath(m_FileTree.GetSelectedItem());
	if (fileName.empty())
	{
		::AfxMessageBox(L"Error!! Not Select File");
		return false;
	}

	if (m_playSharedMemName.IsEmpty())
	{
		::AfxMessageBox(L"Error!! Enter Play Shared Memory Name ");
		return false;
	}

	if (!m_playShmMem.Init(wstr2str((LPCTSTR)m_playSharedMemName), 256))
	{
		return false;
	}

	m_playFileName.SetWindowTextW(str2wstr(fileName).c_str());
	m_udpPlayer.Open(fileName);
	m_udpPlayer.Play();
	return true;
}


void CShmView::RecordEnd()
{
	m_udpStream.Close();
	TCHAR szFilter[] = L"UDPStreaming(*.shm)|*.shm| All Files(*.*)|*.*||";
	CFileDialog dlg(FALSE, L"shm", NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		// 저장이 완료된 임시파일의 이름을 변경한다.
		rename(g_tempShmStreamFileName.c_str(), wstr2str((LPCTSTR)dlg.GetPathName()).c_str());
	}
	OnBnClickedButtonRefresh(); // 파일리스트 업데이트
}


bool CShmView::RecordStart()
{
	UpdateData();

	if (!m_readShmMem.IsOpen())
	{
		::AfxMessageBox(L"Error!! Open Shared Memory First");
		return false;
	}

	if (m_udpStream.Open(true, g_tempShmStreamFileName, m_readMemorySize))
	{
		m_state = RECORD;
		SetBackgroundColor(RGB(255, 127, 80));
		m_buttonRecord.SetWindowTextW(L"Stop");
	}
	else
	{
		::AfxMessageBox(formatw("shared memory record error!! [%s]", g_tempShmStreamFileName.c_str()).c_str());
		return false;
	}

	m_incRecordLength = 0;

	return true;
}


CShmView::STATE CShmView::ChangeState(const STATE nextState)
{
	switch (m_state)
	{
		// Stop State
	case CShmView::STOP:
	{
		switch (nextState)
		{
		case CShmView::STOP:
			break;
		case CShmView::PLAY:
		{
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(FALSE);
// 			m_UDPIP.EnableWindow(FALSE);
// 			m_UDPPort.EnableWindow(FALSE);
// 			m_FileTree.EnableWindow(FALSE);
// 			m_FileInfoTree.EnableWindow(FALSE);
// 			m_RefreshButton.EnableWindow(FALSE);
// 			m_PlayButton.SetWindowTextW(L"Pause");
// 			SetBackgroundColor(g_playColor);
			if (Play())
			{
				m_buttonPlay.SetWindowTextW(L"Stop");
				SetBackgroundColor(g_blueColor);
			}
		}
		break;
		case CShmView::PAUSE:
//			m_PlayButton.SetWindowTextW(L"Play");
			break;
		case CShmView::RECORD:
// 			m_PlayButton.EnableWindow(FALSE);
// 			m_StopButton.EnableWindow(FALSE);
// 			m_RecordButton.EnableWindow(TRUE);
// 			m_UDPIP.EnableWindow(FALSE);
// 			m_UDPPort.EnableWindow(FALSE);
// 			m_FileTree.EnableWindow(FALSE);
// 			m_RefreshButton.EnableWindow(FALSE);
// 			m_FileInfoTree.EnableWindow(FALSE);
// 			m_RecordButton.SetWindowTextW(L"Record End");
// 			SetBackgroundColor(RGB(255, 127, 80));

//			m_udpStream.Open(true, g_temporaryUDPStreamFileName);
			if (!RecordStart())
				return m_state;
			break;

		default:
			break;
		}
		m_state = nextState;
	}
	break;

	// Play State
	case CShmView::PLAY:
	{
		switch (nextState)
		{
		case CShmView::STOP:
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(TRUE);
// 			m_UDPIP.EnableWindow(TRUE);
// 			m_UDPPort.EnableWindow(TRUE);
// 			m_FileTree.EnableWindow(TRUE);
// 			m_FileInfoTree.EnableWindow(TRUE);
// 			m_RefreshButton.EnableWindow(TRUE);
// 			m_PlayButton.SetWindowTextW(L"Play");
// 			m_RecordButton.SetWindowTextW(L"Record");
			SetBackgroundColor(g_grayColor);

			m_buttonPlay.SetWindowTextW(L"Play");
			m_udpPlayer.Stop();
			break;

		case CShmView::PLAY:
			break;

		case CShmView::PAUSE:
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(FALSE);
// 			m_PlayButton.SetWindowTextW(L"Play");
			SetBackgroundColor(RGB(119, 136, 153));

			m_udpPlayer.Pause();
			break;

		case CShmView::RECORD:
			break;

		default:
			break;
		}
		m_state = nextState;
	}
	break;

	// Pause State
	case CShmView::PAUSE:
	{
		switch (nextState)
		{
		case CShmView::STOP:
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(TRUE);
// 			m_UDPIP.EnableWindow(TRUE);
// 			m_UDPPort.EnableWindow(TRUE);
// 			m_FileTree.EnableWindow(TRUE);
// 			m_FileInfoTree.EnableWindow(TRUE);
// 			m_RefreshButton.EnableWindow(TRUE);
// 			m_PlayButton.SetWindowTextW(L"Play");
// 			m_RecordButton.SetWindowTextW(L"Record");
// 			SetBackgroundColor(RGB(240, 240, 240));

			m_udpPlayer.Stop();
			break;

		case CShmView::PLAY:
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(FALSE);
// 			m_PlayButton.SetWindowTextW(L"Pause");
// 			SetBackgroundColor(g_playColor);
//			m_udpPlayer.Play();
			break;

		case CShmView::PAUSE:
			break;

		case CShmView::RECORD:
			break;

		default:
			break;
		}
		m_state = nextState;
	}
	break;

	case CShmView::RECORD:
	{
		switch (nextState)
		{
		case CShmView::STOP:
// 			m_PlayButton.EnableWindow(TRUE);
// 			m_StopButton.EnableWindow(TRUE);
// 			m_RecordButton.EnableWindow(TRUE);
// 			m_UDPIP.EnableWindow(TRUE);
// 			m_UDPPort.EnableWindow(TRUE);
// 			m_FileTree.EnableWindow(TRUE);
// 			m_FileInfoTree.EnableWindow(TRUE);
// 			m_RefreshButton.EnableWindow(TRUE);
// 			m_PlayButton.SetWindowTextW(L"Play");
// 			m_RecordButton.SetWindowTextW(L"Record");
// 			SetBackgroundColor(RGB(240, 240, 240));
			m_buttonRecord.SetWindowTextW(L"Record");
			SetBackgroundColor(g_grayColor);
 			RecordEnd();
			break;

		case CShmView::PLAY:
		case CShmView::PAUSE:
		case CShmView::RECORD:
			break;

		default:
			break;
		}
		m_state = nextState;
	}
	break;

	default:
		break;
	}

	return m_state;
}

