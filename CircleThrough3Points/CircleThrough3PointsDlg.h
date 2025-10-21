
// CircleThrough3PointsDlg.h: 헤더 파일
//
#include "CCanvasStatic.h"
#include <atomic>
#pragma once

constexpr UINT WM_RANDOM_DONE = WM_USER + 201;

// CCircleThrough3PointsDlg 대화 상자
class CCircleThrough3PointsDlg : public CDialogEx
{
// 생성입니다.
public:
	CCircleThrough3PointsDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLETHROUGH3POINTS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnRandom();
	static UINT __cdecl RandomWorkerProc(LPVOID p);
	CWinThread* m_pWorker{ nullptr };
	bool m_randomRunning{ false };

	std::atomic_bool m_cancelRandom{ false };
private:
    CCanvasStatic m_canvas;
    void SyncEditsToCanvas(); // 에딧값 → 캔버스
	afx_msg LRESULT OnRandomDone(WPARAM, LPARAM);
};
