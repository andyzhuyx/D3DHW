#include "Timer.h"

Timer::Timer()
	:
	_SecPerCount(0.0f),
	_DeltaTime(-1.0f),
	_BaseTime(0),
	_CurrTime(0),
	_PrevTime(0),
	_StopTime(0),
	_PausedTime(0),
	_IsStopped(false)
{
	__int64 CountPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&CountPerSec);
	_SecPerCount = 1.0 / (double)CountPerSec;
}

void Timer::Start() {
	if (_IsStopped) {
		__int64 curr;
		QueryPerformanceCounter((LARGE_INTEGER*)&curr);
		_PausedTime += curr - _StopTime;
		
		_StopTime = 0;
		_PrevTime = curr;
		_CurrTime = curr;
		_IsStopped = false;
	}
}

void Timer::Reset() {
	__int64 curr;
	QueryPerformanceCounter((LARGE_INTEGER*)&curr);

	_BaseTime = curr;
	_PrevTime = curr;
	_IsStopped = false;
	_StopTime = 0;
	_PausedTime = 0;
}

void Timer::Stop() {
	if (!_IsStopped) {
		__int64 curr;
		QueryPerformanceCounter((LARGE_INTEGER*)&curr);
		_StopTime = curr;
		_IsStopped = true;
	}
}

void Timer::Tick() {
	if (_IsStopped) {
		_DeltaTime = 0.0;
		return;
	}
	__int64 curr;
	QueryPerformanceCounter((LARGE_INTEGER*)&curr);
	_CurrTime = curr;
	_DeltaTime = (_CurrTime - _PrevTime) * _SecPerCount;
	_PrevTime = curr;

	if (_DeltaTime < 0.0) {
		_DeltaTime = 0.0;
	}
}

float Timer::GetTotalTime() const{
	if (_IsStopped) {
		return (float)((_StopTime - _BaseTime - _PausedTime) * _SecPerCount);
	}
	else {
		__int64 curr;
		QueryPerformanceCounter((LARGE_INTEGER*)&curr);
		return (float)((curr - _BaseTime - _PausedTime) * _SecPerCount);
	}
}

float Timer::GetDeltaTime() const {
	return (float)_DeltaTime;
}