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
	return 0.0;
}
float GameTimer::DeltaTime()const
{
	return mDeltaTime;
}

void GameTimer::Rest()
{

}
void GameTimer::Start()
{

}
void GameTimer::Stop()
{

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