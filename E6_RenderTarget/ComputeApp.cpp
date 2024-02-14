#include "ComputeApp.h"
#include "ComputeApp.h"
ComputeApp::ComputeApp()
{

}

void ComputeApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenh = screenHeight;
	screenw = screenWidth;

	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	textureShader = new ComputePipelineShader(renderer->getDevice(), hwnd);
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);


	//ComputeShader = new BaseComputeShader(renderer->getDevice(), hwnd, 10);
	computeTextureShader = new BaseComputeTextureShader(renderer->getDevice(), hwnd, 1,1);
	computeRT = new RenderTexture(renderer->getDevice(), 1, 1, 0.1f, 100.f);
}


ComputeApp::~ComputeApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool ComputeApp::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	camera->move(0.01);

	return true;
}

bool ComputeApp::render()
{

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	// Get matrices
	camera->update();
	// Get matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, computeTextureShader->getSRV(), camera->getPosition());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
	return true;
}


void ComputeApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	if (ImGui::Button("Compute")) 
	{
		//ComputeShader->GPUSort(renderer->getDeviceContext());
		computeRT->clearRenderTarget(renderer->getDeviceContext(), 1, 1, 1, 1);
		computeTextureShader->setShaderParameters(renderer->getDeviceContext(), SRV);
		computeTextureShader->compute(renderer->getDeviceContext(), 1, 1, 1);
		computeTextureShader->unbind(renderer->getDeviceContext());
		SRV = computeTextureShader->getSRV();
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
