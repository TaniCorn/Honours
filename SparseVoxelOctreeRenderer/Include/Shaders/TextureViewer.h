#pragma once


#include "Texture/TextureShader.h"
#include "DXF.h"

namespace TextureView
{
	/// <summary>
	/// Contains a texture shader, an orthographic mesh and a render texture to map to that mesh. Will render a texture to the screen.
	/// </summary>
	class TextureViewer
	{
	public:
		TextureViewer(ID3D11Device* Device, ID3D11DeviceContext* Context, HWND WinHWND, float ScreenWidth, float ScreenHeight, float NearPlane = 0.1f, float FarPlane = 100.0f);
		~TextureViewer();

		void Render(ID3D11DeviceContext* Context, XMMATRIX WorldMatrix, XMMATRIX OrthoMatrix, XMMATRIX OrthoViewMatrix, ID3D11ShaderResourceView* SRV) const;
		void ResizeTexture(ID3D11Device* Device, HWND hwnd, ID3D11DeviceContext* Context, float NewWidth, float NewHeight, float XPos, float YPos, float NearPlane = 0.1f, float FarPlane = 100.0f);
		RenderTexture* GetRenderTexture() const;
	private:
		std::unique_ptr<TextureShader> Shader;
		std::unique_ptr<OrthoMesh> Mesh;
		std::unique_ptr<RenderTexture> Texture;
	};
}


