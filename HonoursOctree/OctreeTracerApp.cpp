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
	
	loadModels();

	int i = 0;
	for (auto comp : voxelModels)
	{
		voxelModels[comp.first].pallette = &modelLoader.getPalette(comp.first);
		voxelModelPalettes[i] = voxelModels[comp.first].pallette;
		i++;
	}

	octreeTracer = new OctreeTracerShader(renderer->getDevice(), hwnd, 200000, screenWidth, screenHeight);
	//cpuReconstruction();
	octreeTracer->setVoxelPalette(renderer->getDeviceContext(), voxelModelPalettes);
	//octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), gpuOctreeRepresentation);

	camera->setPosition(350, 150, -2000);

}
void OctreeTracerApp::loadModels()
{
	memMeasure.CaptureStart();

	modelNames[0] = "monu1";
	modelNames[1] = "dragon";
	modelNames[2] = "cars";
	modelNames[3] = "chair";
	modelNames[4] = "doom";
	modelNames[5] = "menger";
	modelNames[6] = "teapot";
	modelNames[7] = "room";

	modelLoader.loadModel("monu1", "res/monu1.vox");
	modelLoader.loadModel("dragon", "res/dragon.vox");
	modelLoader.loadModel("cars", "res/cars.vox");
	modelLoader.loadModel("chair", "res/chair.vox");
	modelLoader.loadModel("doom", "res/doom.vox");
	modelLoader.loadModel("menger", "res/menger.vox");
	modelLoader.loadModel("teapot", "res/teapot.vox");
	modelLoader.loadModel("room", "res/room.vox");

	voxelModels["monu1"] = VoxelOctreeModels();
	voxelModels["dragon"] = VoxelOctreeModels();
	voxelModels["cars"] = VoxelOctreeModels();
	voxelModels["chair"] = VoxelOctreeModels();
	voxelModels["doom"] = VoxelOctreeModels();
	voxelModels["menger"] = VoxelOctreeModels();
	voxelModels["teapot"] = VoxelOctreeModels();
	voxelModels["room"] = VoxelOctreeModels();
	memMeasure.CaptureEnd();
	//magicavoxel::Palette* g = &modelLoader.getPalette("monu1");


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
	//recontime -= timer->getTime();
	//if (recontime < 0 && construction)

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameStart(renderer->getDeviceContext());
#endif
	if (construction == 0)
	{
		computeTracer();
	}
	else if (construction == 1)
	{
		cpuReconstruction();
	}
	else if (construction == 2)
	{
		gpuReconstruction(wnd);
	}
	else if (construction == 3) 
	{
		cpuReconstruction();
		computeTracer();
	}
	else if (construction == 4)
	{
		gpuReconstruction(wnd);
		computeTracer();
	}
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
	//Renders the outputted SRV to pixel shader
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	om->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, octreeTracer->getSRV());
	textureShader->render(renderer->getDeviceContext(), om->getIndexCount());

	gui();

	renderer->endScene();
	return true;
}


void OctreeTracerApp::computeTracer()
{
	//Calculates the voxel tracer
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



void OctreeTracerApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	//ImGui::SliderFloat("VoxelSize", &minSize, 0, 10);
	ImGui::Text("Step 1. Construct a model");
	ImGui::Text("Step 2. Mess around with VoxelViewMode and VoxelViewDepth");
	ImGui::Text("WARNING: Don't construct all on GPU");
	ImGui::SliderInt("VoxelViewMode", &voxelViewMode, 0,5);
	ImGui::Text(viewModeDisplay.c_str());
	ImGui::SliderInt("VoxelViewDepth", &voxelViewDepth, 0, 8);
	heatMap = false;
	switch (voxelViewMode)
	{
	case 0:
		viewModeDisplay = "Octree Tracer";
		break;
	case 1:
		viewModeDisplay = "Heatmap";
		heatMap = true;
		break;
	case 2:
		viewModeDisplay = "Old Tracer Heatmap";
		heatMap = true;
		break;
	case 3:
		viewModeDisplay = "Box render at depth";
		break;
	case 4:
		viewModeDisplay = "Wireframe render at depth";
		break;
	case 5:
		viewModeDisplay = "Wireframe render above depth";
		break;
	default:
		viewModeDisplay = "Octree Tracer";
		break;
	}
	if (ImGui::TreeNode("Construction"))
	{
		if (ImGui::Button("Construct all octrees in CPU")) {
			cpuReconstruction();
			octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), gpuOctreeRepresentation);
		}
		if (ImGui::Button("Construct all octrees in GPU")) {
			gpuReconstruction(wnd);
			octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), voxelModelResources);
		}

		ImGui::SliderInt("Model Number", &modelToConstruct, 0, 7);
		ImGui::Text(modelNames[modelToConstruct].c_str());
		if (ImGui::Button("Construct single octree in GPU")) {
			specificGPUConstruction(wnd, modelNames[modelToConstruct], voxelModels[modelNames[modelToConstruct]].octreeConstructor);
			octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), voxelModelResources);
		}
		if (ImGui::Button("Construct single octree in CPU")) {
			specificCPUConstruction(modelNames[modelToConstruct], voxelModels[modelNames[modelToConstruct]].gpuOctree);
			octreeTracer->setOctreeVoxel(renderer->getDeviceContext(), voxelModels[modelNames[modelToConstruct]].gpuOctree);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Profiling"))
	{
		if (ImGui::Button("Profile Tracer")) {
			g_nvperf.StartCollectionOnNextFrame();
		}
		if (ImGui::Button("Profile CPU Construction")) {
			g_nvperf.StartCollectionOnNextFrame();
			construction = 1;
		}
		if (ImGui::Button("Profile GPU Construction")) {
			g_nvperf.StartCollectionOnNextFrame();
			construction = 2;
		}
		if (ImGui::Button("Profile CPU Construction and Tracer")) {
			g_nvperf.StartCollectionOnNextFrame();
			construction = 3;
		}
		if (ImGui::Button("Profile GPU Construction and Tracer")) {
			g_nvperf.StartCollectionOnNextFrame();
			construction = 4;
		}
		ImGui::InputText("MemorySaveFile", userinput, 40);
		if (ImGui::Button("Memory Capture Start")) {
			memMeasure.CaptureStart();
		}
		if (ImGui::Button("Memory Capture End")) {
			memMeasure.CaptureEnd();
			std::string s = userinput;
			memMeasure.SingleOutputWithRam(userinput);
		}
		ImGui::TreePop();
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void OctreeTracerApp::gpuReconstruction(HWND hwnd)
{
	int i = 0;
	for (auto comp : voxelModels)
	{
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PushRange(comp.first.c_str());
#endif
		std::string identifierName = comp.first;
		voxelModels[identifierName].octreeConstructor = constructInGPU(hwnd, identifierName, voxelModels[identifierName].octreeConstructor);
		voxelModelResources[i] = voxelModels[identifierName].octreeConstructor->getSRV();
		voxelModels[identifierName].pallette = &modelLoader.getPalette(identifierName);
		voxelModelPalettes[i] = &modelLoader.getPalette(identifierName);
		i++;

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PopRange(); // Draw
#endif
	}
}

void OctreeTracerApp::cpuReconstruction()
{

#if TMMEASURE
	tMeasure.CaptureStart();
#endif
	int i = 0;
	for (auto comp : voxelModels)
	{
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PushRange(comp.first.c_str());
#endif
		std::string identifierName = comp.first;
		voxelModels[identifierName].gpuOctree = constructInCPU(identifierName, voxelModels[identifierName].gpuOctree);
		gpuOctreeRepresentation[i] = voxelModels[identifierName].gpuOctree;
		voxelModels[identifierName].pallette = &modelLoader.getPalette(identifierName);
		voxelModelPalettes[i] = &modelLoader.getPalette(identifierName);
		i++;
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PopRange(); // Draw
#endif
	}
	octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), gpuOctreeRepresentation);
#if TMMEASURE
	tMeasure.CaptureEnd();
	tMeasure.SingleOutput("Passing CPU Octree to GPU");
#endif

}

GPUOctree* OctreeTracerApp::constructInCPU(std::string identifierName, GPUOctree* oc)
{
	//NOTE: Z is up in magicavoxel
	int res = modelLoader.getModelDimensions(identifierName);
	std::vector<Voxel> voxels = modelLoader.getModelVoxels(identifierName);

#if TMMEASURE
	tMeasure.CaptureStart();
#endif
	GPUOctree* octree;
	if (oc == nullptr)
	{
		octree = new GPUOctree(voxels.size());
		octree->octree[0].TopLeftFrontPosition = XMFLOAT3(0, res, 0);
		octree->octree[0].BottomRightBackPosition = XMFLOAT3(res, 0, res);
	}
	else {
		octree = oc;
		oc->Clear();
	}


	for (int i = 0; i < voxels.size(); i++)
	{
		Voxel* vox = new Voxel(voxels[i]);
		octree->InsertVoxel(vox);
		octree->voxels.push_back(vox);
	}
#if TMMEASURE
	tMeasure.CaptureEnd();
	tMeasure.SingleOutput("CPU: " + identifierName);
#endif
	return octree;
}

OctreeConstructorShader* OctreeTracerApp::constructInGPU(HWND hwnd, std::string identifierName, OctreeConstructorShader* oc)
{
	//NOTE: Z is up in magicavoxel
	int res = modelLoader.getModelDimensions(identifierName);
	std::vector<Voxel> voxels = modelLoader.getModelVoxels(identifierName);
#if TMMEASURE
	tMeasure.CaptureStart();
#endif
	OctreeConstructorShader* octreeConstructor; 
	if (oc == nullptr)
	{
		octreeConstructor = new OctreeConstructorShader(renderer->getDevice(), hwnd, 200000, voxels.size());
	}
	else
	{
		//delete oc;
		//oc = new OctreeConstructorShader(renderer->getDevice(), hwnd, 200000, voxels.size());
		octreeConstructor = oc;
	}

	octreeConstructor->setShaderParameters(renderer->getDeviceContext());
	octreeConstructor->setVoxels(renderer->getDeviceContext(), voxels, res, res, res);
	octreeConstructor->compute(renderer->getDeviceContext(), 1, 1, 1);
	octreeConstructor->unbind(renderer->getDeviceContext());

#if TMMEASURE
	tMeasure.CaptureEnd();
	tMeasure.SingleOutput(identifierName);
#endif

	return octreeConstructor;
}

void OctreeTracerApp::specificGPUConstruction(HWND hwnd, std::string identifierName, OctreeConstructorShader* oc)
{
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameStart(renderer->getDeviceContext());
#endif
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PushRange(identifierName.c_str());
#endif
		voxelModels[identifierName].octreeConstructor = constructInGPU(hwnd, identifierName, voxelModels[identifierName].octreeConstructor);
		voxelModelResources[0] = voxelModels[identifierName].octreeConstructor->getSRV();
		voxelModels[identifierName].pallette = &modelLoader.getPalette(identifierName);
		voxelModelPalettes[0] = &modelLoader.getPalette(identifierName);
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
		g_nvperf.PopRange(); // Draw
#endif
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameEnd();
#endif
}

void OctreeTracerApp::specificCPUConstruction(std::string identifierName, GPUOctree* oc)
{
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameStart(renderer->getDeviceContext());
#endif
#if TMMEASURE
	tMeasure.CaptureStart();
#endif
		voxelModels[identifierName].gpuOctree = constructInCPU(identifierName, voxelModels[identifierName].gpuOctree);
		gpuOctreeRepresentation[0] = voxelModels[identifierName].gpuOctree;
		voxelModels[identifierName].pallette = &modelLoader.getPalette(identifierName);
		voxelModelPalettes[0] = &modelLoader.getPalette(identifierName);
		octreeTracer->setOctreeVoxels(renderer->getDeviceContext(), gpuOctreeRepresentation);
#if TMMEASURE
	tMeasure.CaptureEnd();
	tMeasure.SingleOutput("Passing CPU Octree to GPU");
#endif

#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.OnFrameEnd();
#endif
}
