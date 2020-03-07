#pragma once
#include <Windows.h>
class Timer{
public:
	Timer();

	void Start();
	void Reset();
	void Stop();
	void Tick();

	float GetTotalTime()const;
	float GetDeltaTime()const;
private:
	double _SecPerCount;
	double _DeltaTime;


	__int64 _BaseTime;
	__int64 _CurrTime;
	__int64 _PrevTime;
	__int64 _StopTime;
	__int64 _PausedTime;

	bool _IsStopped;
};