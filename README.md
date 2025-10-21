🧩 주요 기능
-

🖱️ 마우스 클릭	픽처 컨트롤 영역에서 3개의 점을 클릭하면 자동으로 정원 계산 및 표시

🔄 Reset 버튼	점과 원을 모두 초기화하고, 랜덤 동작 중이면 즉시 중단

🎲 Random 버튼	0.5초 간격으로 무작위 점 3개를 생성 (총 10회 반복)

💾 이미지 저장	각 랜덤 단계마다 현재 화면을 PNG로 자동 저장 (snapshots/step_01.png 등)

🧮 수학적 계산	세 점의 외심(원 중심)과 반지름을 계산하는 기하 알고리즘 포함

⚙️ 코드 구성
-

CircleThrough3PointsDlg.cpp / .h     →   메인 다이얼로그, 버튼 핸들러 및 랜덤 스레드 관리

CCanvasStatic.cpp / .h     →   커스텀 픽처 컨트롤 (점, 원 그리기 및 마우스 이벤트 처리)

Resource.h / .rc     →   UI 리소스 및 컨트롤 ID 정의

snapshots/    →    랜덤 동작 중 자동 저장된 이미지 파일 폴더


💻 실행 방법
-
1. Visual Studio에서 솔루션(.sln) 파일을 엽니다.
2. Debug 또는 Release 빌드 후 실행합니다.
3. 프로그램 실행 후:
- 픽처 컨트롤 영역에서 마우스로 3개의 점을 찍습니다.
- 원이 자동으로 그려집니다.
- [랜덤] 버튼을 누르면 10단계 자동 시뮬레이션이 진행됩니다.
- [리셋] 버튼으로 언제든 초기화 가능합니다.
4. 랜덤 실행 중 자동 저장된 이미지 파일은
→ 프로젝트 이름/x64/debug/snapshots/step_01.png … step_10.png 로 생성됩니다.

🧠 주요 구현 내용 요약
-
ComputeCircleThrough3Points()
→ 세 점으로부터 외심과 반지름 계산

OnLButtonDown() / OnPaint()
→ 마우스 입력 기반 점 추가 및 원 실시간 렌더링

OnBnClickedBtnRandom()
→ 별도 스레드에서 랜덤 좌표 생성 및 캔버스 갱신

SaveSnapshotPNG()
→ GDI+ 기반으로 화면 이미지를 PNG로 저장
