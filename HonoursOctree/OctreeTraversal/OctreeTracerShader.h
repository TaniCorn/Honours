#pragma once

#include "BaseShader.h"
#include "../VoxelOctree.h"

using namespace std;
using namespace DirectX;

class OctreeTracerShader : public BaseShader
{
public:
	OctreeTracerShader(ID3D11Device* device, HWND hwnd, int octantNumber, int inWidth, int inHeight) : BaseShader(device, hwnd)
	{
		this->device = device;
		this->hwnd = hwnd;
		NumberOfOctants = octantNumber;
		screenWidth = inWidth;
		screenHeight = inHeight;
		initShader(L"OctreeTracer_cs.cso", NULL);
	};
	ID3D11Device* device;
	HWND hwnd;
	int screenWidth, screenHeight;
	int NumberOfOctants;
	ID3D11ShaderResourceView* getSRV() { return m_SRV; };
	void unbind(ID3D11DeviceContext* dc);

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camerapos, ID3D11ShaderResourceView* texture, int voxelView = 0, int viewDepth = 0, bool heat = false);
	void setOctreeVoxels(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* octree);
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
	struct ViewMode
	{
		int mode;
		int variable;
		int heatmap;
		float pad;
	};
private:
	void initShader(const wchar_t* cfile, const wchar_t* blank);
	HRESULT CreateInput();
	HRESULT CreateOutput();
	HRESULT CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth);

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;

	ID3D11Buffer* in_matrixBuffer = nullptr;
	ID3D11Buffer* in_cameraBuffer = nullptr;
	ID3D11Buffer* in_viewBuffer = nullptr;
	ID3D11Buffer* in_octreeBuffer = nullptr;
	ID3D11Buffer* in_octreeStagingBuffer = nullptr;

	ID3D11Texture2D* m_tex;
	ID3D11ShaderResourceView* m_OctreeSRV = nullptr;
	ID3D11ShaderResourceView* m_SRV = nullptr;
	ID3D11UnorderedAccessView* m_UAV = nullptr;
};

