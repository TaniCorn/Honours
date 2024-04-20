#pragma once

#ifndef OCTREETRACERSHADER_H
#define OCTREETACERSHADER_H

#include "BaseShader.h"
#include "../Resources/VoxelOctree.h"
#include "../MagicalVox/vox_file.h"
#include "../OctreeConstruction/OctreeCPUConstructor.h"


using namespace std;
using namespace DirectX;

class OctreeTracerShader : public BaseShader
{
public:
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
	struct ViewModeBuffer
	{
		int mode;
		int variable;
		int heatmap;
		int amountOfOctrees;
	};
	struct VoxelColor {
		UINT32 rgba[256];
	};
	struct VoxelPaletteBuffer {
		VoxelColor palettes[8];
	};
	OctreeTracerShader(ID3D11Device* device, HWND hwnd, int octantNumber, int inWidth, int inHeight) : BaseShader(device, hwnd)
	{
		this->device = device;
		this->hwnd = hwnd;
		NumberOfOctants = octantNumber;
		screenWidth = inWidth;
		screenHeight = inHeight;
		initShader(L"OctreeTracer_cs.cso", NULL);

	};

	~OctreeTracerShader();
	ID3D11ShaderResourceView* getSRV() { return m_SRV; };
	void unbind(ID3D11DeviceContext* dc);

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camerapos, ID3D11ShaderResourceView* texture, int voxelView = 0, int viewDepth = 0, bool heat = false, int amountOfOctrees = 2);
	void setOctreeVoxels(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* octree[8]);
	void setOctreeVoxels(ID3D11DeviceContext* deviceContext, GPUOctree* octree[8]);
	void setVoxelPalette(ID3D11DeviceContext* deviceContext, magicavoxel::Palette* palettes[8]);
	
	ID3D11Device* device;
	HWND hwnd;
	int screenWidth, screenHeight;
	int NumberOfOctants;

private:
	void initShader(const wchar_t* cfile, const wchar_t* blank);
	HRESULT CreateInput();
	HRESULT CreateOutput();
	HRESULT CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth);

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;

	ID3D11Buffer* in_matrixBuffer = nullptr;
	ID3D11Buffer* in_voxelPaletteBuffer = nullptr;
	ID3D11Buffer* in_cameraBuffer = nullptr;
	ID3D11Buffer* in_viewBuffer = nullptr;
	ID3D11Buffer* in_octreeBuffer[8];
	ID3D11Buffer* in_octreeStagingBuffer = nullptr;

	ID3D11Texture2D* m_tex;
	ID3D11ShaderResourceView* m_OctreeSRV[8];
	ID3D11ShaderResourceView* m_SRV = nullptr;
	ID3D11ShaderResourceView* m_paletteSRV = nullptr;
	ID3D11UnorderedAccessView* m_UAV = nullptr;
};

#endif // !OCTREETRACERSHADER_H
