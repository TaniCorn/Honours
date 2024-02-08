#pragma once
/*Tanapat Somrid 03/11/2022
* I've pretty much grabbed the majority of this in https://learn.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-compute-create which linked to an example repo.
* I have repurposed the computeshader example to fit in here instead.
* The original BaseShader implementation was missing compute shader stuff, and rather than try to mess around with that, I've created this standalone computeshader class.

*/
#ifndef _BASECOMPUTESHADER_H
#define _BASECOMPUTESHADER_H

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <fstream>
#include "imGUI/imgui.h"
#include <vector>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

using namespace std;
using namespace DirectX;

const UINT NUM_ELEMENTS = 512 * 512;
const UINT BITONIC_BLOCK_SIZE = 512;
const UINT TRANSPOSE_BLOCK_SIZE = 16;
const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

class BaseComputeShader
{
public:
	BaseComputeShader(ID3D11Device* device, HWND hwnd, int resolution) {
		this->device = device;
		this->hwnd = hwnd;
		data = new float[resolution];
		for (int i = 0; i < resolution; i++)
		{
			data[i] = 0;
		}
		results = new float[resolution];
		this->resolution = resolution;
		CreateComputeShader(L"ComputeTest_cs.hlsl", "main" , &g_pComputeShaderBitonic);
		CreateComputeData();
	};
	ID3D11Device* device;
	HWND hwnd;

	struct CB
	{
		UINT iLevel;
		UINT iLevelMask;
		UINT iWidth;
		UINT iHeight;
	};
	void GPUSort(ID3D11DeviceContext* currentContext);

	void CopyResultsIntoData() {
		for (int i = 0; i < resolution; i++)
		{
			data[i] = results[i];
		}
	}
	float* data;
	float* results;
	int resolution;
	bool fin = false;

	ID3D11ShaderResourceView* GetSRV() {
		return g_pBufferSRV;
	}
private:
	HRESULT FindDXSDKShaderFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);

protected:

	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void CreateComputeShader(const wchar_t* computeShaderNamne, const char* computeShaderEntryPoint, ID3D11ComputeShader** outComputeShader);
	void CreateComputeData();

	HRESULT CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth);
	HRESULT CreateReadBackBuffer(ID3D11Buffer** readBuffer);
	void SetConstants(ID3D11DeviceContext* context, UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight)
	{
		CB cb = { iLevel, iLevelMask, iWidth, iHeight };
		context->UpdateSubresource(g_pCB, 0, nullptr, &cb, 0, 0);
		context->CSSetConstantBuffers(0, 1, &g_pCB);
	}


	//std::vector<UINT> data = std::vector<UINT>(NUM_ELEMENTS);
	//std::vector<UINT> results = std::vector<UINT>(NUM_ELEMENTS);

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;
	ID3D11ComputeShader* g_pComputeShaderTranspose = nullptr;

	ID3D11Buffer* g_pCB = nullptr;

	ID3D11Buffer* g_pBuffer1 = nullptr;
	ID3D11Buffer* g_pBuffer2 = nullptr;
	ID3D11ShaderResourceView* g_pBufferSRV = nullptr;
	ID3D11UnorderedAccessView* g_pBufferUAV = nullptr;
	ID3D11Buffer* g_pReadBackBuffer = nullptr;

};

#endif