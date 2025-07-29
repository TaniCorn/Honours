#pragma once
#ifndef PERFORMANCETRACKER_H
#define PERFORMANCETRACKER_H

#include <Windows.h>
#include <map>
#include <string>
#include "TimeMeasure.h"
struct PerformanceDataEntry
{
	TimeMeasure TimeMeasureData;
	double TimeInMS;
	float VRAMDifference;
	float RAMDifference;
};

class PerformanceTracker
{
public:
	PerformanceDataEntry* GetPerformanceData(const std::string& name);
	void AddPerformanceData(const std::string& name, TimeMeasure CopyOfTimeMeasure);
	void AddContinousPerformanceData(const std::string& name, TimeMeasure CopyOfTimeMeasure);
	std::map<std::string, PerformanceDataEntry>& GetPerformanceDataMap() { return PerformanceData; }

private:
	std::map<std::string, PerformanceDataEntry> PerformanceData;
};

#endif // !PERFORMANCETRACKER_H
