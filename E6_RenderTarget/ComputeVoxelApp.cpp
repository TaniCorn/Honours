#include "ComputeVoxelApp.h"
#include <iostream>
#include <fstream>

int ComputeVoxelApp::IDCSV = 0;
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
	computeVoxels = new ComputeVoxelShader(renderer->getDevice(), hwnd, 4,screenWidth, screenHeight);
	voxelRT = new RenderTexture(renderer->getDevice(), screenw, screenh, 0.1f, 100.0f);
	voxelOM = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	quadTreeShader = new QuadTreeShader(renderer->getDevice(), hwnd);

	LoadVoxModel();
	WriteCPUToCSV(octree, "CPUOctree.txt");
	WriteVoxelsToCSV("CPUVoxels.txt");
	GPUOctree = new ComputeOctreeShader(renderer->getDevice(), hwnd, 200000, octree->points.size());
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
	//ComputeVoxelRender();
	//RenderVoxModel();
	CPURenderVoxModel();
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
		GPUOctree->updateCPPOctree(renderer->getDeviceContext(), octree);
		GPUOctree->compute(renderer->getDeviceContext(), 1, 1, 1);
		computeVoxels->setOctreeVoxels(renderer->getDeviceContext(), GPUOctree->getSRV());
	}
	if (ImGui::Button("Copy"))
	{
		CopyGPUOctreeToCPU(GPUOctree, octree);
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void ComputeVoxelApp::RecursiveAddOctree(ComputeOctreeShader::VoxelOctree* oc, UINT32 currentStride, Octree* currentOctree)
{
	for (int i = 0; i < 8; i++)
	{
		ComputeOctreeShader::VoxelOctree* currentOctant;
		currentOctant = &oc[currentStride];
		if (oc[currentStride].RGB != 0)
		{
			ComputeOctreeShader::VoxelOctree* o = &oc[currentStride];
			float posX = o->BottomRightBackPosition.y;
			float posY = o->TopLeftFrontPosition.z;
			float posZ = o->TopLeftFrontPosition.x;
			uint8_t col = o->RGB;
			OctVoxel* vox = new OctVoxel();
			vox->identifier = 0;
			vox->point.SetPoint(o->VoxelPosition);
			vox->color = col;

			currentOctree->storedVoxel = vox;
			Octree::points.insert(vox);
			return;
		}
		UINT32 nextOctantStride = oc[currentStride].Octants[i];
		if (nextOctantStride != 0)
		{
			ComputeOctreeShader::VoxelOctree* nextOctant;
			nextOctant = &oc[nextOctantStride];
			Octree* nextOctree;
			nextOctree = new Octree(OctPoint(nextOctant->TopLeftFrontPosition), OctPoint(nextOctant->BottomRightBackPosition));
			nextOctree->depth = currentOctree->depth + 1;
			switch (i)
			{
			case 0:
				currentOctree->TLF = nextOctree;
				break;
			case 1:
				currentOctree->TRF = nextOctree;
				break;
			case 2:
				currentOctree->BLF = nextOctree;
				break;
			case 3:
				currentOctree->BRF = nextOctree;
				break;
			case 4:
				currentOctree->TLB = nextOctree;
				break;
			case 5:
				currentOctree->TRB = nextOctree;
				break;
			case 6:
				currentOctree->BLB = nextOctree;
				break;
			case 7:
				currentOctree->BRB = nextOctree;
				break;
			default:
				break;
			}
			RecursiveAddOctree(oc, nextOctantStride, nextOctree);
		}
	}
}
void ComputeVoxelApp::CopyGPUOctreeToCPU(ComputeOctreeShader* GPU, Octree* CPU)
{
	ComputeOctreeShader::VoxelOctree* oc = GPUOctree->GetVoxelOctreeOutput(renderer->getDeviceContext());
	
	int xRes = oc[0].BottomRightBackPosition.x;
	int yRes = oc[0].TopLeftFrontPosition.y;
	int zRes = oc[0].BottomRightBackPosition.z;
	res.x = xRes;
	res.y = yRes;
	res.z = zRes;
	//resolution = xRes;
	octree = new Octree(OctPoint(0, yRes, 0), OctPoint(xRes, 0, zRes));
	colPalette = VoxModelLoader->denseModels().at(0).palette();
	octree->points.clear();

	int stride = 0;
	ComputeOctreeShader::VoxelOctree* currentOctant;
	Octree* currentOctree = octree;
	RecursiveAddOctree(oc, stride, currentOctree);
	stride = 0;
	WriteCPUToCSV(octree, "GPUOctree.txt");
	WriteVoxelsToCSV("GPUVoxels.txt");

}
void ComputeVoxelApp::RecursiveWriteCSV(Octree* CPU, std::ofstream& file)
{
	IDCSV++;
	std::string posTLF;
	posTLF = std::to_string(CPU->topLeftFrontPoint.GetPoint().x) + " , " + std::to_string(CPU->topLeftFrontPoint.GetPoint().y) + " , " + std::to_string(CPU->topLeftFrontPoint.GetPoint().z);
	std::string posBRB;
	posBRB = std::to_string(CPU->bottomRightBackPoint.GetPoint().x) + " , " + std::to_string(CPU->bottomRightBackPoint.GetPoint().y) + " , " + std::to_string(CPU->bottomRightBackPoint.GetPoint().z);
	
	int numberOfOctants = 0;
	if (CPU->BLF != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->BRF != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->TLF != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->TRF != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->BLB != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->BRB != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->TLB != nullptr)
	{
		numberOfOctants++;
	}
	if (CPU->TRB != nullptr)
	{
		numberOfOctants++;
	}
	file << "New Octant ID: " + std::to_string(IDCSV) + "\n";
	file << "Depth , " + std::to_string(CPU->depth) + " , Octants , " + std::to_string(numberOfOctants) + "\n";
	file << "Position TLF , " + posTLF + "\n";
	file << "Position BRB , " + posBRB + "\n";

	if (CPU->BLF != nullptr)
	{
		RecursiveWriteCSV(CPU->BLF, file);
	}
	if (CPU->BRF != nullptr)
	{
		RecursiveWriteCSV(CPU->BRF, file);
	}
	if (CPU->TLF != nullptr)
	{
		RecursiveWriteCSV(CPU->TLF, file);
	}
	if (CPU->TRF != nullptr)
	{
		RecursiveWriteCSV(CPU->TRF, file);
	}
	if (CPU->BLB != nullptr)
	{
		RecursiveWriteCSV(CPU->BLB, file);
	}
	if (CPU->BRB != nullptr)
	{
		RecursiveWriteCSV(CPU->BRB, file);
	}
	if (CPU->TLB != nullptr)
	{
		RecursiveWriteCSV(CPU->TLB, file);
	}
	if (CPU->TRB != nullptr)
	{
		RecursiveWriteCSV(CPU->TRB, file);
	}

	return;
}
void ComputeVoxelApp::WriteCPUToCSV(Octree* CPU, std::string name)
{
	std::ofstream myfile;
	IDCSV = 0;
	myfile.open(name);
	RecursiveWriteCSV(CPU, myfile);
	myfile.close();
}
void ComputeVoxelApp::WriteVoxelsToCSV(std::string name)
{
	std::ofstream myfile;
	IDCSV = 0;
	myfile.open(name);
	int ID = 0;
	for (auto p : Octree::points)
	{
		std::string point;
		point = std::to_string(p->point.GetPoint().x) + " , " + std::to_string(p->point.GetPoint().y) + " , " + std::to_string(p->point.GetPoint().z);
		ID++;
		myfile << "New Point ID: " + std::to_string(ID) +","+ point + "\n";
	}
	myfile.close();
}
void ComputeVoxelApp::CPURecursiveOctreeLoop(Octree* octree, float depth)
{
	depth += 0.1f;
	if (octree->BLF != nullptr)
	{
		CPURenderCubeTree(depth, octree->BLF);
		CPURecursiveOctreeLoop(octree->BLF, depth);
	}
	if (octree->BRF != nullptr)
	{
		CPURenderCubeTree(depth, octree->BRF);
		CPURecursiveOctreeLoop(octree->BRF, depth);
	}
	if (octree->TLF != nullptr)
	{
		CPURenderCubeTree(depth, octree->TLF);
		CPURecursiveOctreeLoop(octree->TLF, depth);
	}
	if (octree->TRF != nullptr)
	{
		CPURenderCubeTree(depth, octree->TRF);
		CPURecursiveOctreeLoop(octree->TRF, depth);
	}


	if (octree->BLB != nullptr)
	{
		CPURenderCubeTree(depth, octree->BLB);
		CPURecursiveOctreeLoop(octree->BLB, depth);
	}
	if (octree->BRB != nullptr)
	{
		CPURenderCubeTree(depth, octree->BRB);
		CPURecursiveOctreeLoop(octree->BRB, depth);
	}
	if (octree->TLB != nullptr)
	{
		CPURenderCubeTree(depth, octree->TLB);
		CPURecursiveOctreeLoop(octree->TLB, depth);
	}
	if (octree->TRB != nullptr)
	{
		CPURenderCubeTree(depth, octree->TRB);
		CPURecursiveOctreeLoop(octree->TRB, depth);
	}
}

void ComputeVoxelApp::CPURenderCubeTree(const float depth, Octree* childQuad)
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
	float y = childQuad->topLeftFrontPoint.GetY();
	float z = childQuad->topLeftFrontPoint.GetZ();
	XMMATRIX translation = XMMatrixTranslation(x, y, z);
	XMMATRIX localMoveMatrix = XMMatrixTranslation(1, -1, 1);


	cube->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * localMoveMatrix * scale * translation, viewMatrix, projectionMatrix, depth, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
}
void ComputeVoxelApp::CPURenderVoxModel()
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

		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		XMMATRIX viewMatrix = camera->getViewMatrix();
		XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
		XMMATRIX scale = XMMatrixScaling(res.x/2, res.y/2, res.z/2);
		XMMATRIX translation = XMMatrixTranslation(0, res.y, 0);
		XMMATRIX localMoveMatrix = XMMatrixTranslation(1, -1, 1);

		cube->sendData(renderer->getDeviceContext());
		quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * localMoveMatrix * scale * translation, viewMatrix, projectionMatrix, depth, maxDepth);
		quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());

		CPURecursiveOctreeLoop(octree, depth);

	}


	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}
