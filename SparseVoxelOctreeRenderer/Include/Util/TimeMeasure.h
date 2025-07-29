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

	SIZE_T GetVRamStart() const { return VramStart; }
	SIZE_T GetVRamEnd() const { return VramEnd; }
	SIZE_T GetRamStart() const { return RamStart; }
	SIZE_T GetRamEnd() const { return RamEnd; }
	float GetVRamChange() const { return static_cast<float>(VramEnd) - static_cast<float>(VramStart); }
	float GetRamChange() const { return static_cast<float>(RamEnd) - static_cast<float>(RamStart); }

	void SingleOutput(std::string name);
	void SingleAverageOutput(std::string name);
	void SingleOutputWithRam(std::string name);

	
	std::chrono::steady_clock::time_point StartPoint, EndPoint;
	SIZE_T VramStart, VramEnd; // Note Vram here denotes Virtual RAM, not GPU VRAM
	SIZE_T RamStart, RamEnd; // This is the physical RAM usage

	long long AverageDiff;
	int Amount;
	bool IsUniqueContinuousCapture = false;

private:

	SIZE_T CaptureVRam();
	SIZE_T CaptureRam();
};

#endif // !TIMEMEASURE_H
