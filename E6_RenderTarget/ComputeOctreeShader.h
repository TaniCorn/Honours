#pragma once

#include "BaseShader.h"

class Octree;
using namespace std;
using namespace DirectX;

class ComputeOctreeShader : public BaseShader
{
public:
	ComputeOctreeShader(ID3D11Device* device, HWND hwnd, int octantNumber, int voxelNumber) : BaseShader(device, hwnd)
	{
		this->device = device;
		this->hwnd = hwnd;
		NumberOfOctants = octantNumber;
		NumberOfVoxels = voxelNumber;
		initShader(L"ComputeOctree_cs.cso", NULL);
	};
	ID3D11Device* device;
	HWND hwnd;

	ID3D11ShaderResourceView* getSRV() { return m_outSRV; };
	ID3D11UnorderedAccessView* getUAV() { return m_UAV; };
	ID3D11Buffer* getOctreeBuffer() { return out_voxelOctreeBuffer; }
	void unbind(ID3D11DeviceContext* dc);

	void setShaderParameters(ID3D11DeviceContext* deviceContext);
	void updateCPPOctree(ID3D11DeviceContext* deviceContext, Octree* ot);

	//struct VoxelOctree {
	//	XMFLOAT4 position;
	//};
	struct VoxelOctree
	{
		XMFLOAT3 TopLeftFrontPosition; // 4bytes*3 = 12bytes
		XMFLOAT3 BottomRightBackPosition; //12 bytes
		XMFLOAT3 VoxelPosition;
		UINT32 RGB; //4bytes
		UINT32 Depth;
		UINT32 Octants[8];
		//4bytes * 9 = 36bytes
		//36 + 12 + 12 = 60bytes per octree
	};
	struct GPUVoxel 
	{
		XMFLOAT3 point;
		UINT32 color;
	};
	struct VoxelModelDims {
		int DimX;
		int DimY;
		int DimZ;
		int buffer;
	};
	int NumberOfOctants = 0;
	int NumberOfVoxels = 0;
	VoxelOctree* GetVoxelOctreeOutput(ID3D11DeviceContext* deviceContext);
private:
	void initShader(const wchar_t* cfile, const wchar_t* blank);
	HRESULT CreateInput();
	HRESULT CreateOutput();

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;

	ID3D11Buffer* in_voxelBuffer = nullptr;
	ID3D11Buffer* in_dimsBuffer = nullptr;
	ID3D11Buffer* out_voxelOctreeBuffer = nullptr;
	ID3D11Buffer* out_voxelOctreeStagingBuffer = nullptr;

	ID3D11Texture2D* m_tex;
	ID3D11ShaderResourceView* m_SRV = nullptr;
	ID3D11ShaderResourceView* m_outSRV = nullptr;
	ID3D11UnorderedAccessView* m_UAV = nullptr;
};


