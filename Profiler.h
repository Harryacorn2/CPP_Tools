#ifndef PROFLIER_H
#define PROFLIER_H

#include <unordered_map>
#include <string>

#define PROFILE_SCOPE(name) \
Profiler::ScopedTimer name##_scope(Profiler::Get()->GetTimer(std::string(#name)))

class Profiler
{
public:
	class Timer
	{
	public:
		void Start();
		void Stop();
		void Reset();
		const std::string& GetName() const;
		double GetTime_ms() const;
		double GetMax_ms() const;
		double GetAvg_ms() const;

		std::string mName;
		double mCurrentFrameTime = 0.0f; // Time taken in this frame
		double mLongestFrameTime = 0.0f; // Time taken in longest frame
		double mTotalTime = 0.0f; // Total time this timer has been running
		int mFrameCount = 0; // Number of frames this timer has been on during
		std::chrono::high_resolution_clock::time_point mStart; // The moment in time the timer was started

	private:
		Timer(const std::string& name) : mName(name) {}
		~Timer() {};

		friend class Profiler;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(Timer* timer) : mTimer(timer) { mTimer->Start(); }
		~ScopedTimer() { mTimer->Stop(); }

	private:
		Timer* mTimer;
	};

	static Profiler* Get();
	Timer* GetTimer(const std::string& name);
	void ResetAll();

private:
	Profiler();
	~Profiler();
	void BeginTimer(const std::string& name, uint64_t startTime);
	void EndTimer(uint64_t endTime);

	FILE* mJsonFile;
	bool firstTime = true;
	std::unordered_map<std::string, Timer*> mTimers;

	friend class Timer;
};

#endif