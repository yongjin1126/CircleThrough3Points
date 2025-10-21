#pragma once
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// UI 스레드 간 통신용(랜덤 이동)
constexpr UINT WM_RANDOM_TICK = WM_USER + 101;

// CCanvasStatic

class CCanvasStatic : public CStatic
{
	DECLARE_DYNAMIC(CCanvasStatic)

public:
	CCanvasStatic();
	virtual ~CCanvasStatic();

	void SetPointRadius(float r) { m_pointR = (r < 1.f ? 1.f : r); Invalidate(FALSE); }
	void SetBorderWidth(float w) { m_borderW = (w < 1.f ? 1.f : w); Invalidate(FALSE); }

	// 좌표 텍스트 찍을 곳(다이얼로그 스태틱 ID)
	void SetCoordTargets(UINT id1, UINT id2, UINT id3) { m_idP1 = id1; m_idP2 = id2; m_idP3 = id3; }
	
	void Reset();
	// 외부에서 좌표 읽고 싶을 때
	bool Has3Points() const { return m_ptCount == 3; }

	bool SaveSnapshotPNG(const CString& path);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnRandomTick(WPARAM, LPARAM); // 랜덤 이동 수신

	void DrawAll(CDC* pDC);
	void DrawCircleOutline(CDC* pDC, const POINTF& c, float r, int penW);
	void DrawFilledDot(CDC* pDC, const POINTF& c, float r);
	void RecomputeCircle();
	void UpdateCoordUI();
	static bool ComputeCircleThrough3Points(const POINTF& A, const POINTF& B, const POINTF& C, POINTF& O, float& R);
	static float Dist2(const POINTF& a, const POINTF& b) { float dx = a.x - b.x, dy = a.y - b.y; return dx * dx + dy * dy; }

private:
	// 데이터
	POINTF m_pts[3];
	int    m_ptCount;     // 0~3
	int    m_dragIndex;   // -1=없음
	float  m_pointR;      // 클릭점 반지름(px)
	float  m_borderW;     // 정원 테두리 두께(px)

	// 계산된 정원
	bool   m_hasCircle;
	POINTF m_center;
	float  m_radius;

	// 좌표 스태틱 ID (다이얼로그에 그려줌)
	UINT m_idP1, m_idP2, m_idP3;
};


