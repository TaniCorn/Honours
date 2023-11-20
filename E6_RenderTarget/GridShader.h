#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class GridShader : public BaseShader
{
public:
	GridShader(ID3D11Device* device, HWND hwnd);
	~GridShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT3 camerapos);
	struct Voxel {
		XMFLOAT4 position;
	};
	struct CameraBuffer {
		XMFLOAT3 position;
		float padding;
	};
	struct InvMatrixBuffer {
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMMATRIX invViewMatrix;
		XMMATRIX invProjMatrix;
		XMMATRIX worldMatrix;
		XMMATRIX invWorldMatrix;


	};
private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* invMatrixBuffer;
	ID3D11Buffer* voxelBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* sampleState;
};

