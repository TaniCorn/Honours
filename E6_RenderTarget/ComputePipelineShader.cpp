#include "ComputePipelineShader.h"



ComputePipelineShader::ComputePipelineShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"ComputePipeline_vs.cso", L"ComputePipeline_ps.cso");
}


ComputePipelineShader::~ComputePipelineShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ComputePipelineShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	matrixBufferDesc.ByteWidth = sizeof(Voxel);
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &voxelBuffer);
	matrixBufferDesc.ByteWidth = sizeof(CameraBuffer);
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &cameraBuffer);
	matrixBufferDesc.ByteWidth = sizeof(InvMatrixBuffer);
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &invMatrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

}


void ComputePipelineShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT3 camerapos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(orthoView);
	tproj = XMMatrixTranspose(ortho);

	// Sned matrix data
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(voxelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	Voxel* voxPtr = (Voxel*)mappedResource.pData;
	voxPtr->position = XMFLOAT4(0, 0, 5, 0);
	deviceContext->Unmap(voxelBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &voxelBuffer);


	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraBuffer* camPtr = (CameraBuffer*)mappedResource.pData;
	camPtr->padding = 1;
	camPtr->position = camerapos;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invView = XMMatrixTranspose(view);
	XMMATRIX invProjection = XMMatrixInverse(&XMMatrixDeterminant(projection), projection);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);


	result = deviceContext->Map(invMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	InvMatrixBuffer* invPtr = (InvMatrixBuffer*)mappedResource.pData;
	invPtr->viewMatrix = view;
	invPtr->projectionMatrix = projection;
	invPtr->invViewMatrix = invView;
	invPtr->invProjMatrix = invProjection;
	invPtr->worldMatrix = world;
	invPtr->invWorldMatrix = invWorld;
	deviceContext->Unmap(invMatrixBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &invMatrixBuffer);
	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}





