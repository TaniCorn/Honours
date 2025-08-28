#pragma once

#include "BaseApplication.h"

// Self Made Classes
#include "TextureViewer.h"
#include "VoxelModelManager.h"
#include "SVOManager.h"
#include "SVOTraverserShader.h"

// Benchmarking
#include "ApplicationDefinitions.h"
#include "TimeMeasure.h"
#include "PerformanceTracker.h"
#include "ConstructionPerformance.h"


class AppScene : public BaseApplication
{
public:
	AppScene();
	~AppScene();

	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN) override;
	bool frame() override;
protected:
	bool render() override;
private:
	void renderGUI();

	void CameraControlsGUI();
	void TracerControlsGUI();
	void PerfTrackerGUI();

	std::unique_ptr<TextureView::TextureViewer> RTViewer; // Renders a texture to the screen

	std::unique_ptr<VoxelModelManager> RawVoxelModels; // Stores the voxel models loaded from disk, unformatted
	std::unique_ptr<SVOManager> SVOModels; // Stores the SVO models created from the voxel models, formatted for GPU usage
	std::unique_ptr<SVOTraverserShader> SVOTraverser; // Traversal shader for the SVOs, renders to a RenderTexture

	ConstructionPerformance ConstructionPerf;
	TimeMeasure TimeAndMemoryTracker;
	bool modelSelection[MODELAMOUNT] = { false, false, false, false, false, false, false, false };
	TimeMeasure ReconstructionTracker;
	float CaptureTime = 1.0f;
	PerformanceTracker PerfTrack;
	bool ShouldConstructSVOContinously = false;
	// Tracer variables
	bool IsHeatmapEnabled = false;
	int VoxelViewMode = 0; // 0: Normal, 1: Heatmap, 2:Boxes at Depth, 3: Wireframe at Depth, 4: Wireframe above Depth
	int VoxelViewDepth = 0; // Depth for the view mode
	int ViewIterations = 150;
	std::string ViewModeDisplay = "Octree Tracer"; // Display string for the current view mode
};

