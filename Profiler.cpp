#include "Profiler.h"
#include <thread>
#include <stdio.h>

void Profiler::Timer::Start()
{
	mStart = std::chrono::high_resolution_clock::now();
	Profiler::Get()->BeginTimer(mName, mStart.time_since_epoch().count()/1000);
}

void Profiler::Timer::Stop()
{
	auto stoppingTime = std::chrono::high_resolution_clock::now();
	double localDuration = std::chrono::duration<double, std::milli>(stoppingTime - mStart).count();
	mCurrentFrameTime += localDuration;
	Profiler::Get()->EndTimer(stoppingTime.time_since_epoch().count()/1000);
}

void Profiler::Timer::Reset()
{
	mTotalTime += mCurrentFrameTime;
	mFrameCount++;
	if (mLongestFrameTime < mCurrentFrameTime) {
		mLongestFrameTime = mCurrentFrameTime;
	}
	mCurrentFrameTime = 0.0f;
}

const std::string& Profiler::Timer::GetName() const
{
	return mName;
}

double Profiler::Timer::GetTime_ms() const
{
	return mCurrentFrameTime;
}

double Profiler::Timer::GetMax_ms() const
{
	return mLongestFrameTime;
}

double Profiler::Timer::GetAvg_ms() const
{
	return (mTotalTime / mFrameCount);
}

Profiler::Profiler()
{
	mJsonFile = fopen("profile.json", "w");
	std::fprintf(mJsonFile, "[\n");
}

Profiler::~Profiler()
{
	FILE* pFile;

	pFile = fopen("profile.txt", "w");

	std::fprintf(pFile, "Name: avg(ms), max(ms)\n");

	for (auto it = mTimers.begin(); it != mTimers.end(); it++) {
		// txt
		std::fprintf(pFile, "%s: %f, %f\n", it->first.c_str(), it->second->GetAvg_ms(), it->second->GetMax_ms());

		// delete
		delete it->second;
	}

	std::fprintf(mJsonFile, "\n]");

	std::fclose(pFile);
	std::fclose(mJsonFile);
}

Profiler* Profiler::Get()
{
	static Profiler theProfiler = Profiler();
	return &theProfiler;
}

Profiler::Timer* Profiler::GetTimer(const std::string& name)
{
	std::unordered_map<std::string, Timer*>::const_iterator it = mTimers.find(name);
	if (it == mTimers.end()) {
		mTimers[name] = new Timer(name);
		return mTimers[name];
	}

	return it->second;
}

void Profiler::ResetAll()
{
	for (auto it = mTimers.begin(); it != mTimers.end(); it++) {
		it->second->Reset();
	}
}

void Profiler::BeginTimer(const std::string& name, uint64_t startTime)
{
	size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
	if (firstTime) {
		std::fprintf(mJsonFile, "{\"name\": \"%s\", \"pid\": 1, \"tid\": %u, \"ph\": \"B\", \"ts\": %llu}", name.c_str(), tid, startTime);
		firstTime = false;
		return;
	}
	std::fprintf(mJsonFile, ",\n{\"name\": \"%s\", \"pid\": 1, \"tid\": %u, \"ph\": \"B\", \"ts\": %llu}", name.c_str(), tid, startTime);
}

void Profiler::EndTimer(uint64_t endTime)
{
	size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
	std::fprintf(mJsonFile, ",\n{\"pid\": 1, \"tid\": %u, \"ph\": \"E\", \"ts\": %llu}", tid, endTime);
}