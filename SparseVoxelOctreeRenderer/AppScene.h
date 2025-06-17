#pragma once

#include "BaseApplication.h"
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
};

