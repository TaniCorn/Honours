#include "ComputeVoxelApp.h"
ComputeVoxelApp::ComputeVoxelApp()
{

}

void ComputeVoxelApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenh = screenHeight;
	screenw = screenWidth;

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0);
	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1);


	voxelShader = new VoxelShader(renderer->getDevice(), hwnd);
	computeVoxels = new ComputeVoxelShader(renderer->getDevice(), hwnd, screenWidth, screenHeight);
	voxelRT = new RenderTexture(renderer->getDevice(), screenw, screenh, 0.1f, 100.0f);
	voxelOM = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	LoadVoxModel();

	GPUOctree = new ComputeOctreeShader(renderer->getDevice(), hwnd, 100, 2);
}


ComputeVoxelApp::~ComputeVoxelApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool ComputeVoxelApp::frame()
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

bool ComputeVoxelApp::render()
{
	ComputeVoxelRender();
	RenderVoxModel();
	return true;
}




void ComputeVoxelApp::LoadVoxModel()
{
	if (VoxModelLoader == nullptr)
	{
		VoxModelLoader = new magicavoxel::VoxFile(true, true);
		VoxModelLoader->Load("res/monu1.vox");
	}


	const magicavoxel::VoxSparseModel& sparseModel = VoxModelLoader->sparseModels().at(0);// List of non-empty voxels


	int xRes = sparseModel.size().x;
	int yRes = sparseModel.size().y;
	int zRes = sparseModel.size().z;
	res.x = xRes;
	res.y = yRes;
	res.z = zRes;
	//resolution = xRes;
	octree = new Octree(OctPoint(0, yRes, 0), OctPoint(xRes, 0, zRes));
	colPalette = VoxModelLoader->denseModels().at(0).palette();
	Octree::OctantAmount = 1;

	for (int i = 0; i < sparseModel.voxels().size(); i++)
	{
		uint8_t posX = sparseModel.voxels()[i].y;
		uint8_t posY = sparseModel.voxels()[i].z;
		uint8_t posZ = sparseModel.voxels()[i].x;
		uint8_t col = sparseModel.voxels()[i].color;
		OctVoxel* vox = new OctVoxel();
		vox->identifier = i;
		vox->point.SetPoint(posX, posY, posZ);
		vox->color = col;

		octree->insert(vox);
		int octreevoxsize = Octree::points.size();
		octpoints.push_back(vox);

	}
	yRes = sparseModel.size().y;
	Octree::minSize = 1;
}

void ComputeVoxelApp::RenderVoxModel()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	//renderer->setZBuffer(false);
	voxelOM->sendData(renderer->getDeviceContext());
	voxelShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, computeVoxels->getSRV());
	voxelShader->render(renderer->getDeviceContext(), voxelOM->getIndexCount());
	//renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void ComputeVoxelApp::ComputeVoxelRender()
{
	camera->update();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	voxelRT->clearRenderTarget(renderer->getDeviceContext(), 1,1,1,1);
	computeVoxels->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, camera->getPosition(), voxelRT->getShaderResourceView());
	computeVoxels->compute(renderer->getDeviceContext(), 74, 40, 1);
	computeVoxels->unbind(renderer->getDeviceContext());
}


void ComputeVoxelApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Octree Render", &bRenderOctree);
	if (bRenderOctree)
	{
		wireframeToggle = true;
	}
	ImGui::SliderInt("VoxelSize", &Octree::minSize, 1, 20);

	if (ImGui::Button("Reload"))
	{
		delete octree;
		LoadVoxModel();
	}

	if (ImGui::Button("Compute"))
	{
		GPUOctree->setShaderParameters(renderer->getDeviceContext());
		GPUOctree->updateCPPOctree(renderer->getDeviceContext(), nullptr);
		GPUOctree->compute(renderer->getDeviceContext(), 1, 1, 1);
		GPUOctree->GetVoxelOctreeOutput(renderer->getDeviceContext());
		computeVoxels->setOctreeVoxels(renderer->getDeviceContext(), GPUOctree->getSRV());
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
