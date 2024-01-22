/**
* \brief includes all header files for the whole framework. 
*
* Provides access to the whole framework.
* Use when extending existing classes or when creating your own.
* Includes system, baseApplication, baseShader, baseMesh and all other pre-made meshes, light object and renderTexture object.
*
* \author Paul Robertson
*/

#pragma once
// Include system level headers
#include "System.h"
//#include "D3D.h"
#include "BaseApplication.h"
#include "BaseShader.h"
//#include "TextureManager.h"

// Inlcude geometry headers
#include "BaseMesh.h"
#include "CubeMesh.h"
#include "Model.h"
#include "OrthoMesh.h"
#include "PlaneMesh.h"
#include "PointMesh.h"
#include "QuadMesh.h"
#include "SphereMesh.h"
#include "TessellationMesh.h"
#include "TriangleMesh.h"
#include "AModel.h"

// Include additional rendering headers
#include "Light.h"
#include "RenderTexture.h"
#include "ShadowMap.h"

// imGUI includes
//#include "imgui.h"
//#include "imgui_impl_dx11.h"

//#include "imGUI/imgui.h"
//#include "imGUI/imgui_impl_dx11.h"
//#include "imGUI/imgui_impl_win32.h"
