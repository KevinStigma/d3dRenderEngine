#ifndef GAMETIMER_H
#define GAMETIMER_H
class GameTimer
{
public:
	GameTimer();
	float GameTime()const;
	float DeltaTime()const;
	float TotalTime()const;
	void Reset();
	void Start();
	void Stop();
	void Tick();
private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pauseTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;
	bool m_stopped;
};
#endif