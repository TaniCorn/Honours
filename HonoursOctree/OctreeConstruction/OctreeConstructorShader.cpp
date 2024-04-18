#include "OctreeConstructorShader.h"
#include "Octree.h"
void OctreeConstructorShader::compute(ID3D11DeviceContext* dc, int x, int y, int z)
{
	dc->CSSetShader(computeShader, NULL, 0);
	dc->Dispatch(x, y, z);
	dc->CopyResource(out_voxelOctreeStagingBuffer, out_voxelOctreeBuffer);

}

void OctreeConstructorShader::initShader(const wchar_t* cfile, const wchar_t* blank)
{
	loadComputeShader(cfile);
	createInput();
	createOutput();
}

HRESULT OctreeConstructorShader::createInput()
{
//https://stackoverflow.com/questions/32049639/directx-11-compute-shader-writing-to-an-output-resource
	HRESULT hr;
	D3D11_BUFFER_DESC constant_buffer_desc = {};
	constant_buffer_desc.ByteWidth = sizeof(VoxelModelDims);
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer(&constant_buffer_desc, 0, &in_dimsBuffer);
	if (hr != S_OK)
	{
		return  hr;
	}

	D3D11_BUFFER_DESC constantDataDesc;
	constantDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constantDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	constantDataDesc.ByteWidth = sizeof(Voxel) * numberOfVoxels;
	constantDataDesc.StructureByteStride = sizeof(Voxel);
	hr = device->CreateBuffer(&constantDataDesc, 0, &in_voxelBuffer);
	if (hr != S_OK)
	{
		return  hr;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = numberOfVoxels;
	hr = device->CreateShaderResourceView(in_voxelBuffer, &srvDesc, &m_SRV);

	return hr;
}

HRESULT OctreeConstructorShader::createOutput()
{
	//https://stackoverflow.com/questions/32049639/directx-11-compute-shader-writing-to-an-output-resource
	HRESULT hr;

	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(VoxelOctree) * numberOfOctants;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(VoxelOctree);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = (device->CreateBuffer(&outputDesc, 0, &out_voxelOctreeBuffer));
	if (hr != S_OK)
	{
		return  hr;
	}
	
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	hr = (device->CreateBuffer(&outputDesc, 0, &out_voxelOctreeStagingBuffer));
	if (hr != S_OK)
	{
		return  hr;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = numberOfOctants;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = device->CreateUnorderedAccessView(out_voxelOctreeBuffer, &uavDesc, &m_UAV);
	if (hr != S_OK)
	{
		return  hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = numberOfOctants;
	hr = device->CreateShaderResourceView(out_voxelOctreeStagingBuffer, &srvDesc, &m_outSRV);

	return hr;
}



void OctreeConstructorShader::setShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_SRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_UAV, 0);
	deviceContext->CSSetConstantBuffers(0, 1, &in_dimsBuffer);
}

void OctreeConstructorShader::updateCPPOctree(ID3D11DeviceContext* deviceContext, CPPOctree* oc)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	result = deviceContext->Map(in_voxelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	Voxel* invPtr = (Voxel*)mappedResource.pData;
	int i = 0;
	for (auto vox : oc->voxels)
	{
		invPtr[i].color = vox->color;
		invPtr[i].point = XMFLOAT3(vox->point.x, vox->point.y, vox->point.z);
		i++;
	}
	deviceContext->Unmap(in_voxelBuffer, 0);

	result = deviceContext->Map(in_dimsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VoxelModelDims* dimsPtr = (VoxelModelDims*)mappedResource.pData;
	dimsPtr->DimX = oc->octree->bottomRightBackPoint.x;
	dimsPtr->DimY = oc->octree->topLeftFrontPoint.y;
	dimsPtr->DimZ = oc->octree->bottomRightBackPoint.z;
	deviceContext->Unmap(in_dimsBuffer, 0);
}

OctreeConstructorShader::~OctreeConstructorShader()
{

	if (g_pComputeShaderBitonic)
	{
		g_pComputeShaderBitonic->Release();
		g_pComputeShaderBitonic = 0;
	}
	if (m_tex)
	{
		m_tex->Release();
		m_tex = 0;
	}
	if (m_SRV)
	{
		m_SRV->Release();
		m_SRV = 0;
	}
	if (m_UAV)
	{
		m_UAV->Release();
		m_UAV = 0;
	}	
	if (m_outSRV)
	{
		m_outSRV->Release();
		m_outSRV = 0;
	}	
	if (out_voxelOctreeStagingBuffer)
	{
		out_voxelOctreeStagingBuffer->Release();
		out_voxelOctreeStagingBuffer = 0;
	}
	if (out_voxelOctreeBuffer)
	{
		out_voxelOctreeBuffer->Release();
		out_voxelOctreeBuffer = 0;
	}	
	if (in_dimsBuffer)
	{
		in_dimsBuffer->Release();
		in_dimsBuffer = 0;
	}	
	if (in_voxelBuffer)
	{
		in_voxelBuffer->Release();
		in_voxelBuffer = 0;
	}
}

void OctreeConstructorShader::unbind(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	dc->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	dc->CSSetShader(nullptr, nullptr, 0);
}