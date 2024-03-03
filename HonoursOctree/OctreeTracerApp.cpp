#include "OctreeTracerApp.h"

OctreeTracerApp::OctreeTracerApp()
{

}

void OctreeTracerApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenh = screenHeight;
	screenw = screenWidth;

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0);


	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	rt = new RenderTexture(renderer->getDevice(), screenw, screenh, 0.1f, 100.0f);
	om = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	generateVoxelModel(hwnd, "Monu1", "res/monu1.vox");
	generateVoxelModel(hwnd, "Dragon", "res/dragon.vox");

	octreeTracer = new OctreeTracerShader(renderer->getDevice(), hwnd, 99999, screenWidth, screenHeight);
}


OctreeTracerApp::~OctreeTracerApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool OctreeTracerApp::frame()
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
	camera->move(0.1);

	return true;
}

bool OctreeTracerApp::render()
{
	computeTracer();
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
		XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

		//renderer->setZBuffer(false);
		om->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, octreeTracer->getSRV());
		textureShader->render(renderer->getDeviceContext(), om->getIndexCount());
		//renderer->setZBuffer(true);

		// Render GUI
		gui();

		// Present the rendered scene to the screen.
		renderer->endScene();


	return true;
}


void OctreeTracerApp::computeTracer()
{
	camera->update();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	rt->clearRenderTarget(renderer->getDeviceContext(), 1,1,1,1);
	octreeTracer->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, camera->getPosition(), rt->getShaderResourceView(), voxelViewMode, voxelViewDepth, heatMap);
	octreeTracer->compute(renderer->getDeviceContext(), 74, 40, 1);
	octreeTracer->unbind(renderer->getDeviceContext());
}


void OctreeTracerApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::SliderFloat("VoxelSize", &minSize, 0, 10);
	ImGui::SliderInt("VoxelViewMode", &voxelViewMode, 0,5);
	ImGui::SliderInt("VoxelViewDepth", &voxelViewDepth, 0, 20);
	ImGui::Checkbox("Heatmap", &heatMap);
	switch (voxelViewMode)
	{
	case 0:
		viewModeDisplay = "Regular Octree Traversal Method";
		break;
	case 1:
		viewModeDisplay = "Will render all voxels found(provided the octant amounts are less than 9999)";
		voxelViewDepth = 1;
		break;
	case 2:
		viewModeDisplay = "Renders the octree boxes on different depths";
		break;
	case 3:
		viewModeDisplay = "Renders number 1 as wireframes";
		voxelViewDepth = 1;
		break;
	case 4:
		viewModeDisplay = "Renders number 2 as wireframes";
		break;
	case 5:
		viewModeDisplay = "Renders 2 as wireframes above depth";
		break;
	default:
		viewModeDisplay = "Renders number 2 as wireframes";
		break;
	}
	ImGui::Text(viewModeDisplay.c_str());


	int vmvChanged = voxelModelView;
	ImGui::SliderInt("VoxelModelView", &voxelModelView, 0, 10);
	if (voxelModelView != vmvChanged)
	{
		if (voxelModelView < voxelModelResources.size())
		{
			octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), voxelModelResources[voxelModelView]);
		}
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

CPPOctree* OctreeTracerApp::loadVoxModel(std::string identifierName, std::string filepath)
{
	//NOTE: Z is up in magicavoxel
	modelLoader.loadModel(identifierName, filepath);
	int res = modelLoader.getModelDimensions(identifierName);
	std::vector<Voxel> voxels = modelLoader.getModelVoxels(identifierName);
	CPPOctree* octree = new CPPOctree();
	Octree* oc;
	oc = new Octree(XMFLOAT3(0, res, 0), XMFLOAT3(res, 0, res));
	const int sizeOfVoxels = 1;

	for (int i = 0; i < voxels.size(); i++)
	{
		Voxel* vox = new Voxel(voxels[i]);
		oc->insert(vox, sizeOfVoxels);
		octree->voxels.push_back(vox);

	}
	octree->minSize = 1;
	octree->octantAmount = oc->getOctantAmount(oc);
	octree->octree = oc;
	return octree;
}

void OctreeTracerApp::generateVoxelModel(HWND hwnd, std::string identifierName, std::string filepath)
{
	CPPOctree* cppOctree = loadVoxModel(identifierName, filepath);
	OctreeConstructorShader* octreeConstructor = new OctreeConstructorShader(renderer->getDevice(), hwnd, cppOctree->octantAmount, cppOctree->voxels.size());
	ID3D11ShaderResourceView* voxelModelView;
	octreeConstructor->setShaderParameters(renderer->getDeviceContext());
	octreeConstructor->updateCPPOctree(renderer->getDeviceContext(), cppOctree);
	octreeConstructor->compute(renderer->getDeviceContext(), 1, 1, 1);
	octreeConstructor->unbind(renderer->getDeviceContext());
	voxelModelView = octreeConstructor->getSRV();

	voxelModels[identifierName].cppOctree = cppOctree;
	voxelModels[identifierName].octreeConstructor = octreeConstructor;
	voxelModels[identifierName].voxelModel = voxelModelView;
	voxelModelResources.push_back(voxelModelView);
}
