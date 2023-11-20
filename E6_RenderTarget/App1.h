// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TextureShader.h"
#include "GreyShader.h"
#include "CameraSpot.h"
class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void firstPass();
	void secondPass();
	void greyPass();
	void texturepass();

	void firstPlayerPass();
	void secondPlayerPass();
	void gui();

private:
	float screenw, screenh;
	CubeMesh* cubeMesh;
	SphereMesh* sphereMesh;
	PlaneMesh* planeMesh;

	OrthoMesh* orthoMesh;
	OrthoMesh* secondOrthoMesh;
	OrthoMesh* greyOrthoMesh;

	LightShader* lightShader;
	TextureShader* textureShader;
	GreyShader* greyShader;
	CameraSpot* spotShader;

	FPCamera* secondCamera;

	RenderTexture* renderTexture;
	RenderTexture* secondRenderTexture;
	RenderTexture* greyRenderTexture;


	OrthoMesh* firstPlayerOrthoMesh;
	RenderTexture* firstPlayerRenderTexture;
	RenderTexture* secondPlayerRenderTexture;
	Light* light;
};

#endif