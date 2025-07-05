#include "AppScene.h"

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

	camera->setPosition(0, 0, 0);
	camera->setPosition(350, 150, -1000);
	camera->setRotation(0, 0, 0);

	RTViewer = std::make_unique<TextureView::TextureViewer>(renderer->getDevice(), renderer->getDeviceContext(), hwnd, screenWidth, screenHeight);

	RawVoxelModels = std::make_unique<VoxelModelManager>();
	SVOModels = std::make_unique<SVOManager>();
	const int PregeneratedOctreeSize = 200000;
	SVOTraverser = std::make_unique<SVOTraverserShader>(renderer->getDevice(), hwnd, PregeneratedOctreeSize, screenWidth, screenHeight);

	const std::string ModelName = "monu1";
	if (RawVoxelModels->LoadModel(ModelName, "res/monu1.vox"))
	{
		std::vector<Voxel> Voxels = RawVoxelModels->ConstructVoxelsFromModel(ModelName);
		int VoxelModelResolution = RawVoxelModels->GetModelDimensions(ModelName);
		const UINT VoxelSize = 1;
		SVOModels->InitialiseSVOModel(ModelName, Voxels.size(), VoxelModelResolution, VoxelSize);
		SVOModels->CreateSVOModel(ModelName, Voxels);

		for (size_t i = 0; i < MODELAMOUNT; i++)
		{
			voxelModelPalettes[i] = RawVoxelModels->GetPalette(ModelName);
		}
	}
	else
	{
		MessageBox(hwnd, L"Exception Init: ", L"Error loading model", MB_OK);
		throw std::runtime_error("Error loading model");
	}
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
	//

	RTViewer->GetRenderTexture()->clearRenderTarget(renderer->getDeviceContext(), 1, 0, 0, 1);
	SVOTraverser->SetShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, camera->getPosition(), RTViewer->GetRenderTexture()->getShaderResourceView(), 0, 1, 0, 2);
	SVOTraverser->SetVoxelModel(renderer->getDeviceContext(), SVOModels->GetSVOModel("monu1"), 0);
	SVOTraverser->SetVoxelPalette(renderer->getDeviceContext(), voxelModelPalettes);
	SVOTraverser->compute(renderer->getDeviceContext(), 74, 40, 1);
	SVOTraverser->Unbind(renderer->getDeviceContext());


	result = render();
	if (!result)
	{
		return false;
	}
	camera->move(0.9);

	return true;
}

bool AppScene::render()
{
	renderer->beginScene(1.0f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	/*RenderTexture* rt = TextureViewer->GetRenderTexture();
	rt->clearRenderTarget(renderer->getDeviceContext(), 1, 1, 1, 1);*/
	//TODO: Replace nullptr with the compute tracers SRV texture
	
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
