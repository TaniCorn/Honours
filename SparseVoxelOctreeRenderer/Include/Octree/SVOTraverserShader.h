#pragma once

#ifndef SVOTRAVERSERSHADER_H
#define SVOTRAVERSERSHADER_H

#include "BaseShader.h"
#include "Voxel.h"
#include "vox_file.h"
#include "SVOGPURepresentation.h"

using namespace OctreeGPU;
using namespace std;
using namespace DirectX;

class SVOTraverserShader : public BaseShader
{
public:
	//* ---------- GPU BUFFERS ---------- *//

	struct CameraBuffer {
		XMFLOAT3 Position;
		float Padding;
	};
	struct InvMatrixBuffer {
		XMMATRIX ViewMatrix;
		XMMATRIX ProjectionMatrix;
		XMMATRIX InvViewMatrix;
		XMMATRIX InvProjMatrix;
		XMMATRIX WorldMatrix;
		XMMATRIX InvWorldMatrix;
	};
	struct ViewModeBuffer
	{
		int Mode;
		int Variable;
		int Heatmap;
		int AmountOfOctrees;
	};
	struct VoxelColor {
		UINT32 Rgba[256];
	};
	struct VoxelPaletteBuffer {
		VoxelColor Palettes[8];
	};

	SVOTraverserShader(ID3D11Device* Device, HWND Hwnd, int OctantNumber, int InWidth, int InHeight);
	~SVOTraverserShader();
	void initShader(const wchar_t* CFile, const wchar_t* Blank) override;

	void Unbind(ID3D11DeviceContext* Dc);


	//TODO: remove this if can be replaced by the focused functions below
	void SetShaderParameters(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection, XMFLOAT3 CameraPos, ID3D11ShaderResourceView* Texture, int VoxelView = 0, int ViewDepth = 0, bool Heat = false, int AmountOfOctrees = 2);

	void SetMatrixBuffer(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection);
	void SetCameraBuffer(ID3D11DeviceContext* DeviceContext, const XMFLOAT3& CameraPos, int VoxelView = 0);
	void SetViewModeBuffer(ID3D11DeviceContext* DeviceContext, int VoxelView, int ViewDepth, bool Heatmap, int AmountOfOctrees = 2);
	void SetTexture(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Texture);
	void SetVoxelPalette(ID3D11DeviceContext* DeviceContext, magicavoxel::Palette* Palettes[8]);

	void SetOctreeVoxels(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Octree[8]);
	void SetOctreeVoxels(ID3D11DeviceContext* DeviceContext, SVOGPURepresentation* Octree[8]);
	void SetVoxelModel(ID3D11DeviceContext* DeviceContext, SVOGPURepresentation* Octree, unsigned short StorageIndex);


	ID3D11ShaderResourceView* GetSRV();


	ID3D11Device* Device;
	HWND Hwnd;
	int ScreenWidth, ScreenHeight;
	int NumberOfOctants;

private:
	HRESULT CreateInput();
	HRESULT CreateOutput();
	HRESULT CreateConstantBuffer(ID3D11Buffer** OutBuffer, UINT ByteWidth);

	ID3D11ComputeShader* ComputeShaderBitonic = nullptr;

	ID3D11Buffer* InMatrixBuffer = nullptr;
	ID3D11Buffer* InVoxelPaletteBuffer = nullptr;
	ID3D11Buffer* InCameraBuffer = nullptr;
	ID3D11Buffer* InViewBuffer = nullptr;
	ID3D11Buffer* InOctreeBuffer[8];
	ID3D11ShaderResourceView* InOctreeSRV[8];
	ID3D11ShaderResourceView* InPaletteSRV = nullptr;
	ID3D11Buffer* InOctreeStagingBuffer = nullptr; // For debugging purposes, to read back octree data

	// Texture UAV/SRV bind
	ID3D11Texture2D* Tex;
	ID3D11ShaderResourceView* TexSRV = nullptr; // TODO: Figure out if we needed this bound to SRV
	ID3D11UnorderedAccessView* TexUAV = nullptr;
};

#endif // !SVOTRAVERSERSHADER_H