#pragma once

#ifndef VOXELOCTREE_H
#define VOXELOCTREE_H

#include "DXF.h"
//Reminder: Make sure this matches VoxelOctree.hlsli
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

//// Expands a 10-bit integer into 30 bits
//// by inserting 2 zeros after each bit.
//unsigned int expandBits(unsigned int v)
//{
//    v = (v * 0x00010001u) & 0xFF0000FFu;
//    v = (v * 0x00000101u) & 0x0F00F00Fu;
//    v = (v * 0x00000011u) & 0xC30C30C3u;
//    v = (v * 0x00000005u) & 0x49249249u;
//    return v;
//}
//
//// Calculates a 30-bit Morton code for the
//// given 3D point located within the unit cube [0,1].
//unsigned int morton3D(float x, float y, float z)
//{
//    x = min(max(x * 1024.0f, 0.0f), 1023.0f);
//    y = min(max(y * 1024.0f, 0.0f), 1023.0f);
//    z = min(max(z * 1024.0f, 0.0f), 1023.0f);
//    unsigned int xx = expandBits((unsigned int)x);
//    unsigned int yy = expandBits((unsigned int)y);
//    unsigned int zz = expandBits((unsigned int)z);
//    return xx * 4 + yy * 2 + zz;
//}
#endif // !VOXELOCTREE_H
