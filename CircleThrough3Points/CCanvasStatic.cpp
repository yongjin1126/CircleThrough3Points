// CCanvasStatic.cpp: 구현 파일
//

#include "pch.h"
#include "CircleThrough3Points.h"
#include "CCanvasStatic.h"
#include <memory>

using namespace Gdiplus;
// CCanvasStatic

IMPLEMENT_DYNAMIC(CCanvasStatic, CStatic)

CCanvasStatic::CCanvasStatic()
	: m_ptCount(0), m_dragIndex(-1),
	m_pointR(10.f), m_borderW(3.f),
	m_hasCircle(false), m_radius(0.f),
	m_idP1(0), m_idP2(0), m_idP3(0)
{

}

CCanvasStatic::~CCanvasStatic()
{
}


BEGIN_MESSAGE_MAP(CCanvasStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_RANDOM_TICK, &CCanvasStatic::OnRandomTick)
END_MESSAGE_MAP()



// CCanvasStatic 메시지 처리기

static ULONG_PTR g_gdiplusToken = 0;
static void EnsureGdiplus()
{
	if (!g_gdiplusToken) {
		GdiplusStartupInput si; GdiplusStartup(&g_gdiplusToken, &si, nullptr);
	}
}
static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0, size = 0; GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;
	std::unique_ptr<BYTE[]> buf(new BYTE[size]);
	ImageCodecInfo* p = (ImageCodecInfo*)buf.get();
	GetImageEncoders(num, size, p);
	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(p[j].MimeType, format) == 0) { *pClsid = p[j].Clsid; return (int)j; }
	}
	return -1;
}
static CString ExeDir()                       // ★ 저장 폴더 경로
{
	wchar_t buf[MAX_PATH]; GetModuleFileName(nullptr, buf, MAX_PATH);
	CString p(buf); int pos = p.ReverseFind('\\'); if (pos >= 0) p = p.Left(pos + 1);
	return p;
}


void CCanvasStatic::Reset()
{
	m_ptCount = 0;
	m_dragIndex = -1;
	m_hasCircle = false;
	Invalidate(FALSE);
}

bool CCanvasStatic::SaveSnapshotPNG(const CString& path)
{
	EnsureGdiplus();
	CClientDC dc(this);
	CRect rc; GetClientRect(&rc);

	CDC mem; mem.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* old = mem.SelectObject(&bmp);
	mem.FillSolidRect(rc, RGB(255, 255, 255));
	DrawAll(&mem);                    // 현재 장면 그리기
	mem.SelectObject(old);

	Bitmap* image = Bitmap::FromHBITMAP((HBITMAP)bmp.GetSafeHandle(), NULL);
	if (!image) return false;
	CLSID clsid; if (GetEncoderClsid(L"image/png", &clsid) < 0) { delete image; return false; }
	Status st = image->Save(path, &clsid, NULL);
	delete image;
	return st == Ok;
}

void CCanvasStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.
	CRect rc; GetClientRect(&rc);
	CDC mem; mem.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* old = mem.SelectObject(&bmp);
	mem.FillSolidRect(rc, RGB(255, 255, 255));

	DrawAll(&mem);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &mem, 0, 0, SRCCOPY);
	mem.SelectObject(old);
}

void CCanvasStatic::DrawAll(CDC* pDC)
{
	// 클릭점(채움)
	for (int i = 0; i < m_ptCount; ++i)
		DrawFilledDot(pDC, m_pts[i], m_pointR);

	// 세 점 원(윤곽선만)
	if (m_hasCircle)
		DrawCircleOutline(pDC, m_center, m_radius, (int)m_borderW);
}

void CCanvasStatic::DrawFilledDot(CDC* pDC, const POINTF& c, float r)
{
	const int N = 72;
	std::vector<POINT> poly(N);
	for (int i = 0; i < N; ++i) {
		float ang = (float)i * 2.f * (float)M_PI / (float)N;
		poly[i].x = (LONG)lround(c.x + r * cosf(ang));
		poly[i].y = (LONG)lround(c.y + r * sinf(ang));
	}
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush br(RGB(0, 0, 0));
	CPen* op = pDC->SelectObject(&pen);
	CBrush* ob = pDC->SelectObject(&br);
	pDC->Polygon(poly.data(), (int)poly.size());
	pDC->SelectObject(ob);
	pDC->SelectObject(op);
}

void CCanvasStatic::DrawCircleOutline(CDC* pDC, const POINTF& c, float r, int penW)
{
	const int N = 360;
	std::vector<POINT> poly(N + 1);
	for (int i = 0; i <= N; ++i) {
		float ang = (float)i * (float)M_PI / 180.f; // 0~180deg*2
		poly[i].x = (LONG)lround(c.x + r * cosf(ang));
		poly[i].y = (LONG)lround(c.y + r * sinf(ang));
	}
	CPen pen(PS_SOLID, penW, RGB(0, 0, 0));
	CPen* op = pDC->SelectObject(&pen);
	pDC->Polyline(poly.data(), (int)poly.size());
	pDC->SelectObject(op);
}

bool CCanvasStatic::ComputeCircleThrough3Points(const POINTF& A, const POINTF& B, const POINTF& C, POINTF& O, float& R)
{
	float a1 = B.x - A.x, b1 = B.y - A.y;
	float a2 = C.x - A.x, b2 = C.y - A.y;
	float d =  (a1 * b2 - b1 * a2);
	if (fabsf(d) < 1e-3f) return false; // 거의 일직선

	float c1 = (a1 * (A.x + B.x) + b1 * (A.y + B.y)) * 0.5f;
	float c2 = (a2 * (A.x + C.x) + b2 * (A.y + C.y)) * 0.5f;

	O.x = (c1 * b2 - b1 * c2) / d;
	O.y = (a1 * c2 - c1 * a2) / d;
	R = sqrtf((O.x - A.x) * (O.x - A.x) + (O.y - A.y) * (O.y - A.y));
	return true;
}

void CCanvasStatic::RecomputeCircle()
{
	m_hasCircle = (m_ptCount == 3) &&
		ComputeCircleThrough3Points(m_pts[0], m_pts[1], m_pts[2], m_center, m_radius);
}

void CCanvasStatic::UpdateCoordUI()
{
	if (!GetParent()) return;
	CString s;
	if (m_idP1 && m_ptCount > 0) { s.Format(_T("P1:(%.0f,%.0f)"), m_pts[0].x, m_pts[0].y); GetParent()->SetDlgItemText(m_idP1, s); }
	if (m_idP2 && m_ptCount > 1) { s.Format(_T("P2:(%.0f,%.0f)"), m_pts[1].x, m_pts[1].y); GetParent()->SetDlgItemText(m_idP2, s); }
	if (m_idP3 && m_ptCount > 2) { s.Format(_T("P3:(%.0f,%.0f)"), m_pts[2].x, m_pts[2].y); GetParent()->SetDlgItemText(m_idP3, s); }
}

void CCanvasStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_dragIndex >= 0 && (nFlags & MK_LBUTTON)) {
		m_pts[m_dragIndex] = POINTF{ (float)point.x,(float)point.y };
		UpdateCoordUI();
		RecomputeCircle();
		Invalidate(FALSE); // 실시간
	}
	CStatic::OnMouseMove(nFlags, point);
}


void CCanvasStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACE(_T("Canvas LButtonDown: %d,%d\n"), point.x, point.y);
	POINTF p{ (float)point.x,(float)point.y };

	// 드래그 히트 테스트
	for (int i = 0; i < m_ptCount; ++i) {
		if (Dist2(m_pts[i], p) <= (m_pointR * m_pointR * 4.f)) { m_dragIndex = i; SetCapture(); return; }
	}

	// 클릭점 추가 (세 점까지만)
	if (m_ptCount < 3) {
		m_pts[m_ptCount++] = p;
		UpdateCoordUI();
		RecomputeCircle();
		Invalidate(FALSE);
	}


	CStatic::OnLButtonDown(nFlags, point);
}


void CCanvasStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_dragIndex >= 0) { ReleaseCapture(); m_dragIndex = -1; }

	CStatic::OnLButtonUp(nFlags, point);
}

LRESULT CCanvasStatic::OnRandomTick(WPARAM wParam, LPARAM lParam)
{
	POINTF* arr = reinterpret_cast<POINTF*>(lParam);
	if (arr) {
		for (int i = 0; i < 3; ++i) m_pts[i] = arr[i];
		delete[] arr;
		m_ptCount = 3;
		UpdateCoordUI();
		RecomputeCircle();
		Invalidate(FALSE);
	}

	// ★ wParam = 1..10 단계일 때 PNG 저장
	if (wParam >= 1 && wParam <= 10) {
		CString dir = ExeDir() + _T("snapshots\\");
		::CreateDirectory(dir, nullptr);
		CString fn; fn.Format(_T("%sstep_%02u.png"), dir.GetString(), (UINT)wParam);
		SaveSnapshotPNG(fn);
	}
	return 0;
}
