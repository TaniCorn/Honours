#include "OctreeTracerApp.h"

#define NV_PERF_ENABLE_INSTRUMENTATION             // use macro NV_PERF_ENABLE_INSTRUMENTATION to turn on the Nsight Perf instrumentation.

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
#include "NvPerfReportGeneratorD3D11.h"
#endif

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
#include "nvperf_host_impl.h"
#endif
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
nv::perf::profiler::ReportGeneratorD3D11 g_nvperf;
NVPW_Device_ClockStatus g_clockStatus = NVPW_DEVICE_CLOCK_STATUS_UNKNOWN; // Used to restore clock state when exiting
const ULONGLONG g_warmupTicks = 500u; /* milliseconds */
ULONGLONG g_startTicks = 0u;
ULONGLONG g_currentTicks = 0u;
#endif

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

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_startTicks = GetTickCount64();
	g_nvperf.InitializeReportGenerator(renderer->getDevice());
	g_nvperf.SetFrameLevelRangeName("Frame");
	g_nvperf.SetNumNestingLevels(2);
	g_nvperf.SetMaxNumRanges(2+8); // "Frame" + "Compute"2 + "Render"
	g_nvperf.outputOptions.directoryName = "HtmlReports\\Honours";

	// LoadDriver() must be called first, which is taken care of by InitializeReportGenerator()
	g_clockStatus = nv::perf::D3D11GetDeviceClockState(renderer->getDevice());
	nv::perf::D3D11SetDeviceClockState(renderer->getDevice(), NVPW_DEVICE_CLOCK_SETTING_LOCK_TO_RATED_TDP);

#endif

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	rt = new RenderTexture(renderer->getDevice(), screenw, screenh, 0.1f, 100.0f);
	om = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	generateVoxelModel(hwnd, "Monu1", "res/monu1.vox");
	generateVoxelModel(hwnd, "Dragon", "res/dragon.vox");

	generateVoxelModel(hwnd, "Cars", "res/cars.vox");
	generateVoxelModel(hwnd, "Chair", "res/chair.vox");
	generateVoxelModel(hwnd, "Doom", "res/doom.vox");
	generateVoxelModel(hwnd, "Menger", "res/menger.vox");
	generateVoxelModel(hwnd, "teapot", "res/teapot.vox");
	generateVoxelModel(hwnd, "room", "res/room.vox");

	//generateVoxelModel(hwnd, "Dragon", "res/dragon.vox");

	octreeTracer = new OctreeTracerShader(renderer->getDevice(), hwnd, 99999, screenWidth, screenHeight);
	octreeTracer->setVoxelPalette(renderer->getDeviceContext(), voxelModelPalettes);


}


OctreeTracerApp::~OctreeTracerApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.Reset();
	nv::perf::D3D11SetDeviceClockState(renderer->getDevice(), g_clockStatus);
#endif
}


bool OctreeTracerApp::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	reconstructModels();
	computeTracer();
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameEnd();
#endif
	result = render();


	if (!result)
	{
		return false;
	}
	camera->move(0.9);

	return true;
}

bool OctreeTracerApp::render()
{
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
//


#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PushRange("RenderingVoxels");
#endif

		rt->clearRenderTarget(renderer->getDeviceContext(), 1, 1, 1, 1);
		octreeTracer->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix, camera->getPosition(), rt->getShaderResourceView(), voxelViewMode, voxelViewDepth, heatMap, ind);
		octreeTracer->setVoxelPalette(renderer->getDeviceContext(), voxelModelPalettes);
		octreeTracer->compute(renderer->getDeviceContext(), 74, 40, 1);
		octreeTracer->unbind(renderer->getDeviceContext());
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PopRange(); // Draw
#endif


}

void OctreeTracerApp::reconstructModels()
{
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameStart(renderer->getDeviceContext());
#endif
	int i = 0;
	for (auto comp : voxelModels) {
		if (!construction)
		{
			break;
		}
		i++;
		std::string name = "Computing: " + comp.first;
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PushRange(name.c_str());
#endif
		OctreeConstructorShader* octreeConstructor = comp.second.octreeConstructor;
		CPPOctree* oc = comp.second.cppOctree;
		octreeConstructor->setShaderParameters(renderer->getDeviceContext());
		octreeConstructor->updateCPPOctree(renderer->getDeviceContext(), oc);
		octreeConstructor->compute(renderer->getDeviceContext(), 1, 1, 1);
		octreeConstructor->unbind(renderer->getDeviceContext());
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PopRange(); // Draw
#endif

	}

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
	if(ImGui::Button("Profile")) {
		g_nvperf.StartCollectionOnNextFrame();
		construction = true;
	}
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
		//if (voxelModelView < voxelModelResources.size())
		//{
			
			octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), voxelModelResources);
		//}
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
	voxelModels[identifierName].pallette = &modelLoader.getPalette(identifierName);
	//voxelModelResources.push_back(voxelModelView);
	voxelModelResources[ind] = voxelModelView;
	voxelModelPalettes[ind] = &modelLoader.getPalette(identifierName);
	ind++;
}
