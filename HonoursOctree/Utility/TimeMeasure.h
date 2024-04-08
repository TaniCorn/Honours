#pragma once
#ifndef TIMEMEASURE_H
#define TIMEMEASURE_H

#include <chrono>
#include <string>
class TimeMeasure
{
public:
	void CaptureStart();
	void CaptureEnd();
	double GetTimeInSeconds();
	double GetTimeInMilliseconds();
	double GetTimeInNanoseconds();

	void SingleOutput(std::string name);

private:
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point endPoint;
};

#endif // !TIMEMEASURE_H
