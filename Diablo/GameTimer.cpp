#include "GameTimer.h"
#include<windows.h>
GameTimer::GameTimer()
	:mSecondPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0), mStopTime(0), mPrevTime(0), mCurrTime(0), mIsStopped(false)
{
	//GameTimer을 호출 할 때마다 시작 시간을 기록하기 위해 사용
	__int64 countPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&countPerSec);  //태초로 부터 몇 번 카운트 되었는지를 반환한다
	mSecondPerCount = 1.0 / (double)countPerSec;  //초 당 몇 번 카운트 되었는지 알기 위해 1초/전체 틱 수를 해준다
}

float GameTimer::TotalTime()const
{
	if (mIsStopped)
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondPerCount);  //정지 상태면 ((정지된 시간 - 일시 정지 시간)-태초의 시간)*초 당 틱수
	else
		return(float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondPerCount);  //((현재 시간 - 일시 정지 시간)-태초의 시간)*초 당 틱수
}
float GameTimer::DeltaTime()const
{
	return mDeltaTime;
}

void GameTimer::Rest()  //변수를 Reset해준다
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;  //태초 시간을 현재 시간으로 설정
	mPrevTime = currTime;  //이전 시간을 현재 시간으로 설정
	mStopTime = 0;  //Stop시간을 0으로 설정
	mIsStopped = false;  //Stop 여부를 false로 설정
}
void GameTimer::Start()
{
	__int64 startTime;  //재시작 시 시간을 담을 변수
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);  //재시작 시 현재 시간을 startTime에 담는다

	if (mIsStopped)  //만약 Stop 함수가 호출 됬었으면
	{
		mPausedTime += (startTime - mStopTime);  //시작 시간 - 정지된 시간을 mPausedTime에 누적

		mPrevTime = startTime;  //이전 시간을 startTime으로 설정
		mStopTime = 0;  //시작 했기에 mStopTime을 0으로 설정
		mIsStopped = false;  //시작 했기에 mIsStopped를 false로 설정
	}
}
void GameTimer::Stop()
{
	if (!mIsStopped)  //이미 실행 했으면 무시
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;  //정지된 시점의 현재 시간을 mStopTime 변수에 넣는다
		mIsStopped = true;  //Stop 여부를 true로 변경
	}
}
void GameTimer::Tick()  //매 프레임 마다 mDeltaTime을 계산
{
	if (mIsStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondPerCount;

	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;

	}
}