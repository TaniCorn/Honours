#pragma once
#ifndef TIMEMEASURE_H
#define TIMEMEASURE_H

#include <chrono>
#include <string>
#include <Windows.h>
#include <Psapi.h>

class TimeMeasure
{
public:
	void CaptureStart();
	void CaptureEnd();
	double GetTimeInSeconds();
	double GetTimeInMilliseconds();
	double GetTimeInNanoseconds();

	void SingleOutput(std::string name);
	void SingleAverageOutput(std::string name);
	void SingleOutputWithRam(std::string name);

	void ContinuousCaptureStart();
	void ContinuousCaptureEnd();

private:

	SIZE_T CaptureVRam();
	SIZE_T CaptureRam();
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point endPoint;
	SIZE_T vramStart, vramEnd;
	SIZE_T ramStart, ramEnd;

	long long averageDiff;
	int amount;
};

#endif // !TIMEMEASURE_H
