#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
private:
	double mSecondPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mIsStopped;

public:
	GameTimer();

	float TotalTime()const;
	float DeltaTime()const;

	void Rest();
	void Start();
	void Stop();
	void Tick();
};
#endif 
