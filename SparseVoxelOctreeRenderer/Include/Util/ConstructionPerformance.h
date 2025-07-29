#pragma once
#ifndef CONSTRUCTIONPERF_H
#define CONSTRUCTIONPERF_H

#include "ApplicationDefinitions.h"
#include "TimeMeasure.h"

#include <vector>
#include <string>

class SVOManager;
class VoxelModelManager;
class Timer;
class PerformanceTracker;

class ConstructionPerformance
{
public:

	void SetModelNames(const std::vector<std::string>& Names)
	{
		ModelNames = Names;
	}
	void ContinouslyConstruct(SVOManager& SVOModels, VoxelModelManager& RawVoxelModels, Timer& timer, PerformanceTracker& PerfTrack);
	void GUIRender(PerformanceTracker& PerfTrack);

private:
	TimeMeasure ReconstructionTracker;

	std::vector<std::string> ModelNames;
	bool ModelSelection[MODELAMOUNT] = { };// Initialises all to false
	bool ShouldConstructSVOContinously = false;
	float CaptureTime = 1.0f;
};

#endif // !CONSTRUCTIONPERF_H
