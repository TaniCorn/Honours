#pragma once
// Texture
// Loads and stores a texture ready for rendering.
// Handles mipmap generation on load.

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <d3d11.h>
//#include "../DirectXTK/Inc/DDSTextureLoader.h"
//#include "../DirectXTK/Inc/WICTextureLoader.h"
#include "DTK\include\DDSTextureLoader.h"
#include "DTK\include\WICTextureLoader.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>
//#include "Texture.h"

using namespace DirectX;

class TextureManager
{
public:
	TextureManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TextureManager();

	void loadTexture(const wchar_t* uid, const wchar_t* filename);
	ID3D11ShaderResourceView* getTexture(const wchar_t* uid);

private:
	bool does_file_exist(const wchar_t *fileName);
	void generateTexture(ID3D11Device* device);
	void addDefaultTexture();

	ID3D11ShaderResourceView* texture;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	std::map<wchar_t*, ID3D11ShaderResourceView*> textureMap;
	ID3D11Texture2D *pTexture;
};

#endif