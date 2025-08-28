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
	enum ModelAmountsSize
	{
		MODELAMOUNTS = 8 // const enum hack
	};
	struct VoxelPaletteBuffer {
		VoxelColor Palettes[MODELAMOUNTS];
	};

	struct ApplicationBuffer
	{
		int ApplicationWidth;
		int ApplicationHeight;
		int Buf3;
		int Buf4;
	};
public:
	SVOTraverserShader(ID3D11Device* Device, HWND Hwnd, int OctantNumber, int InWidth, int InHeight);
	~SVOTraverserShader();
	void initShader(const wchar_t* CFile, const wchar_t* Blank) override;



	/// <summary>
	/// This is just a conglomeration of the SetBuffer functions and the SetTexture function
	/// </summary>
	void SetShaderParameters(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection, XMFLOAT3 CameraPos, ID3D11ShaderResourceView* Texture, int VoxelView = 0, int ViewDepth = 0, bool Heat = false, int AmountOfOctrees = 2);

	void SetMatrixBuffer(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection);
	void SetCameraBuffer(ID3D11DeviceContext* DeviceContext, const XMFLOAT3& CameraPos, int VoxelView = 0);
	void SetViewModeBuffer(ID3D11DeviceContext* DeviceContext, int VoxelView, int ViewDepth, bool Heatmap, int AmountOfOctrees = 2);
	void SetTexture(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Texture);
	void SetApplicationBuffer(ID3D11DeviceContext* DeviceContext, int Width, int Height);

	/// <summary>
	/// Set the octrees from shader resource views. These should've been generated from the GPUConstruction
	/// </summary>
	void SetOctreeVoxels(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Octree[MODELAMOUNTS]);

	/// <summary>
	/// Set the Voxel model and palette for the traverser shader. Index between 0 and 7.
	/// </summary>
	/// <param name="StorageIndex"> 0 to 7 </param>
	void SetVoxelModelAndPalette(ID3D11DeviceContext* DeviceContext, const SVOGPURepresentation* const Octree, const magicavoxel::Palette& ColorPalette, unsigned short StorageIndex);
	void SetVoxelModel(ID3D11DeviceContext* DeviceContext, const SVOGPURepresentation* const Octree, unsigned short StorageIndex);
	void SetVoxelPalette(ID3D11DeviceContext* DeviceContext, const magicavoxel::Palette& ColorPalette, int StorageIndex);

	ID3D11ShaderResourceView* GetSRV();
	void Unbind(ID3D11DeviceContext* Dc);

	/// <summary>
	/// Doesn't currently work properly. For now just recreate the entire shader
	/// </summary>
	void Resize(ID3D11Device* Device, HWND Hwnd, int NewWidth, int NewHeight);

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
	ID3D11Buffer* InCameraBuffer = nullptr;
	ID3D11Buffer* InViewBuffer = nullptr;
	ID3D11Buffer* InApplicationBuffer = nullptr;
	ID3D11Buffer* InVoxelPaletteBuffer[MODELAMOUNTS];
	ID3D11Buffer* InOctreeBuffer[MODELAMOUNTS];
	ID3D11ShaderResourceView* InOctreeSRV[MODELAMOUNTS];
	ID3D11ShaderResourceView* InPaletteSRV[MODELAMOUNTS]; // TODO: Seems redundant if we can figure out how to use the constant buffer correctly
	ID3D11Buffer* InOctreeStagingBuffer = nullptr; // For debugging purposes, to read back octree data

	// Texture UAV/SRV bind
	ID3D11Texture2D* Tex;
	ID3D11ShaderResourceView* TexSRV = nullptr; // TODO: Figure out if we needed this bound to SRV
	ID3D11UnorderedAccessView* TexUAV = nullptr;
};

#undef MODELAMOUNTS
#endif // !SVOTRAVERSERSHADER_H