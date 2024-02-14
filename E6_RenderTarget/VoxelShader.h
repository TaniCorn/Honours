#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class VoxelShader : public BaseShader
{
public:

	VoxelShader(ID3D11Device* device, HWND hwnd);
	~VoxelShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, ID3D11ShaderResourceView* texture);
private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

