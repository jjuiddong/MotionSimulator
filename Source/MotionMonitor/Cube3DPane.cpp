// Cube3DView.cpp : implementation file
//

#include "stdafx.h"
#include "MotionMonitor.h"
#include "Cube3DPane.h"


// CCube3DView
CCube3DPane::CCube3DPane()
{
}

CCube3DPane::~CCube3DPane()
{
}


BEGIN_MESSAGE_MAP(CCube3DPane, CDockablePaneBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCube3DView message handlers
int CCube3DPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_view = new C3DView();
	m_view->Create(IDD_DIALOG_3D, this);
	m_view->ShowWindow(SW_SHOW);
	SetChildView(m_view);

	cController::Get()->Init(m_view->GetRenderer());
	m_view->SetRenderCube(true);
	
	graphic::GetMainCamera()->SetCamera(Vector3(-8.5f, 2.4f, -3.2f), Vector3(0, 0, 0), Vector3(0, 1, 0));

	return 0;
}
