#include "AppScene.h"
#include "SVOHelper.h"

#define BENCHMARK 1
#if BENCHMARK
	#include "Perfkit/NvPerfUtility/NVPerfReportGeneratorD3D11.h"
	#include "Perfkit/windows-desktop-x64/nvperf_host_impl.h"
	#include "Perfkit/nvperf_target.h"
	//nv::perf::profiler::ReportGeneratorD3D11 g_nvperf;
	//nv::perf::ClockInfo g_clockStatus; // Used to restore clock state when exiting
	//const ULONGLONG g_warmupTicks = 500u; /* milliseconds */
	//ULONGLONG g_startTicks = 0u;
	//ULONGLONG g_currentTicks = 0u;

	nv::perf::profiler::ReportGeneratorD3D11 g_nvperf;
	const double m_nvperfWarmupTime = 0.5; // Wait 0.5s to allow the clock to stabalize before begining to profile
	nv::perf::ClockInfo m_clockInfo; // Used to restore clock state when exiting

	LARGE_INTEGER m_clockFreq;
	LARGE_INTEGER m_startTimestamp;
	double m_currentRunTime;
#endif

AppScene::AppScene()
{
}

AppScene::~AppScene()
{
	// Remember do we need to delete pallette?
	BaseApplication::~BaseApplication();
#ifdef NV_PERF_ENABLE_INSTRUMENTATION
	g_nvperf.Reset();
	nv::perf::D3D11SetDeviceClockState(renderer->getDevice(), g_clockStatus);
#endif
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
	camera->lookSpeed = 0.25f;

	RTViewer = std::make_unique<TextureView::TextureViewer>(renderer->getDevice(), renderer->getDeviceContext(), hwnd, screenWidth, screenHeight);

	RawVoxelModels = std::make_unique<VoxelModelManager>();
	SVOModels = std::make_unique<SVOManager>();
	const int PregeneratedOctreeSize = 200000;
	SVOTraverser = std::make_unique<SVOTraverserShader>(renderer->getDevice(), hwnd, PregeneratedOctreeSize, screenWidth, screenHeight);

	// Load and construct the SVO Models from the files
#if BENCHMARK
	TimeAndMemoryTracker.CaptureStart();
#endif
	SVOHelper::LoadModelInFromFile("dragon", "res/dragon.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("monu1", "res/monu1.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("cars", "res/cars.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("chair", "res/chair.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("doom", "res/doom.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("menger", "res/menger.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("teapot", "res/teapot.vox", *SVOModels, *RawVoxelModels);
	SVOHelper::LoadModelInFromFile("room", "res/room.vox", *SVOModels, *RawVoxelModels);
#if BENCHMARK
	TimeAndMemoryTracker.CaptureEnd();
	PerfTrack.AddPerformanceData("InitLoadAndConstruct", TimeAndMemoryTracker);
#endif

	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("dragon"), SVOModels->GetPalette("dragon"), 0);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("monu1"), SVOModels->GetPalette("monu1"), 1);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("cars"), SVOModels->GetPalette("cars"), 2);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("chair"), SVOModels->GetPalette("chair"), 3);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("doom"), SVOModels->GetPalette("doom"), 4);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("menger"), SVOModels->GetPalette("menger"), 5);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("teapot"), SVOModels->GetPalette("teapot"), 6);
	SVOTraverser->SetVoxelModelAndPalette(renderer->getDeviceContext(), SVOModels->GetSVOModel("room"), SVOModels->GetPalette("room"), 7);

#if BENCHMARK
	ConstructionPerf.SetModelNames(SVOModels->GetModelNames());
	//g_startTicks = GetTickCount64();
	//g_nvperf.InitializeReportGenerator(renderer->getDevice());
	//g_nvperf.SetFrameLevelRangeName("Frame");
	//g_nvperf.SetNumNestingLevels(2);
	////g_nvperf.SetMaxNumRanges(2 + 8); // "Frame" + "Compute"2 + "Render"
	//g_nvperf.outputOptions.directoryName = "HtmlReports\\Tracer";

	//// LoadDriver() must be called first, which is taken care of by InitializeReportGenerator()
	//g_clockStatus = nv::perf::D3D11GetDeviceClockState(renderer->getDevice());
	//nv::perf::D3D11SetDeviceClockState(renderer->getDevice(), NVPW_DEVICE_CLOCK_SETTING_LOCK_TO_RATED_TDP);

	g_nvperf.InitializeReportGenerator(renderer->getDevice());
	g_nvperf.SetFrameLevelRangeName("Frame");
	g_nvperf.SetNumNestingLevels(2);
	g_nvperf.SetMaxNumRanges(2); // "Frame" + "Draw"
	g_nvperf.outputOptions.directoryName = "HtmlReports\\D3D11";

	// LoadDriver() must be called first, which is taken care of by InitializeReportGenerator()
	m_clockInfo = nv::perf::D3D11GetDeviceClockState(renderer->getDevice());
	nv::perf::D3D11SetDeviceClockState(renderer->getDevice(), NVPW_DEVICE_CLOCK_SETTING_LOCK_TO_RATED_TDP);

	QueryPerformanceFrequency(&m_clockFreq);
	QueryPerformanceCounter(&m_startTimestamp);
#endif
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

#ifdef BENCHMARK
	g_nvperf.OnFrameStart(renderer->getDeviceContext());
	g_nvperf.PushRange("Computing Voxel Renderer");
#endif
	RTViewer->GetRenderTexture()->clearRenderTarget(renderer->getDeviceContext(), 1, 0, 0, 1);

	SVOTraverser->SetMatrixBuffer(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, viewMatrix, projectionMatrix);
	SVOTraverser->SetCameraBuffer(renderer->getDeviceContext(), camera->getPosition(), 0);
	SVOTraverser->SetViewModeBuffer(renderer->getDeviceContext(), VoxelViewMode, VoxelViewDepth, IsHeatmapEnabled, -1);
	SVOTraverser->SetTexture(renderer->getDeviceContext(), RTViewer->GetRenderTexture()->getShaderResourceView());

	SVOTraverser->compute(renderer->getDeviceContext(), 74, 40, 1);
	SVOTraverser->Unbind(renderer->getDeviceContext());
#ifdef BENCHMARK
	g_nvperf.PopRange(); // Draw
	g_nvperf.OnFrameEnd();
#endif


#if BENCHMARK
	ConstructionPerf.ContinouslyConstruct(*SVOModels, *RawVoxelModels, *timer, PerfTrack);
#endif // BENCHMARK




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

	//ImGui::ShowDemoWindow();
	//ImGui::Render();
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

	CameraControlsGUI();
	TracerControlsGUI();
#if BENCHMARK
	PerfTrackerGUI();
#endif
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void AppScene::CameraControlsGUI()
{
	if (ImGui::CollapsingHeader("Camera Controls"))
	{
		ImGui::SliderFloat("Camera Speed", &camera->camSpeed, 1.f, 100.0f);
		ImGui::SliderFloat("Camera Sensitivity", &camera->lookSpeed, 0.0f, 2.0f);
	}
}

void AppScene::TracerControlsGUI()
{
	if (ImGui::CollapsingHeader("Tracer Controls"))
	{
		ImGui::Text(ViewModeDisplay.c_str());
		ImGui::SliderInt("Voxel ViewMode", &VoxelViewMode, 0, 4);
		if (VoxelViewMode != 0)
		{
			ImGui::SliderInt("Voxel ViewDepth", &VoxelViewDepth, 0, 7);
		}
		IsHeatmapEnabled = false;
		switch (VoxelViewMode)
		{
		case 0:
			ViewModeDisplay = "Octree Tracer Main";
			break;
		case 1:
			ViewModeDisplay = "Heatmap";
			IsHeatmapEnabled = true;
			break;
		case 2:
			ViewModeDisplay = "Box render at depth";
			break;
		case 3:
			ViewModeDisplay = "Wireframe render at depth";
			break;
		case 4:
			ViewModeDisplay = "Wireframe render above depth";
			break;
		default:
			ViewModeDisplay = "Octree Tracer Main";
			break;
		}
	}
}

void AppScene::PerfTrackerGUI()
{
	if (ImGui::CollapsingHeader("Performance"))
	{
		ConstructionPerf.GUIRender(PerfTrack);

		if (g_nvperf.IsCollectingReport())
		{
			ImGui::Text("Collecting Report");
		}
		else if (g_nvperf.GetInitStatus() == nv::perf::profiler::ReportGeneratorInitStatus::Succeeded)
		{
			ImGui::Text("Profile Successful");
		}

		if (ImGui::Button("NVIDIA Profiler Tracer")) {
			g_nvperf.StartCollectionOnNextFrame();
		}
		if(ImGui::Button("Stop NVIDIA Profiler Tracer")) {
			g_nvperf.Reset();
		}

		if(ImGui::CollapsingHeader("History"))
		{
			auto p = PerfTrack.GetPerformanceDataMap();
			for each(auto var in p)
			{
				if (ImGui::TreeNode(var.first.c_str()))
				{
					ImGui::Text("Time Elapsed/Average Time: %.2f ms", var.second.TimeInMS);
					ImGui::Text("RAM Diff: %.2f MB", var.second.RAMDifference);
					ImGui::Text("V-RAM Diff: %.2f MB", var.second.VRAMDifference);
					ImGui::TreePop();
				}
			}
		}

	}
}
