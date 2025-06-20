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
	void Reset();
	void CaptureStart();
	void CaptureEnd();
	void ContinuousCaptureStart();
	void ContinuousCaptureEnd();

	double GetTimeInSeconds();
	double GetTimeInMilliseconds();
	double GetTimeInNanoseconds();
	double GetAverageTimeInMilliseconds();

	void SingleOutput(std::string name);
	void SingleAverageOutput(std::string name);
	void SingleOutputWithRam(std::string name);




private:

	SIZE_T CaptureVRam();
	SIZE_T CaptureRam();
	std::chrono::steady_clock::time_point StartPoint, EndPoint;
	SIZE_T VramStart, VramEnd;
	SIZE_T RamStart, RamEnd;

	long long AverageDiff;
	int Amount;
};

#endif // !TIMEMEASURE_H
