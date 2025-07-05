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
	std::unique_ptr<TextureView::TextureViewer> RTViewer;

	std::unique_ptr<VoxelModelManager> RawVoxelModels;
	std::unique_ptr<SVOManager> SVOModels;
	std::unique_ptr<SVOTraverserShader> SVOTraverser;
	magicavoxel::Palette* voxelModelPalettes[MODELAMOUNT];
};

