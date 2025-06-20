#include "AppScene.h"

void AppScene::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{

	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0); // 	camera->setPosition(350, 150, -2000);

	TextureViewer = std::make_unique<TextureView::TextureViewer>(renderer->getDevice(), renderer->getDeviceContext(), hwnd, screenWidth, screenHeight);
}

bool AppScene::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}




	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void AppScene::handleInput(float dt)
{
	camera->move(dt);
}

bool AppScene::render()
{
	renderer->beginScene(1.0f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	RenderTexture* rt = TextureViewer->GetRenderTexture();
	rt->clearRenderTarget(renderer->getDeviceContext(), 1, 1, 1, 1);
	//TODO: Replace nullptr with the compute tracers SRV texture
	TextureViewer->Render(renderer->getDeviceContext(), worldMatrix, orthoMatrix, orthoViewMatrix, nullptr);

	renderGUI();

	renderer->endScene();
	return true;
}

void AppScene::renderGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);
	
	// Have a ImGUI class for the specific window to render

	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
