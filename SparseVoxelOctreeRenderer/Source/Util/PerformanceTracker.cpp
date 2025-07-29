#include "PerformanceTracker.h"

PerformanceDataEntry* PerformanceTracker::GetPerformanceData(const std::string& name)
{
	if (PerformanceData.find(name) != PerformanceData.end())
	{
		return &PerformanceData[name];
	}
	else
	{
		return nullptr;
	}
}

void PerformanceTracker::AddPerformanceData(const std::string& name, TimeMeasure CopyOfTimeMeasure)
{
	PerformanceDataEntry Entry;
	Entry.TimeMeasureData = CopyOfTimeMeasure;
	Entry.TimeInMS = CopyOfTimeMeasure.GetTimeInMilliseconds();
	Entry.RAMDifference = (CopyOfTimeMeasure.GetRamChange() / 1024.0f) / 1024.0f; // Translate to MB
	Entry.VRAMDifference = (CopyOfTimeMeasure.GetVRamChange() / 1024.0f) / 1024.0f; // Translate to MB
	PerformanceData[name] = Entry;
}
void PerformanceTracker::AddContinousPerformanceData(const std::string& name, TimeMeasure CopyOfTimeMeasure)
{
	PerformanceDataEntry Entry;
	Entry.TimeMeasureData = CopyOfTimeMeasure;
	Entry.TimeInMS = CopyOfTimeMeasure.GetAverageTimeInMilliseconds();
	Entry.RAMDifference = (static_cast<float>(CopyOfTimeMeasure.GetRamChange()) / 1024.0f) / 1024.0f; // Translate to MB
	Entry.VRAMDifference = (static_cast<float>(CopyOfTimeMeasure.GetVRamChange()) / 1024.0f) / 1024.0f; // Translate to MB
	PerformanceData[name] = Entry;
}