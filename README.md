# 작업 일지
<26/03/02>
GameTimer완성
InitMainWindow(윈도우 생성 함수) 완성
# 버그 노트

# 클래스 및 함수 설명
Application Class : 애플리케이션의 생명주기와 Directx12의 기본 자원을 관리하는 클래스
function
Application(생성자)
  - 싱글톤 패턴을 적용하여 애플리케이션 내에 오직 하나의 Instance를 가질 수 있도록 보장하고, 전역적인 접근점을 제공함
MsgProc
  - WM_ACTIVATE를 사용해서 윈도우의 활성화 상태를 확인해서 Timer를 멈추거나 작동 시킨다. 그로 인해서 불필요한 시간 계산을 줄이고 비활성화 상태일 때 리소스의 낭비를 방지한다.
  - WM_SIZE를 사용해서 윈도우의 크기를 갱신한다
  - SIZE_MINIMIZED, SIZE_MAXMIZED : 윈도우의 창을 내렸을 때와 최대로 늘렸을 때에 맞춰 타이머를 멈출지 말지를 정한다.
  - SIZE_RESTORED : 윈도우의 창을 원래대로 되돌렸을 때 이전 상태에 따른 처리 작성
  - WM_ENTERSIZEMOVE : 윈도우의 위치를 변경 할 때 타이머를 멈춰서 리소스 낭비를 방지
  - WM_EXITSIZEMOVE : 윈도우의 위치 변경을 종료 할 때 타이머를 다시 사작
InitMainWindow
  - 
