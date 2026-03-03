# 작업 일지
<26/03/02>
GameTimer 클래스 완성
InitMainWindow 함수 완성
<26/03/03>
Run 함수 완성
CalculateFrameState 함수 완성
AnsiToWString 함수 완성
ThrowIfFailed 매크로 완성
# 내일 할 일 정리
Rtv Heap 만들기, Dsv Heap 만들기, SwapChain 만들기, OnResize 함수 마무리
# 버그 노트
메크로 멀티라인 작성 시 각 줄 뒤에는 \(역슬레쉬)를 넣어야 한다
# 핵심 기법
고해상도 타이머 및 프레임 독립성 (Class GameTimer)
  - QueryPerformanceCounter를 사용하여 마이크로초 단위의 정밀한 시간측정을 통해 시간계산에 대한 정밀도를 보장한다
  - 매 프레임 마다 DeltaTime을 계산하여 컴퓨터의 성능과 상관없이 게임 내 물체가 움직이게 한다
  - 일시정지(Stop)와 재개(Start)를 사용함으로써 윈도우를 사용하지 않을 때와 크기를 조정할 때 시간이 왜곡되는것을 방지한다
    
애플리케이션 구조 및 리소스 관리(Class Application)
  - 애플리케이션에 싱글톤 패턴을 적용시킴으로 써 인스터스를 하나 만 가질 수 있도록 보장하고 전역으로 접근할 수 있게 해준다
  - WM_ACTIVATE를 사용해서 윈도우가 비활성화 상태일 때 타이머와 렌더링 루프를 정지 시킴으로 써 CPU 및 GPU의 점유율을 최소화 한다
  - 사용자가 윈도우의 크기를 조절하는 동안 WM_ENTERSIZEMOVE를 사용해서 타이머를 정지시켜서 불피요한 자원사용을 막는다. 작업이 끝나면 WM_EXITSIZEMOVE를 호출해 타이머를 재개하고 Resize작업을 한다

윈도우 추상화 및 안정성(Function InitMainWindow)
  -  AdjustWindowRect함수를 사용해서 상단 바를 제외한 실제 그림이 그려지는 영역을 해상도와 일치 시킨다
  -  윈도우 클래스를 등록하는 RegisterClass와 창을 생성하는 CreateWindow를 검사하여 실패 시 에러메시지를 출력하고 즉시 종료하여 비정상적인 메모리 접근을 방지한다

데드 타임(Dead-Time) 루프
  - PeekMessage를 사용하여 메시지 큐에 실행할 메시지가 없는 데드 타임에도 Update와 Draw 함수를 호출하여 애니메이션 및 실시간 렌더링이 끊기지 않게한다

비활성화 시 자원관리
  - mAppPaused가 true일 경우에는 윈도우가 비활성화 상태임을 감지하고 Sleep을 호출하여 스레드를 대기상태로 전환하여 불필요한 루프 반복을 줄여 CPU 점유율과 전력소모를 낮춰 리소스 관리를 최적화함 

# 클래스 및 함수 기능 설명
Class GameTimer
  - 고해상도 타이머를 사용하여 독립적인 시간 계산과 FPS 통계를 관리한다
Function
생성자 
  - GameTimer를 호출할 때 시작 부터 카운트가 몇 번 호출 됬는지 알려준다
TotalTimer
  - 일시 정지된 시간을 제외한 전체 지난 시간을 반환한다
DeltaTime
  - 프레임 간 간격을 반환한다
Reset
  - 루프를 시작할 때 호출하여 정확한 시간 계산을 하게 해준다
Start
  - 정지 후 다시 시작할 때 정확한 시간을 계산하기 위해 정지된 시간을 mPausedTime에 누적시킨다. 그리고 다시 시작된 시점을 mPrevTime에 담는다
Stop
  - 정확한 시간 계산을 위해 정지된 시간을 담아둔다
Tick
  - 매 프레임마다 mDeltaTime을 계산한다. 그리고 mDeltaTime이 음수가 되면 문제가 발생할 수 있기 때문에 mDeltaTime이 음수면 0으로 고정시킨다
    
Class Application 
  - 애플리케이션의 생명주기와 Directx12의 기본 자원을 관리하는 클래스
Function
생성자
  - 싱글톤 패턴을 적용하여 애플리케이션 내에 오직 하나의 Instance를 가질 수 있도록 보장하고, 전역적인 접근점을 제공함
MsgProc
  - WM_ACTIVATE를 사용해서 윈도우의 활성화 상태를 확인해서 Timer를 멈추거나 작동 시킨다. 그로 인해서 불필요한 시간 계산을 줄이고 비활성화 상태일 때 리소스의 낭비를 방지한다.
  - WM_SIZE를 사용해서 윈도우의 크기를 갱신한다
  - SIZE_MINIMIZED, SIZE_MAXMIZED : 윈도우의 창을 내렸을 때와 최대로 늘렸을 때에 맞춰 타이머를 멈출지 말지를 정한다.
  - SIZE_RESTORED : 윈도우의 창을 원래대로 되돌렸을 때 이전 상태에 따른 처리 작성
  - WM_ENTERSIZEMOVE : 윈도우의 위치를 변경 할 때 타이머를 멈춰서 리소스 낭비를 방지
  - WM_EXITSIZEMOVE : 윈도우의 위치 변경을 종료 할 때 타이머를 다시 사작
  - WM_GETMINMAXINFO : 윈도우의 최소 크기를 제한해 너무 작아졌을 때 오류가 발생하는 것을 방지한다
  - WM_MENUCHAR : 윈도의 크기를 최대로 키우는 단축키 인 Alt+Enter를 눌렀을 때 비프(beep)음 내는걸 방지해서 UX(유저경험)을 해치지 않는다
  - DefWindowProc : 애플리케이션이 직접 처리하지 않는 윈도우 표준 메시지를 운영체제에 위임하여 표준 윈도우 동작과의 호환성을 유지한다
InitMainWindow
  - 창의 스타일, 메시지 처리기(MainWndProc), 배경색 등을 설정하여 운영체제에 애플리케이션의 명세 등록한다
  - ShowWindow와 UpdateWindow를 호출하여 생성된 창을 즉시 화면에 출력하고 초기 화면을 그리도록 명령한다
Run
  - 매 프레임 마다 렌더, 프레임, 위치 등을 갱신한다
CalculateFrameState
  - 성능 지표인 프레임을 실기간으로 확인 함으로써 최적화 상태를 실시간으로 확인하며 작업할 수 있도록 한다

Class AppUtility 
  - 반복적으로 호출될 수 있는 문자열 변환, 수학 연산, 리소스 관리 등을 한곳에 모아 관리한다
Function

Class DxException 
  - 정확한 에러 메시지의 정보를 넘겨주기 위한 클래스
macro
ThrowIfFailed
  - 런타임시 에러를 즉시 포착하고 화면에 출력하기 위해 작성


