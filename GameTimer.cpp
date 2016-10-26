#include "GameTimer.h"
#include <d3d11.h>
GameTimer::GameTimer() :m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0), m_pauseTime(0),
m_prevTime(0), m_currTime(0), m_stopped(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

void GameTimer::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}
	
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	m_deltaTime = (m_currTime - m_prevTime)*m_secondsPerCount;
	m_prevTime = m_currTime;

	//The m_Deltatime may be negative because of hardware error
	if (m_deltaTime < 0.0)
		m_deltaTime = 0.0;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&(currTime));
	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_stopped = false;
}

float GameTimer::DeltaTime()const
{
	return (float)m_deltaTime;
}

void GameTimer::Stop()
{
	if (!m_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_stopTime = currTime;
		m_stopped = true;
	}
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (m_stopped)
	{
		m_pauseTime += (startTime - m_stopTime);
		m_prevTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

float GameTimer::TotalTime()const
{
	if (m_stopped)
	{
		return (float)(((m_stopTime-m_pauseTime)-m_baseTime)*m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currTime - m_pauseTime) - m_baseTime)*m_secondsPerCount);
	}
}

