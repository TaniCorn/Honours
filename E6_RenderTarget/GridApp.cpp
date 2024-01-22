#include "GridApp.h"
GridApp::GridApp()
{

}

void GridApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	screenh = screenHeight;
	screenw = screenWidth;

	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	textureShader = new GridShader(renderer->getDevice(), hwnd);

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0);
}


GridApp::~GridApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool GridApp::frame()
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

bool GridApp::render()
{

	texturepass();
	return true;


}
#include <chrono>
#include <iostream>
static long long timep = 0.0f;
static int framesadd = 0;
void GridApp::texturepass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, renderTexture->getShaderResourceView(), camera->getPosition());
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	renderer->setZBuffer(true);


	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
	float t = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	timep += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	framesadd++;

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}


void GridApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("FrameTime: %i", timep / framesadd);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

