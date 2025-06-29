#include "SVOTraverserShader.h"
using namespace OctreeGPU;
SVOTraverserShader::SVOTraverserShader(ID3D11Device* Device, HWND Hwnd, int OctantNumber, int InWidth, int InHeight) 
	: BaseShader(Device, Hwnd),
	Device(Device), Hwnd(Hwnd), 
	NumberOfOctants(OctantNumber), 
	ScreenWidth(InWidth), ScreenHeight(InHeight)
{
	InitShader(L"OctreeTracer_cs.cso", NULL);
}
SVOTraverserShader::~SVOTraverserShader()
{
	if (ComputeShaderBitonic)
	{
		ComputeShaderBitonic->Release();
		ComputeShaderBitonic = 0;
	}
	if (InMatrixBuffer)
	{
		InMatrixBuffer->Release();
		InMatrixBuffer = 0;
	}
	if (InCameraBuffer)
	{
		InCameraBuffer->Release();
		InCameraBuffer = 0;
	}
	if (InViewBuffer)
	{
		InViewBuffer->Release();
		InViewBuffer = 0;
	}
	for (int i = 0; i < 8; i++)
	{
		if (InOctreeBuffer[i])
		{
			InOctreeBuffer[i]->Release();
			InOctreeBuffer[i] = 0;
		}
		if (InOctreeSRV[i])
		{
			InOctreeSRV[i]->Release();
			InOctreeSRV[i] = 0;
		}
	}

	if (InOctreeStagingBuffer)
	{
		InOctreeStagingBuffer->Release();
		InOctreeStagingBuffer = 0;
	}
	if (InVoxelPaletteBuffer)
	{
		InVoxelPaletteBuffer->Release();
		InVoxelPaletteBuffer = 0;
	}
	if (Tex)
	{
		Tex->Release();
		Tex = 0;
	}

	if (TexSRV)
	{
		TexSRV->Release();
		TexSRV = 0;
	}
	if (InPaletteSRV)
	{
		InPaletteSRV->Release();
		InPaletteSRV = 0;
	}
	if (TexUAV)
	{
		TexUAV->Release();
		TexUAV = 0;
	}
}
ID3D11ShaderResourceView* SVOTraverserShader::GetSRV()
{
	return TexSRV;
}
HRESULT SVOTraverserShader::CreateConstantBuffer(ID3D11Buffer** OutBuffer, UINT ByteWidth)
{
	HRESULT Hr = S_OK;

	D3D11_BUFFER_DESC ConstantBufferDesc = {};
	ConstantBufferDesc.ByteWidth = ByteWidth;
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Hr = Device->CreateBuffer(&ConstantBufferDesc, NULL, OutBuffer);
	return Hr;
}

void SVOTraverserShader::InitShader(const wchar_t* CFile, const wchar_t* Blank)
{
	loadComputeShader(CFile);
	CreateInput();
	CreateOutput();
}

HRESULT SVOTraverserShader::CreateInput()
{
	//TODO: Move tex to output bind

	HRESULT Hr;
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));
	TextureDesc.Width = ScreenWidth;
	TextureDesc.Height = ScreenHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;
	Tex = 0;
	Hr = renderer->CreateTexture2D(&TextureDesc, 0, &Tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc;
	SrvDesc.Format = TextureDesc.Format;
	SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = 1;
	Hr = renderer->CreateShaderResourceView(Tex, &SrvDesc, &TexSRV);

	D3D11_SHADER_RESOURCE_VIEW_DESC OctreeDesc;
	OctreeDesc.Format = DXGI_FORMAT_UNKNOWN;
	OctreeDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	OctreeDesc.BufferEx.FirstElement = 0;
	OctreeDesc.BufferEx.Flags = 0;
	OctreeDesc.BufferEx.NumElements = NumberOfOctants;
	D3D11_BUFFER_DESC ConstantDataDesc;
	ConstantDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantDataDesc.ByteWidth = sizeof(SVOSRVRepresentation) * NumberOfOctants;
	ConstantDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ConstantDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantDataDesc.StructureByteStride = sizeof(SVOSRVRepresentation);
	ConstantDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	for (int i = 0; i < 8; i++)
	{
		Hr = Device->CreateBuffer(&ConstantDataDesc, 0, &InOctreeBuffer[i]);
		Hr = Device->CreateShaderResourceView(InOctreeBuffer[i], &OctreeDesc, &InOctreeSRV[i]);
	}

	ConstantDataDesc.ByteWidth = sizeof(VoxelColor) * 8;
	ConstantDataDesc.StructureByteStride = sizeof(VoxelColor);
	Hr = Device->CreateBuffer(&ConstantDataDesc, 0, &InVoxelPaletteBuffer);
	OctreeDesc.BufferEx.NumElements = 8;
	Hr = Device->CreateShaderResourceView(InVoxelPaletteBuffer, &OctreeDesc, &InPaletteSRV);

	D3D11_BUFFER_DESC OutputDesc;
	OutputDesc.Usage = D3D11_USAGE_DEFAULT;
	OutputDesc.ByteWidth = sizeof(SVOSRVRepresentation) * NumberOfOctants;
	OutputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	OutputDesc.CPUAccessFlags = 0;
	OutputDesc.StructureByteStride = sizeof(SVOSRVRepresentation);
	OutputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	OutputDesc.Usage = D3D11_USAGE_DEFAULT;
	OutputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	OutputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Hr = (Device->CreateBuffer(&OutputDesc, 0, &InOctreeStagingBuffer));

	Hr = CreateConstantBuffer(&InMatrixBuffer, sizeof(InvMatrixBuffer));
	Hr = CreateConstantBuffer(&InCameraBuffer, sizeof(CameraBuffer));
	Hr = CreateConstantBuffer(&InViewBuffer, sizeof(ViewModeBuffer));

	return Hr;
}

HRESULT SVOTraverserShader::CreateOutput()
{
	HRESULT Hr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(DescUAV));
	DescUAV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	DescUAV.Texture2D.MipSlice = 0;
	Hr = renderer->CreateUnorderedAccessView(Tex, &DescUAV, &TexUAV);

	return Hr;
}

void SVOTraverserShader::SetShaderParameters(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection, XMFLOAT3 CameraPos, ID3D11ShaderResourceView* Texture, int VoxelView, int ViewDepth, bool Heat, int AmountOfOctrees)
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	DirectX::XMVECTOR F = DirectX::XMMatrixDeterminant(Projection);
	DirectX::XMVECTOR F2 = DirectX::XMMatrixDeterminant(World);

	XMMATRIX InvView = DirectX::XMMatrixTranspose(View);
	XMMATRIX InvProjection = DirectX::XMMatrixInverse(&F, Projection);
	XMMATRIX InvWorld = DirectX::XMMatrixInverse(&F2, World);
	Result = DeviceContext->Map(InMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	InvMatrixBuffer* InvPtr = (InvMatrixBuffer*)MappedResource.pData;
	InvPtr->ViewMatrix = View;
	InvPtr->ProjectionMatrix = Projection;
	InvPtr->InvViewMatrix = InvView;
	InvPtr->InvProjMatrix = InvProjection;
	InvPtr->WorldMatrix = World;
	InvPtr->InvWorldMatrix = InvWorld;
	DeviceContext->Unmap(InMatrixBuffer, 0);

	Result = DeviceContext->Map(InCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CameraBuffer* CamPtr = (CameraBuffer*)MappedResource.pData;
	CamPtr->Padding = VoxelView;
	CamPtr->Position = CameraPos;
	DeviceContext->Unmap(InCameraBuffer, 0);

	Result = DeviceContext->Map(InViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	ViewModeBuffer* ViewPtr = (ViewModeBuffer*)MappedResource.pData;
	ViewPtr->Mode = VoxelView;
	ViewPtr->Variable = ViewDepth;
	ViewPtr->Heatmap = Heat;
	ViewPtr->AmountOfOctrees = AmountOfOctrees;
	DeviceContext->Unmap(InViewBuffer, 0);

	DeviceContext->CSSetConstantBuffers(0, 1, &InMatrixBuffer);
	DeviceContext->CSSetConstantBuffers(1, 1, &InCameraBuffer);
	DeviceContext->CSSetConstantBuffers(2, 1, &InViewBuffer);
	DeviceContext->CSSetConstantBuffers(3, 1, &InVoxelPaletteBuffer);

	DeviceContext->CSSetShaderResources(0, 1, &Texture);
	DeviceContext->CSSetUnorderedAccessViews(0, 1, &TexUAV, 0);
}

void SVOTraverserShader::SetMatrixBuffer(ID3D11DeviceContext* DeviceContext, const XMMATRIX& World, const XMMATRIX& OrthoView, const XMMATRIX& Ortho, const XMMATRIX& View, const XMMATRIX& Projection)
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	DirectX::XMVECTOR F = DirectX::XMMatrixDeterminant(Projection);
	DirectX::XMVECTOR F2 = DirectX::XMMatrixDeterminant(World);

	XMMATRIX InvView = DirectX::XMMatrixTranspose(View);
	XMMATRIX InvProjection = DirectX::XMMatrixInverse(&F, Projection);
	XMMATRIX InvWorld = DirectX::XMMatrixInverse(&F2, World);
	Result = DeviceContext->Map(InMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	InvMatrixBuffer* InvPtr = (InvMatrixBuffer*)MappedResource.pData;
	InvPtr->ViewMatrix = View;
	InvPtr->ProjectionMatrix = Projection;
	InvPtr->InvViewMatrix = InvView;
	InvPtr->InvProjMatrix = InvProjection;
	InvPtr->WorldMatrix = World;
	InvPtr->InvWorldMatrix = InvWorld;
	DeviceContext->Unmap(InMatrixBuffer, 0);
	DeviceContext->CSSetConstantBuffers(0, 1, &InMatrixBuffer);
}

void SVOTraverserShader::SetCameraBuffer(ID3D11DeviceContext* DeviceContext, const XMFLOAT3& CameraPos, int VoxelView)
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	Result = DeviceContext->Map(InCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CameraBuffer* CamPtr = (CameraBuffer*)MappedResource.pData;
	CamPtr->Padding = VoxelView;
	CamPtr->Position = CameraPos;
	DeviceContext->Unmap(InCameraBuffer, 0);
	DeviceContext->CSSetConstantBuffers(1, 1, &InCameraBuffer);
}

void SVOTraverserShader::SetViewModeBuffer(ID3D11DeviceContext* DeviceContext, int VoxelView, int ViewDepth, bool Heat, int AmountOfOctrees)
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	Result = DeviceContext->Map(InViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	ViewModeBuffer* ViewPtr = (ViewModeBuffer*)MappedResource.pData;
	ViewPtr->Mode = VoxelView;
	ViewPtr->Variable = ViewDepth;
	ViewPtr->Heatmap = Heat;
	ViewPtr->AmountOfOctrees = AmountOfOctrees;
	DeviceContext->Unmap(InViewBuffer, 0);
	DeviceContext->CSSetConstantBuffers(2, 1, &InViewBuffer);
}

void SVOTraverserShader::SetTexture(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Texture)
{
	DeviceContext->CSSetShaderResources(0, 1, &Texture);
	DeviceContext->CSSetUnorderedAccessViews(0, 1, &TexUAV, 0);
}

void SVOTraverserShader::SetOctreeVoxels(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView* Octree[8])
{
	DeviceContext->CSSetShaderResources(1, 8, Octree);
}

void SVOTraverserShader::SetOctreeVoxels(ID3D11DeviceContext* DeviceContext, SVOConstructorCPU* Octree[8])
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	for (int i = 0; i < 8; i++)
	{
		SVOConstructorCPU* Oc = Octree[i];
		if (Oc == NULL)
		{
			return;
		}
		Result = DeviceContext->Map(InOctreeBuffer[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		SVOSRVRepresentation* Oct = (SVOSRVRepresentation*)MappedResource.pData;
		const int OctreeLength = Octree[i]->GetMaxStride();
		for (int j = 0; j < OctreeLength; j++)
		{
			int Index = j;

			Octree[i]->GetOctant(j);
			Oct[Index].BottomRightBackPosition = Octree[i]->GetOctant(j).GetBottomRightBackPosition();
			Oct[Index].TopLeftFrontPosition = Octree[i]->GetOctant(j).GetTopLeftFrontPosition();
			Oct[Index].Depth = Octree[i]->GetOctant(j).GetDepth();
			Oct[Index].RGB = Octree[i]->GetOctant(j).GetColorIndex();
			Oct[Index].VoxelPosition = Octree[i]->GetOctant(j).GetVoxelPosition();
			for (int k = 0; k < 8; k++)
			{
				Oct[Index].Octants[k] = Octree[i]->GetOctant(j).GetOctantStride(k);
			}
		}
		DeviceContext->Unmap(InOctreeBuffer[i], 0);
		DeviceContext->CSSetShaderResources(1 + i, 1, &InOctreeSRV[i]);
	}
}

void SVOTraverserShader::SetVoxelModel(ID3D11DeviceContext* DeviceContext, SVOConstructorCPU* Octree, unsigned short Index)
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	Result = DeviceContext->Map(InOctreeBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	SVOSRVRepresentation* Oct = (SVOSRVRepresentation*)MappedResource.pData;
	const int OctreeLength = Octree->GetMaxStride();
	for (int j = 0; j < OctreeLength; j++)
	{
		int Index = j;

		Oct[Index].BottomRightBackPosition = Octree->GetOctant(j).GetBottomRightBackPosition();
		Oct[Index].TopLeftFrontPosition = Octree->GetOctant(j).GetTopLeftFrontPosition();
		Oct[Index].Depth = Octree->GetOctant(j).GetDepth();
		Oct[Index].RGB = Octree->GetOctant(j).GetColorIndex();
		Oct[Index].VoxelPosition = Octree->GetOctant(j).GetVoxelPosition();
		for (int k = 0; k < 8; k++)
		{
			Oct[Index].Octants[k] = Octree->GetOctant(j).GetOctantStride(k);
		}
	}
	DeviceContext->Unmap(InOctreeBuffer[Index], 0);
	DeviceContext->CSSetShaderResources(1 + Index, 1, &InOctreeSRV[Index]);
}

void SVOTraverserShader::SetVoxelPalette(ID3D11DeviceContext* DeviceContext, magicavoxel::Palette* Palettes[8])
{
	HRESULT Result;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	Result = DeviceContext->Map(InVoxelPaletteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	VoxelPaletteBuffer* Col = (VoxelPaletteBuffer*)MappedResource.pData;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			magicavoxel::Color C = Palettes[i]->at(j);
			XMFLOAT4 Rgba = XMFLOAT4(C.r / 255.f, C.g / 255.f, C.b / 255.f, C.a / 255.f);
			UINT32 Ucol =
				((static_cast<UINT32>(C.r)) & 0x000000ff) |
				((static_cast<UINT32>(C.g) << 8) & 0x0000ff00) |
				((static_cast<UINT32>(C.b) << 16) & 0x00ff0000) |
				((static_cast<UINT32>(C.a) << 24) & 0xff000000);

			const float Coefficient = 1.f;
			float R = (Ucol & 0x000000ff) * Coefficient;
			float G = ((Ucol >> 8) & 0x000000ff) * Coefficient;
			float B = ((Ucol >> 16) & 0x000000ff) * Coefficient;
			float A = ((Ucol >> 24) & 0x000000ff) * Coefficient;

			Col->Palettes[i].Rgba[j] = Ucol;
		}
	}
	DeviceContext->Unmap(InVoxelPaletteBuffer, 0);

	DeviceContext->CSSetConstantBuffers(3, 1, &InVoxelPaletteBuffer);
	DeviceContext->CSSetShaderResources(9, 1, &InPaletteSRV);
}



void SVOTraverserShader::Unbind(ID3D11DeviceContext* Dc)
{
	ID3D11ShaderResourceView* NullSRV[] = { NULL };
	Dc->CSSetShaderResources(0, 1, NullSRV);

	ID3D11UnorderedAccessView* NullUAV[] = { NULL };
	Dc->CSSetUnorderedAccessViews(0, 1, NullUAV, 0);

	Dc->CSSetShader(nullptr, nullptr, 0);
}