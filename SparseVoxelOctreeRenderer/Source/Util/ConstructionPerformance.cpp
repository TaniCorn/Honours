#include "ConstructionPerformance.h"

#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_dx11.h"
#include "imGUI/imgui_impl_win32.h"

#include "PerformanceTracker.h"

#include "VoxelModelManager.h"
#include "SVOManager.h"

void ConstructionPerformance::ContinouslyConstruct(SVOManager& SVOModels, VoxelModelManager& RawVoxelModels, Timer& timer, PerformanceTracker& PerfTrack)
{
	if (ShouldConstructSVOContinously)
	{
		ReconstructionTracker.ContinuousCaptureStart();

		for (size_t i = 0; i < ModelNames.size(); i++)
		{
			if (ModelSelection[i] == true)
			{
				SVOModels.CreateAndFlushSVOModel(RawVoxelModels.GetVoxelsFromModel(ModelNames[i]), RawVoxelModels.GetModelDimensions(ModelNames[i]), 1);
			}
		}

		ReconstructionTracker.ContinuousCaptureEnd();
		CaptureTime -= timer.getTime();
		if (CaptureTime <= 0)
		{
			ShouldConstructSVOContinously = false;
			PerfTrack.AddContinousPerformanceData("Continuous Capture", ReconstructionTracker);
		}
	}
}

void ConstructionPerformance::GUIRender(PerformanceTracker& PerfTrack)
{
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Captures"))
	{
		if (ImGui::TreeNode("Capture Models: "))
		{
			for (size_t i = 0; i < ModelNames.size(); i++)
			{
				if (ImGui::Selectable(ModelNames[i].c_str(), ModelSelection[i]))
				{
					ModelSelection[i] ^= 1;
				}
			}
			ImGui::TreePop();
		}
		ImGui::SliderFloat("Time to capture", &CaptureTime, 1.f, 60.0f);
		if (ImGui::Button("Start Continous Caputure"))
		{
			ShouldConstructSVOContinously = true;
			ReconstructionTracker.Reset();
		}
		if (ImGui::Button("Stop Continous Capture"))
		{
			ShouldConstructSVOContinously = false;
			PerfTrack.AddContinousPerformanceData("Continuous Capture", ReconstructionTracker);
		}
	}
}
