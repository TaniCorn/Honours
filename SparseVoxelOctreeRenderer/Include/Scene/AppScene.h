#pragma once

#include "BaseApplication.h"

// Self Made Classes
#include "TextureViewer.h"
#include "VoxelModelManager.h"
#include "SVOManager.h"
#include "SVOTraverserShader.h"

#define MODELAMOUNT 8

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

	float CameraSpeed = 1.0f;
	std::unique_ptr<TextureView::TextureViewer> RTViewer; // Renders a texture to the screen

	std::unique_ptr<VoxelModelManager> RawVoxelModels; // Stores the voxel models loaded from disk, unformatted
	std::unique_ptr<SVOManager> SVOModels; // Stores the SVO models created from the voxel models, formatted for GPU usage
	std::unique_ptr<SVOTraverserShader> SVOTraverser; // Traversal shader for the SVOs, renders to a RenderTexture
};

