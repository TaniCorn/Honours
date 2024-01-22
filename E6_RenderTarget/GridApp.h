// Application.h
#ifndef _GRIDAPP_H
#define _GRIDAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "GridShader.h"
#include "GreyShader.h"
#include "CameraSpot.h"
class GridApp : public BaseApplication
{
public:

	GridApp();
	~GridApp();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void texturepass();

	void gui();

private:
	float screenw, screenh;
	OrthoMesh* orthoMesh;
	GridShader* textureShader;
	RenderTexture* renderTexture;
	OrthoMesh* firstPlayerOrthoMesh;
};

#endif