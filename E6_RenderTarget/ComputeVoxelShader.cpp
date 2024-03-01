#include "ComputeVoxelShader.h"

HRESULT ComputeVoxelShader::CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth)
{
	HRESULT hr = S_OK;

	// Create the Const Buffer
	D3D11_BUFFER_DESC constant_buffer_desc = {};
	constant_buffer_desc.ByteWidth = byteWidth;
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer(&constant_buffer_desc, NULL, outBuffer);
//#if defined(_DEBUG)
//	(*outBuffer)->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Constant Buffer") - 1, "Constant Buffer");//We can modify the function later to pass in a string to give each constant buffer a name. But for now it's unecessary
//#endif
	return hr;
}

void ComputeVoxelShader::initShader(const wchar_t* cfile, const wchar_t* blank)
{
	loadComputeShader(cfile);
	CreateInput();
	CreateOutput();

}

HRESULT ComputeVoxelShader::CreateInput()
{
https://stackoverflow.com/questions/44377201/directx-write-to-texture-with-compute-shader
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = sWidth;
	textureDesc.Height = sHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	m_tex = 0;
	hr = renderer->CreateTexture2D(&textureDesc, 0, &m_tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = renderer->CreateShaderResourceView(m_tex, &srvDesc, &m_SRV);


	D3D11_BUFFER_DESC constantDataDesc;
	constantDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantDataDesc.ByteWidth = sizeof(VoxelOctree) * NumberOfOctants;
	constantDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constantDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantDataDesc.StructureByteStride = sizeof(VoxelOctree);
	constantDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = device->CreateBuffer(&constantDataDesc, 0, &in_octreeBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC octreedesc;
	octreedesc.Format = DXGI_FORMAT_UNKNOWN;
	octreedesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	octreedesc.BufferEx.FirstElement = 0;
	octreedesc.BufferEx.Flags = 0;
	octreedesc.BufferEx.NumElements = NumberOfOctants;

	hr = device->CreateShaderResourceView(in_octreeBuffer, &octreedesc, &m_OctreeSRV);

	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(VoxelOctree) * NumberOfOctants;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(VoxelOctree);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = (device->CreateBuffer(&outputDesc, 0, &in_octreeStagingBuffer));

	hr = CreateConstantBuffer(&in_matrixBuffer, sizeof(InvMatrixBuffer));
	hr = CreateConstantBuffer(&in_cameraBuffer, sizeof(CameraBuffer));
	hr = CreateConstantBuffer(&in_viewBuffer, sizeof(ViewMode));

	return hr;

}

HRESULT ComputeVoxelShader::CreateOutput()
{
	HRESULT hr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
	ZeroMemory(&descUAV, sizeof(descUAV));
	descUAV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; ;// DXGI_FORMAT_UNKNOWN;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;
	hr = renderer->CreateUnorderedAccessView(m_tex, &descUAV, &m_UAV);


	return hr;
}

void ComputeVoxelShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camerapos, ID3D11ShaderResourceView* texture, int voxelView, int voxelDepth, bool heat)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX invView = XMMatrixTranspose(view);
	XMMATRIX invProjection = XMMatrixInverse(&XMMatrixDeterminant(projection), projection);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
	result = deviceContext->Map(in_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	InvMatrixBuffer* invPtr = (InvMatrixBuffer*)mappedResource.pData;
	invPtr->viewMatrix = view;
	invPtr->projectionMatrix = projection;
	invPtr->invViewMatrix = invView;
	invPtr->invProjMatrix = invProjection;
	invPtr->worldMatrix = world;
	invPtr->invWorldMatrix = invWorld;
	deviceContext->Unmap(in_matrixBuffer, 0);

	result = deviceContext->Map(in_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraBuffer* camPtr = (CameraBuffer*)mappedResource.pData;
	camPtr->padding = voxelView;
	camPtr->position = camerapos;
	deviceContext->Unmap(in_cameraBuffer, 0);

	result = deviceContext->Map(in_viewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ViewMode* viewPtr = (ViewMode*)mappedResource.pData;
	viewPtr->mode = voxelView;
	viewPtr->variable = voxelDepth;
	viewPtr->heatmap = heat;
	viewPtr->pad = 0;
	deviceContext->Unmap(in_viewBuffer, 0);

	deviceContext->CSSetConstantBuffers(0, 1, &in_matrixBuffer);
	deviceContext->CSSetConstantBuffers(1, 1, &in_cameraBuffer);
	deviceContext->CSSetConstantBuffers(2, 1, &in_viewBuffer);

	deviceContext->CSSetShaderResources(0, 1, &texture);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_UAV, 0);
}

void ComputeVoxelShader::setOctreeVoxels(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* octree)
{
	deviceContext->CSSetShaderResources(1, 1, &octree);
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->CopyResource(in_octreeStagingBuffer, in_octreeBuffer);

	HRESULT hr = deviceContext->Map(in_octreeStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		VoxelOctree* dataView = reinterpret_cast<VoxelOctree*>(mappedResource.pData);
		VoxelOctree* o = &dataView[0];
		o = &dataView[1];
		o = &dataView[2];
		o = &dataView[3];
		o = &dataView[4];
		o = &dataView[5];
		deviceContext->Unmap(in_octreeStagingBuffer, 0);
	}
}

void ComputeVoxelShader::unbind(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	dc->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	dc->CSSetShader(nullptr, nullptr, 0);
}