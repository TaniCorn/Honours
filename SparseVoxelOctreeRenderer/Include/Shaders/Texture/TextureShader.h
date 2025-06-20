#pragma once

#include "BaseShader.h"
using namespace std;
using namespace DirectX;

namespace TextureView
{
	class TextureShader : public BaseShader
	{
	public:

		TextureShader(ID3D11Device* device, HWND hwnd);
		~TextureShader();

		void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& orthoView, const XMMATRIX& ortho, ID3D11ShaderResourceView* texture);
	private:
		void initShader(const wchar_t* vs, const wchar_t* ps);
	};


}
