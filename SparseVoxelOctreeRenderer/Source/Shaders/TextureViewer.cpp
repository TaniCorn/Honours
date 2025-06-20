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

const RenderTexture* TextureViewer::GetRenderTexture() const
{
	// Releasing a raw pointer as texture should be ok to give
	return Texture.get();
}
