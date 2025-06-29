#pragma once

#ifndef SVOCONSTRUCTORCPU_H
#define SVOCONSTRUCTORCPU_H

#include "DXF.h"
#include <set>
#include "Voxel.h"

#define MAX_DEPTH 20

namespace OctreeGPU
{

    //NOTE on Coordinate system:
    /*
	* The Octree is structured in a 3D space with the following coordinate system:
    * X for left and right
	* Y for top and down
    Z for front and back
    * 
    */
    //// NOTE on Stride:
    /*
    * A quick way to understand how the octree is structured with stride and octant indices :
	* SVOContructorCPU contains a large array of OctreeGPURepresentation objects, where index 0 is the root octant
	* All Child Octants are stored in the array, at seemingly random indices.
	* Each Octant (OctreeGPURepresentation) contains an array of 8 UINT32 called ChildOctantStride, 
	     These are the array index or stride of where the child octants are stored in the Octree array.
	* For example, the first Oct(index 0) has its TLF child octant at index 1 
            and its BLB child octant at index 256
	* So to access the BLB child octant of the root octant, you would do:
        OctreeGPURepresentation Root = Octree[0];
		OctreeGPURepresentation BLBChild = Octree[Root.GetOctantStride(BLB)];
    */ 
    /// 


	//TODO: Convert integer accesses for hte octants into an enum class for better readability


    /// <summary>
    /// Offset based Octree implementation.
    /// This represents an indivual octant.
    /// </summary>
    class OctreeGPURepresentation
    {
    public:
        OctreeGPURepresentation();
		friend class SVOConstructorCPU;

        // Getters
		XMFLOAT3 GetTopLeftFrontPosition() const { return TopLeftFrontPosition; }
		XMFLOAT3 GetBottomRightBackPosition() const { return BottomRightBackPosition; }
		XMFLOAT3 GetVoxelPosition() const { return VoxelPosition; }
		UINT32 GetColorIndex() const { return ColorIndex; }
		UINT32 GetDepth() const { return Depth; }
		UINT32 GetOctantStride(int octantIndex) const { return ChildOctantStride[octantIndex]; }

    private:
        XMFLOAT3 GetChildTLFBound(const UINT32 octant) const;
        XMFLOAT3 GetChildBRBBound(const UINT32 octant) const;
        int DetermineOctant(XMFLOAT3 Position) const;
        bool ShouldSubdivide(int size) const;
        bool IsInBoundary(const XMFLOAT3 Position) const;

        XMFLOAT3 TopLeftFrontPosition;
        XMFLOAT3 BottomRightBackPosition;
        XMFLOAT3 VoxelPosition;
        UINT32 ColorIndex;
        UINT32 Depth;
		UINT32 ChildOctantStride[8]; // 0 value means not allocated, otherwise it points to the octant in the Octree array
    };

    /// <summary>
    /// Constructs an SVO from scratch ready to translate the SRV format for usage in a compute shader.
    /// </summary>
    class SVOConstructorCPU
    {
    public:
        SVOConstructorCPU(int amountOfVoxels, UINT minVoxelSize = 1);
        /// <summary>
        /// Inserts a voxel into the Octree via a depth first insertion.
        /// </summary>
        void InsertVoxel(Voxel Vox);
        /// <summary>
        /// Resets the Octree to an empty state with existing size. Clears voxels too.
        /// </summary>
        void Clear();

        const OctreeGPURepresentation& GetOctant(const UINT32& Stride) const;
        int GetMaxStride() const { return MaxStride; }

    private:
        UINT MinSize;
		UINT32 MaxStride; // The current maximum length of the Octree, used to determine the next available index for a new octant
        std::unique_ptr<OctreeGPURepresentation[]> Octree;
		std::unique_ptr<std::vector<Voxel>> Voxels;
    };
}


#undef MAX_DEPTH
#endif // !SVOCONSTRUCTORCPU_H



