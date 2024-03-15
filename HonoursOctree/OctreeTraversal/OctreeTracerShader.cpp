#include "OctreeTracerShader.h"

HRESULT OctreeTracerShader::CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth)
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

void OctreeTracerShader::initShader(const wchar_t* cfile, const wchar_t* blank)
{
	loadComputeShader(cfile);
	CreateInput();
	CreateOutput();

}

HRESULT OctreeTracerShader::CreateInput()
{
https://stackoverflow.com/questions/44377201/directx-write-to-texture-with-compute-shader
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = screenWidth;
	textureDesc.Height = screenHeight;
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
	hr = CreateConstantBuffer(&in_viewBuffer, sizeof(ViewModeBuffer));
	hr = CreateConstantBuffer(&in_voxelPaletteBuffer, sizeof(VoxelPaletteBuffer));

	return hr;

}

HRESULT OctreeTracerShader::CreateOutput()
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

void OctreeTracerShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camerapos, ID3D11ShaderResourceView* texture, int voxelView, int voxelDepth, bool heat, int amountOfOctrees)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	DirectX::XMVECTOR f = DirectX::XMMatrixDeterminant(projection);
	DirectX::XMVECTOR f2 = DirectX::XMMatrixDeterminant(world);


	XMMATRIX invView = DirectX::XMMatrixTranspose(view);
	XMMATRIX invProjection = DirectX::XMMatrixInverse(&f, projection);
	XMMATRIX invWorld = DirectX::XMMatrixInverse(&f2, world);
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
	ViewModeBuffer* viewPtr = (ViewModeBuffer*)mappedResource.pData;
	viewPtr->mode = voxelView;
	viewPtr->variable = voxelDepth;
	viewPtr->heatmap = heat;
	viewPtr->amountOfOctrees = amountOfOctrees;
	deviceContext->Unmap(in_viewBuffer, 0);

	deviceContext->CSSetConstantBuffers(0, 1, &in_matrixBuffer);
	deviceContext->CSSetConstantBuffers(1, 1, &in_cameraBuffer);
	deviceContext->CSSetConstantBuffers(2, 1, &in_viewBuffer);

	deviceContext->CSSetShaderResources(0, 1, &texture);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_UAV, 0);
}

void OctreeTracerShader::setOctreeVoxels(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* octree[8])
{
	deviceContext->CSSetShaderResources(1, 8, octree);
}

void OctreeTracerShader::setVoxelPalette(ID3D11DeviceContext* deviceContext, magicavoxel::Palette* palettes[8])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(in_voxelPaletteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VoxelPaletteBuffer* col = (VoxelPaletteBuffer*)mappedResource.pData;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			col->palettes[i].r[j] = palettes[i]->at(j).r;
			col->palettes[i].g[j] = palettes[i]->at(j).g;
			col->palettes[i].b[j] = palettes[i]->at(j).b;
			col->palettes[i].a[j] = palettes[i]->at(j).a;
		}
	}
	deviceContext->Unmap(in_voxelPaletteBuffer, 0);
	deviceContext->CSSetConstantBuffers(3, 1, &in_voxelPaletteBuffer);

}

OctreeTracerShader::~OctreeTracerShader()
{

	if (g_pComputeShaderBitonic)
	{
		g_pComputeShaderBitonic->Release();
		g_pComputeShaderBitonic = 0;
	}
	if (in_matrixBuffer)
	{
		in_matrixBuffer->Release();
		in_matrixBuffer = 0;
	}
	if (in_cameraBuffer)
	{
		in_cameraBuffer->Release();
		in_cameraBuffer = 0;
	}
	if (in_viewBuffer)
	{
		in_viewBuffer->Release();
		in_viewBuffer = 0;
	}	
	if (in_octreeBuffer)
	{
		in_octreeBuffer->Release();
		in_octreeBuffer = 0;
	}
	if (in_octreeStagingBuffer)
	{
		in_octreeStagingBuffer->Release();
		in_octreeStagingBuffer = 0;
	}
	if (m_tex)
	{
		m_tex->Release();
		m_tex = 0;
	}
	if (m_OctreeSRV)
	{
		m_OctreeSRV->Release();
		m_OctreeSRV = 0;
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
}

void OctreeTracerShader::unbind(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	dc->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	dc->CSSetShader(nullptr, nullptr, 0);
}