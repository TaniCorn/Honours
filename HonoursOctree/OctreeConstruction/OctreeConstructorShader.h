#pragma once

#include "BaseShader.h"
#include "../Resources/VoxelOctree.h"
class CPPOctree;
using namespace std;
using namespace DirectX;

class OctreeConstructorShader : public BaseShader
{
public:
	OctreeConstructorShader(ID3D11Device* device, HWND hwnd, int octantNumber, int voxelNumber) : BaseShader(device, hwnd)
	{
		this->device = device;
		this->hwnd = hwnd;
		numberOfOctants = octantNumber;
		numberOfVoxels = voxelNumber;
		initShader(L"OctreeConstructor_cs.cso", NULL);
	};
	ID3D11Device* device;
	HWND hwnd;

	ID3D11ShaderResourceView* getSRV() { return m_outSRV; };
	ID3D11UnorderedAccessView* getUAV() { return m_UAV; };
	void unbind(ID3D11DeviceContext* dc);

	void setShaderParameters(ID3D11DeviceContext* deviceContext);
	void updateCPPOctree(ID3D11DeviceContext* deviceContext, CPPOctree* ot);
	struct VoxelModelDims {
		int DimX;
		int DimY;
		int DimZ;
		int buffer;
	};
	int numberOfOctants = 0;
	int numberOfVoxels = 0;
	void compute(ID3D11DeviceContext* dc, int x, int y, int z);
private:
	void initShader(const wchar_t* cfile, const wchar_t* blank);
	HRESULT createInput();
	HRESULT createOutput();

	ID3D11ComputeShader* g_pComputeShaderBitonic = nullptr;

	ID3D11Buffer* in_voxelBuffer = nullptr;
	ID3D11Buffer* in_dimsBuffer = nullptr;
	ID3D11Buffer* out_voxelOctreeBuffer = nullptr;

	ID3D11Texture2D* m_tex;
	ID3D11ShaderResourceView* m_SRV = nullptr;
	ID3D11UnorderedAccessView* m_UAV = nullptr;

	ID3D11Buffer* out_voxelOctreeStagingBuffer = nullptr;
	ID3D11ShaderResourceView* m_outSRV = nullptr;
};


