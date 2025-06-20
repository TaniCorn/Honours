#pragma once

#include "BaseApplication.h"

// Self Made Classes
#include "VoxelModelManager.h"
#include "TextureViewer.h"

class AppScene : public BaseApplication
{
public:
	AppScene() {};
	~AppScene() {};

	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN) override;
	bool frame() override;
protected:
	void handleInput(float dt) override;
	bool render() override;
private:
	void renderGUI();

	float CameraSpeed = 1.0f;
	std::unique_ptr<TextureView::TextureViewer> TextureViewer;
};

