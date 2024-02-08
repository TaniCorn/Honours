#include "VoxModelApp.h"
VoxModelApp::VoxModelApp()
{

}

void VoxModelApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenh = screenHeight;
	screenw = screenWidth;

	quadTreeShader = new QuadTreeShader(renderer->getDevice(), hwnd);

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0);
	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1);

	LoadVoxModel();
}


VoxModelApp::~VoxModelApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool VoxModelApp::frame()
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

bool VoxModelApp::render()
{

	RenderVoxModel();
	return true;
}

void VoxModelApp::RecursiveOctreeLoop(Octree* octree, float depth)
{
	depth += 0.1f;
	if (octree->BLF != nullptr)
	{
		RenderCubeTree(depth, octree->BLF);
		RecursiveOctreeLoop(octree->BLF, depth);
	}
	if (octree->BRF != nullptr)
	{
		RenderCubeTree(depth, octree->BRF);
		RecursiveOctreeLoop(octree->BRF, depth);
	}
	if (octree->TLF != nullptr)
	{
		RenderCubeTree(depth, octree->TLF);
		RecursiveOctreeLoop(octree->TLF, depth);
	}
	if (octree->TRF != nullptr)
	{
		RenderCubeTree(depth, octree->TRF);
		RecursiveOctreeLoop(octree->TRF, depth);
	}


	if (octree->BLB != nullptr)
	{
		RenderCubeTree(depth, octree->BLB);
		RecursiveOctreeLoop(octree->BLB, depth);
	}
	if (octree->BRB != nullptr)
	{
		RenderCubeTree(depth, octree->BRB);
		RecursiveOctreeLoop(octree->BRB, depth);
	}
	if (octree->TLB != nullptr)
	{
		RenderCubeTree(depth, octree->TLB);
		RecursiveOctreeLoop(octree->TLB, depth);
	}
	if (octree->TRB != nullptr)
	{
		RenderCubeTree(depth, octree->TRB);
		RecursiveOctreeLoop(octree->TRB, depth);
	}
}

void VoxModelApp::RenderCubeTree(const float depth, Octree* childQuad)
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	int divis = childQuad->depth + 1;
	int fact = pow(2, divis);

	const float resx = res.x / static_cast<float>(fact);
	const float resy = res.y / static_cast<float>(fact);
	const float resz = res.z / static_cast<float>(fact);
	XMMATRIX scale = XMMatrixScaling(resx, resy, resz);
	float x = childQuad->topLeftFrontPoint.GetX();
	float y = childQuad->topLeftFrontPoint.GetY();;
	float z = childQuad->topLeftFrontPoint.GetZ();
	XMMATRIX translation = XMMatrixTranslation(x, y, z);
	XMMATRIX localMoveMatrix = XMMatrixTranslation(1, -1, 1);


	cube->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * localMoveMatrix * scale * translation, viewMatrix, projectionMatrix, depth, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
}



void VoxModelApp::LoadVoxModel()
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

void VoxModelApp::RenderVoxModel()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	//XMMATRIX scale = XMMatrixScaling(resolution / 2, resolution / 2, resolution / 2);
	XMMATRIX localMoveMatrix = XMMatrixTranslation(1, -1, 1);
	float depth = 1.0f;

	if (!bRenderOctree)
	{
		for (auto voxel : octree->points)
		{
			float OctScale = Octree::minSize / 2.0f;
			int divis = Octree::minSize;
			int fact = pow(2, divis);

			const float res = Octree::minSize / static_cast<float>(fact);
			XMMATRIX scale = XMMatrixScaling(res, res, res);
			//XMMATRIX scale = XMMatrixScaling(OctScale, OctScale, OctScale);
			OctVoxel* vox = voxel;
			magicavoxel::Color col = colPalette.at(vox->color);
			XMFLOAT4 rgba = XMFLOAT4(col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f);
			cube->sendData(renderer->getDeviceContext());
			quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * scale * XMMatrixTranslation(vox->point.GetX(), vox->point.GetY(), vox->point.GetZ()), viewMatrix, projectionMatrix, rgba.x, rgba.y, rgba.z, rgba.w);
			quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
		}

	}
	else 
	{
		RecursiveOctreeLoop(octree, depth);

	}


	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}


void VoxModelApp::gui()
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
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
