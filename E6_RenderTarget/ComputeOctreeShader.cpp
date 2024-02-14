#include "ComputeOctreeShader.h"
#include "quad/Octree.h"
ComputeOctreeShader::VoxelOctree* ComputeOctreeShader::GetVoxelOctreeOutput(ID3D11DeviceContext* deviceContext)
{
	// Copy result
	deviceContext->CopyResource(out_voxelOctreeStagingBuffer, out_voxelOctreeBuffer);

	// Update particle system data with output from Compute Shader
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(out_voxelOctreeStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		VoxelOctree* dataView = reinterpret_cast<VoxelOctree*>(mappedResource.pData);
		VoxelOctree* h1 = &dataView[0];
		VoxelOctree* h2 = &dataView[1];
		VoxelOctree* h3 = &dataView[2];
		VoxelOctree* h4 = &dataView[3];
		VoxelOctree* h5 = &dataView[4];
		VoxelOctree* h6 = &dataView[5];
		deviceContext->Unmap(out_voxelOctreeStagingBuffer, 0);
		return dataView;
	}
	return nullptr;
}

void ComputeOctreeShader::initShader(const wchar_t* cfile, const wchar_t* blank)
{
	loadComputeShader(cfile);
	CreateInput();
	CreateOutput();
}

HRESULT ComputeOctreeShader::CreateInput()
{
//https://stackoverflow.com/questions/32049639/directx-11-compute-shader-writing-to-an-output-resource
	HRESULT hr;
	D3D11_BUFFER_DESC constantDataDesc;
	constantDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantDataDesc.ByteWidth = sizeof(VoxelOctree) * NumberOfOctants;
	constantDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constantDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantDataDesc.StructureByteStride = sizeof(VoxelOctree);
	constantDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	//hr = device->CreateBuffer(&constantDataDesc, 0, &in_voxelOctreeBuffer);

	constantDataDesc.ByteWidth = sizeof(GPUVoxel) * NumberOfVoxels;
	constantDataDesc.StructureByteStride = sizeof(GPUVoxel);
	hr = device->CreateBuffer(&constantDataDesc, 0, &in_voxelBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = NumberOfVoxels;

	hr = device->CreateShaderResourceView(in_voxelBuffer, &srvDesc, &m_SRV);
	return hr;
}

HRESULT ComputeOctreeShader::CreateOutput()
{
	//https://stackoverflow.com/questions/32049639/directx-11-compute-shader-writing-to-an-output-resource
	HRESULT hr;

	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(VoxelOctree) * NumberOfOctants;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(VoxelOctree);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = (device->CreateBuffer(&outputDesc, 0, &out_voxelOctreeBuffer));

	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = (device->CreateBuffer(&outputDesc, 0, &out_voxelOctreeStagingBuffer));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = NumberOfOctants;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	hr = device->CreateUnorderedAccessView(out_voxelOctreeBuffer, &uavDesc, &m_UAV);
	return hr;
}

void ComputeOctreeShader::setShaderParameters(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetShaderResources(0, 1, &m_SRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_UAV, 0);
}

void ComputeOctreeShader::updateCPPOctree(ID3D11DeviceContext* deviceContext, Octree* ot)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	result = deviceContext->Map(in_voxelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	GPUVoxel* invPtr = (GPUVoxel*)mappedResource.pData;
	int i = 0;
	/*for (auto vox : ot->points)
	{
		invPtr[i].color = vox->color;
		invPtr[i].point = XMFLOAT3(vox->point.GetX(), vox->point.GetY(), vox->point.GetZ());
		i++;
	}*/
	invPtr[0].color = UINT32(224);
	invPtr[1].color = UINT32(225);
	invPtr[0].point = XMFLOAT3(0.1, 0.1, 0.1);
	invPtr[1].point = XMFLOAT3(0.9, 0.9, 0.9);
	deviceContext->Unmap(in_voxelBuffer, 0);
}

void ComputeOctreeShader::unbind(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	dc->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	dc->CSSetShader(nullptr, nullptr, 0);
}