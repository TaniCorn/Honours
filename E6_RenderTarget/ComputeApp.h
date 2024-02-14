#pragma once
#pragma once
// Application.h
#ifndef _COMPUTEAPP_H
#define _COMPUTEAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "ComputePipelineShader.h"
#include "BaseComputeTextureShader.h"
class ComputeApp : public BaseApplication
{
public:

	ComputeApp();
	~ComputeApp();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	float screenw, screenh;
	//BaseComputeShader* ComputeShader;
	OrthoMesh* orthoMesh;
	ComputePipelineShader* textureShader;
	RenderTexture* renderTexture;
	BaseComputeTextureShader* computeTextureShader;
	RenderTexture* computeRT;
	ID3D11ShaderResourceView* SRV;
};

#endif