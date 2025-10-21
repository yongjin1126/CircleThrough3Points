
// CircleThrough3PointsDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "CircleThrough3Points.h"
#include "CircleThrough3PointsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <random>
#include "CCanvasStatic.h"


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCircleThrough3PointsDlg 대화 상자



CCircleThrough3PointsDlg::CCircleThrough3PointsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLETHROUGH3POINTS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCircleThrough3PointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CANVAS, m_canvas);
}

BEGIN_MESSAGE_MAP(CCircleThrough3PointsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RESET, &CCircleThrough3PointsDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CCircleThrough3PointsDlg::OnBnClickedBtnRandom)
END_MESSAGE_MAP()


// CCircleThrough3PointsDlg 메시지 처리기

BOOL CCircleThrough3PointsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	SetDlgItemInt(IDC_EDIT_POINT_R, 10);
	SetDlgItemInt(IDC_EDIT_BORDER_W, 3);
	SetDlgItemText(IDC_ST_P1, _T("P1:(-,-)"));
	SetDlgItemText(IDC_ST_P2, _T("P2:(-,-)"));
	SetDlgItemText(IDC_ST_P3, _T("P3:(-,-)"));

	// 픽처컨트롤 서브클래싱
	m_canvas.SetCoordTargets(IDC_ST_P1, IDC_ST_P2, IDC_ST_P3);
	SyncEditsToCanvas();
	return TRUE;

}

void CCircleThrough3PointsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCircleThrough3PointsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCircleThrough3PointsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCircleThrough3PointsDlg::OnBnClickedBtnReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_randomRunning) {
		m_cancelRandom = true;    // 워커 루프에서 감지하여 즉시 break
	}

	m_canvas.Reset();
	SetDlgItemText(IDC_ST_P1, _T("P1:(-,-)"));
	SetDlgItemText(IDC_ST_P2, _T("P2:(-,-)"));
	SetDlgItemText(IDC_ST_P3, _T("P3:(-,-)"));
	SyncEditsToCanvas();

	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RESET)->EnableWindow(TRUE);
	m_randomRunning = false;
}


void CCircleThrough3PointsDlg::OnBnClickedBtnRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_randomRunning) return;
	if (!m_canvas.Has3Points()) { AfxMessageBox(_T("먼저 캔버스에 3점을 찍어주세요.")); return; }
	m_cancelRandom = false;
	m_randomRunning = true;
	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(FALSE);
	// GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
	m_pWorker = AfxBeginThread(RandomWorkerProc, this);
}

UINT __cdecl CCircleThrough3PointsDlg::RandomWorkerProc(LPVOID p)
{
	auto* dlg = reinterpret_cast<CCircleThrough3PointsDlg*>(p);
	HWND hwndCanvas = dlg->m_canvas.GetSafeHwnd();

	// 캔버스 내부 영역
	CRect rc; dlg->m_canvas.GetClientRect(&rc);
	std::random_device rd; std::mt19937 gen(rd());
	std::uniform_real_distribution<float> xd(0.f, (float)rc.Width());
	std::uniform_real_distribution<float> yd(0.f, (float)rc.Height());

	for (int i = 0; i < 10; ++i) {
		if (dlg->m_cancelRandom.load()) break;
		POINTF* arr = new POINTF[3];
		for (int k = 0; k < 3; ++k) { arr[k].x = xd(gen); arr[k].y = yd(gen); }
		::PostMessage(hwndCanvas, WM_RANDOM_TICK, (WPARAM)(i + 1), (LPARAM)arr); // ★ 1..10
		::Sleep(500); // 2Hz
	}
	::PostMessage(dlg->GetSafeHwnd(), WM_RANDOM_DONE, 0, 0);   // ★ 버튼 복구 신호
	return 0;
}

void CCircleThrough3PointsDlg::SyncEditsToCanvas()
{
	BOOL ok1 = FALSE, ok2 = FALSE;
	UINT pr = GetDlgItemInt(IDC_EDIT_POINT_R, &ok1, FALSE);
	UINT bw = GetDlgItemInt(IDC_EDIT_BORDER_W, &ok2, FALSE);
	if (ok1) m_canvas.SetPointRadius((float)pr);
	if (ok2) m_canvas.SetBorderWidth((float)bw);
}

LRESULT CCircleThrough3PointsDlg::OnRandomDone(WPARAM, LPARAM)
{
	m_randomRunning = false;
	m_cancelRandom = false;
	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RESET)->EnableWindow(TRUE);
	return 0;
}
