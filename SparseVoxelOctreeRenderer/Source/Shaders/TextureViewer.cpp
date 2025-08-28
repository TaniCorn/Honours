#include "TextureViewer.h"
using namespace TextureView;

TextureViewer::TextureViewer(ID3D11Device* Device, ID3D11DeviceContext* Context, HWND WinHWND, float ScreenWidth, float ScreenHeight, float NearPlane, float FarPlane)
{
	Shader = std::make_unique<TextureShader>(Device, WinHWND);
	Texture = std::make_unique<RenderTexture>(Device, ScreenWidth, ScreenHeight, NearPlane, FarPlane);
	Mesh = std::make_unique<OrthoMesh>(Device, Context, ScreenWidth, ScreenHeight);
}

TextureViewer::~TextureViewer()
{
}

void TextureView::TextureViewer::Render(ID3D11DeviceContext* Context, XMMATRIX WorldMatrix, XMMATRIX OrthoMatrix, XMMATRIX OrthoViewMatrix, ID3D11ShaderResourceView* SRV) const
{
	Mesh->sendData(Context);
	Shader->setShaderParameters(Context, WorldMatrix, OrthoViewMatrix, OrthoMatrix, SRV);
	Shader->render(Context, Mesh->getIndexCount());
}

void TextureView::TextureViewer::ResizeTexture(ID3D11Device* Device, HWND hwnd, ID3D11DeviceContext* Context, float NewWidth, float NewHeight, float XPos, float YPos, float NearPlane, float FarPlane)
{
	if (Texture)
	{
		Texture.reset();
	}
	Texture = std::make_unique<RenderTexture>(Device, NewWidth, NewHeight, NearPlane, FarPlane);

	if(Mesh)
	{
		Mesh.reset();
	}
	Mesh = std::make_unique<OrthoMesh>(Device, Context, NewWidth, NewHeight, XPos, YPos);

	if(Shader)
	{
		Shader.reset();
	}
	Shader = std::make_unique<TextureShader>(Device, hwnd);
}

RenderTexture* TextureViewer::GetRenderTexture() const
{
	// Releasing a raw pointer as texture should be ok to give
	return Texture.get();
}
