#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class QuadTreeShader : public BaseShader
{
private:
	struct QuadBufferType
	{
		float depth;
		float maxdepth;
		XMFLOAT2 padding;
	};

public:
	QuadTreeShader(ID3D11Device* device, HWND hwnd);
	~QuadTreeShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float depth, float maxDepth, float p1 = 1, float p2 = 1);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* quadBuffer;

};

