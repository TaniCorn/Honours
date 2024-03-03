#pragma once

#ifndef VOXELOCTREE_H
#define VOXELOCTREE_H

#include "DXF.h"
struct VoxelOctree
{
	DirectX::XMFLOAT3 TopLeftFrontPosition;
	DirectX::XMFLOAT3 BottomRightBackPosition; 
	DirectX::XMFLOAT3 VoxelPosition;
	UINT32 RGB; 
	UINT32 Depth;
	UINT32 Octants[8];
};
struct Voxel
{
	XMFLOAT3 point;
	UINT32 color;
};

#endif // !VOXELOCTREE_H
