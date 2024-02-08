#pragma once


#include "DXF.h"

using namespace std;
using namespace DirectX;


class BaseComputeTextureShader : public BaseShader
{
public:
	BaseComputeTextureShader(ID3D11Device* device, HWND hwnd, int inWidth, int inHeight) : BaseShader(device, hwnd)
	{
		this->device = device;
		this->hwnd = hwnd;
		sWidth = inWidth;
		sHeight = inHeight;
		initShader(L"ComputeTexture_cs.cso", NULL);
	};
	ID3D11Device* device;
	HWND hwnd;
	int sWidth, sHeight;

	void setShaderParameters(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* texture1);
	ID3D11ShaderResourceView* getSRV() { return g_pBufferSRV; };
	void unbind(ID3D11DeviceContext* dc);
private:
	void initShader(const wchar_t* cfile, const wchar_t* blank);
	void CreateComputeData();

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;

	ID3D11Texture2D* m_tex;
	ID3D11ShaderResourceView* g_pBufferSRV = nullptr;
	ID3D11UnorderedAccessView* g_pBufferUAV = nullptr;

};

