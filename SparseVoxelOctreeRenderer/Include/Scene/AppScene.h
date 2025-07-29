#pragma once

#include "BaseApplication.h"

// Self Made Classes
#include "TextureViewer.h"
#include "VoxelModelManager.h"
#include "SVOManager.h"
#include "SVOTraverserShader.h"


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

	std::unique_ptr<TextureView::TextureViewer> RTViewer; // Renders a texture to the screen

	std::unique_ptr<VoxelModelManager> RawVoxelModels; // Stores the voxel models loaded from disk, unformatted
	std::unique_ptr<SVOManager> SVOModels; // Stores the SVO models created from the voxel models, formatted for GPU usage
	std::unique_ptr<SVOTraverserShader> SVOTraverser; // Traversal shader for the SVOs, renders to a RenderTexture

	bool IsHeatmapEnabled = false;
	int VoxelViewMode = 0; // 0: Normal, 1: Heatmap, 2:Boxes at Depth, 3: Wireframe at Depth, 4: Wireframe above Depth
	int VoxelViewDepth = 0; // Depth for the view mode
	std::string ViewModeDisplay = "Octree Tracer"; // Display string for the current view mode
};

