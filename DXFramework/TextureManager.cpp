// texture
// Loads and stores a single texture.
// Handles .dds, .png and .jpg (probably).
#include "TextureManager.h"


 //Attempt to load texture. If load fails use default texture.
 //Based on extension, uses slightly different loading function for different image types .dds vs .png/.jpg.
TextureManager::TextureManager(ID3D11Device* ldevice, ID3D11DeviceContext* ldeviceContext)
{
	device = ldevice;
	deviceContext = ldeviceContext;
	addDefaultTexture();
}

void TextureManager::loadTexture(const wchar_t* uid, const wchar_t* filename)
{
	HRESULT result;

	// check if file exists
	if (!filename)
	{
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return;
	}
	// if not set default texture
	if (!does_file_exist(filename))
	{
		// change default texture
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return;
	}

	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
	}

	// Load the texture in.
	if (extension == L"dds")
	{
		result = CreateDDSTextureFromFile(device, deviceContext, filename, NULL, &texture);
	}
	else
	{
		result = CreateWICTextureFromFile(device, deviceContext, filename, NULL, &texture, 0);
	}
	
	if (FAILED(result))
	{
		MessageBox(NULL, L"Texture loading error", L"ERROR", MB_OK);
	}
	else
	{
		textureMap.insert(std::make_pair(const_cast<wchar_t*>(uid), texture));
	}
}

// Release resource.
TextureManager::~TextureManager()
{
	if (texture)
	{
		texture->Release();
		texture = 0;
	}
}

// Return texture as a shader resource.
ID3D11ShaderResourceView* TextureManager::getTexture(const wchar_t* uid)
{
	if (textureMap.find(const_cast<wchar_t*>(uid)) != textureMap.end())
	{
		// texture exists
		return textureMap.at(const_cast<wchar_t*>(uid));
	}
	else
	{
		return textureMap.at(L"default");
	}
}

bool TextureManager::does_file_exist(const wchar_t *fname)
{
	std::ifstream infile(fname);
	return infile.good();
}

void TextureManager::generateTexture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	device->CreateTexture2D(&desc, NULL, &pTexture);
}

void TextureManager::addDefaultTexture()
{
	
	static const uint32_t s_pixel = 0xffffffff;

	D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&desc, &initData, &pTexture);

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(pTexture, &SRVDesc, &texture);
		textureMap.insert(std::make_pair(const_cast < wchar_t*>(L"default"), texture));
	}
	
}