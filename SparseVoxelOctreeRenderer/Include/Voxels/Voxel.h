#pragma once

#ifndef VOXEL_H
#define VOXEL_H

#include "DXF.h"

struct Voxel
{
	XMFLOAT3 Point;
	UINT32 Color;
};
namespace OctreeGPU
{
	struct SVOSRVRepresentation
	{
		DirectX::XMFLOAT3 TopLeftFrontPosition;
		DirectX::XMFLOAT3 BottomRightBackPosition;
		DirectX::XMFLOAT3 VoxelPosition;
		UINT32 RGB;
		UINT32 Depth;
		UINT32 Octants[8];
	};
}


#endif