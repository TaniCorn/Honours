#include "AppScene.h"
#include "SVOHelper.h"


AppScene::AppScene()
{
}

AppScene::~AppScene()
{
	// Remember do we need to delete pallette?
}

void AppScene::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{

	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Must set mouse as inactive otherwise it will have some weird rotation issues at the start.
	// Mouse should be re-enabled automatically when the user clicks on the window.
	input->setMouseActive(false);
	// For some reason the inputs are all up by default at the beginning, without this call, the user has to press a key twice to reset the input state
	input->resetAllInputs();
	camera->setPosition(350.0f, 150.0f, -100.0f);
	camera->setRotation(0.0f, 0.0f, 0.0f);
	camera->camSpeed = 10.0f;

	RTViewer = std::make_unique<TextureView::TextureViewer>(renderer->getDevice(), renderer->getDeviceContext(), hwnd, screenWidth, screenHeight);

	RawVoxelModels = std::make_unique<VoxelModelManager>();
	SVOModels = std::make_unique<SVOManager>();
	const int PregeneratedOctreeSize = 200000;
	SVOTraverser = std::make_unique<SVOTraverserShader>(renderer->getDevice(), hwnd, PregeneratedOctreeSize, screenWidth, screenHeight);


	SVOHelper::LoadModelInFromFile("dragon", "res/dragon.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("monu1", "res/monu1.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("cars", "res/cars.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("chair", "res/chair.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("doom", "res/doom.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("menger", "res/menger.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("teapot", "res/teapot.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("room", "res/room.vox", *SVOModels, *RawVoxelModels);

	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("dragon"), SVOModels->GetPalette("dragon"), 0);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("monu1"), SVOModels->GetPalette("monu1"), 1);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("cars"), SVOModels->GetPalette("cars"), 2);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("chair"), SVOModels->GetPalette("chair"), 3);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("doom"), SVOModels->GetPalette("doom"), 4);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("menger"), SVOModels->GetPalette("menger"), 5);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("teapot"), SVOModels->GetPalette("teapot"), 6);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("room"), SVOModels->GetPalette("room"), 7);
}

bool AppScene::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	camera->update();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	RTViewer->GetRenderTexture()->clearRenderTarget(renderer->getDeviceContext(), 1, 0, 0, 1);

	SVOTraverser->SetMatrixBuffer(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix);
	SVOTraverser->SetCameraBuffer(renderer->getDeviceContext(), camera->getPosition(), 0);
	SVOTraverser->SetViewModeBuffer(renderer->getDeviceContext(), 0, 1, 0, 2);
	SVOTraverser->SetTexture(renderer->getDeviceContext(), RTViewer->GetRenderTexture()->getShaderResourceView());

	SVOTraverser->compute(renderer->getDeviceContext(), 74, 40, 1);
	SVOTraverser->Unbind(renderer->getDeviceContext());


	result = render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool AppScene::render()
{
	renderer->beginScene(1.0f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	
	RTViewer->Render(renderer->getDeviceContext(), worldMatrix, orthoMatrix, orthoViewMatrix, SVOTraverser->GetSRV());

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
