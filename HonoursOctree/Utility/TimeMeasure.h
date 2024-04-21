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
	void SingleOutputWithRam(std::string name);

private:

	SIZE_T CaptureVRam();
	SIZE_T CaptureRam();
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point endPoint;
	SIZE_T vramStart, vramEnd;
	SIZE_T ramStart, ramEnd;
};

#endif // !TIMEMEASURE_H
