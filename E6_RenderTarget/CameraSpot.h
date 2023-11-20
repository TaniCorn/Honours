#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class CameraSpot : public BaseShader
{
public:
	CameraSpot(ID3D11Device* device, HWND hwnd);
	~CameraSpot();
	struct CamPosType {
		XMFLOAT4 position;
		XMFLOAT2 screen;
		XMFLOAT2 buffer;
	};

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT3 camPos, XMFLOAT2 screen);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* camBuffer;
	ID3D11SamplerState* sampleState;
};

