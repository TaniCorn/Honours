struct VoxelOctree
{
    float3 TopLeftFrontPosition; // 4bytes*3 = 12bytes
    float3 BottomRightBackPosition; //12 bytes
    uint RGB; //12 bytes
    //uint TLF; //4bytes
    //uint TRF;
    //uint BLF;
    //uint BRF;
    
    //uint TLB;
    //uint TRB;
    //uint BLB;
    //uint BRB;
    uint Octants[8];
    //4bytes * 8 = 32bytes
    //32 + 12 + 12 + 12 = 68bytes per octree
};
struct Voxel
{
    float3 voxPosition;
    uint color;
};
#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7

StructuredBuffer<Voxel> inputOctree : register(t0);
RWStructuredBuffer<VoxelOctree> outputOctree : register(u0);
static uint MaxStride = 0;

//AABB collisiondetection
bool inBoundary(float3 voxPoint, float3 topLeftFrontPoint, float3 bottomRightBackPoint)
{
    return (voxPoint.x > topLeftFrontPoint.x && voxPoint.x <= bottomRightBackPoint.x &&
        voxPoint.y > bottomRightBackPoint.y && voxPoint.y <= topLeftFrontPoint.y &&
        voxPoint.z > topLeftFrontPoint.z && voxPoint.z <= bottomRightBackPoint.z);

}
void insert(VoxelOctree CurrentOctree, Voxel vox, RWStructuredBuffer<VoxelOctree> voxTree, uint CurrentStride)
{
    
    //float3 TLFBound = float3(CurrentOctree.TopLeftFrontPosition.x, CurrentOctree.TopLeftFrontPosition.y, CurrentOctree.TopLeftFrontPosition.z);
    //float3 BRBBound = float3(CurrentOctree.BottomRightBackPosition.x, CurrentOctree.BottomRightBackPosition.y, CurrentOctree.BottomRightBackPosition.z);
    // // If the point is out of bounds
    //if (!inBoundary(vox.voxPosition, TLFBound, BRBBound))
    //{
    //    return;
    //}

    //float minSize = 1.0f;
    //    //Could replace with depth
    //if (abs(TLFBound.r - BRBBound.r) <= minSize
    //       && abs(TLFBound.g - BRBBound.g) <= minSize
    //       && abs(TLFBound.b - BRBBound.b) <= minSize)
    //{
    //    if (CurrentOctree.RGB == 0)
    //    {
    //        CurrentOctree.RGB = vox.color;
            
    //    }
    //    //points.insert(vox);
    //    return;
    //}
    //    // Binary search to insert the point
    //float midx = ((TLFBound.r
    //             + BRBBound.r) / 2.0f);
    //float midy = ((TLFBound.g
    //            + BRBBound.g) / 2.0f);
    //float midz = ((TLFBound.b
    //            + BRBBound.b) / 2.0f);

    ////X is for left and right
    ////Z is for front and back
    ////Y is for top and down
    ////T = TLF ,Y, BRB ,mY,
    ////B = TLF ,mY, BRB ,Y,
    ////R = TLF mX,, BRB X,,
    ////L = TLF X,, BRB mX,,


    //float3 position = vox.voxPosition;
    //if (position.b <= midz)
    //{
    //    if (position.g > midy)
    //    {
    //        if (position.r <= midx)
    //        {
    //            if (CurrentOctree.TLF == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].TLF = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = voxTree[CurrentStride].TopLeftFrontPosition;
    //                voxTree[MaxStride].BottomRightBackPosition = float3(midx, midy, midz);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].TLF], vox, voxTree, voxTree[CurrentStride].TLF);
    //            return;
    //        }
    //        else
    //        {
    //            if (CurrentOctree.TRF == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].TRF = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(midx, voxTree[CurrentStride].TopLeftFrontPosition.g, voxTree[CurrentStride].TopLeftFrontPosition.b);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(BRBBound.r, midy, midz);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].TRF], vox, voxTree, voxTree[CurrentStride].TRF);
    //            return;
    //        }
    //    }
    //    else
    //    {
    //        if (position.r <= midx)
    //        {
    //            if (CurrentOctree.BLF == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].BLF = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(TLFBound.r, midy, TLFBound.b);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(midx, BRBBound.g, midz);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].BLF], vox, voxTree, voxTree[CurrentStride].BLF);
    //            return;
    //        }
    //        else
    //        {
    //            if (CurrentOctree.BRF == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].BRF = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(midx, midy, TLFBound.b);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(BRBBound.r, BRBBound.g, midz);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].BRF], vox, voxTree, voxTree[CurrentStride].BRF);
    //            return;
    //        }
    //    }
    //}
    //else
    //{
    //    if (position.g > midy)
    //    {
    //        if (position.r <= midx)
    //        {
    //            if (CurrentOctree.TLB == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].TLB = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(TLFBound.r, TLFBound.g, midz);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(midx, midy, BRBBound.b);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].TLB], vox, voxTree, voxTree[CurrentStride].TLB);
    //            return;
    //        }
    //        else
    //        {
    //            if (CurrentOctree.TRB == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].TRB = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(midx, TLFBound.g, midz);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(BRBBound.r, midy, BRBBound.b);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].TRB], vox, voxTree, voxTree[CurrentStride].TRB);
    //            return;
    //        }
                
    //    }
    //    else
    //    {
    //        if (position.r <= midx)
    //        {
    //            if (CurrentOctree.BLB == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].BLB = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(TLFBound.r, midy, midz);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(midx, BRBBound.g, BRBBound.b);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].BLB], vox, voxTree, voxTree[CurrentStride].BLB);
    //            return;
    //        }
    //        else
    //        {
    //            if (CurrentOctree.BRB == 0)
    //            {
    //                MaxStride++;
    //                voxTree[CurrentStride].BRB = MaxStride;
    //                voxTree[MaxStride].TopLeftFrontPosition = float3(midx, midy, midz);
    //                voxTree[MaxStride].BottomRightBackPosition = float3(BRBBound.r, BRBBound.g, BRBBound.b);
    //            }
    //            insert(voxTree[voxTree[CurrentStride].BRB], vox, voxTree, voxTree[CurrentStride].BRB);
    //            return;
    //        }
    //    }
    //}
}

int DetermineOctant(VoxelOctree CurrentOctree, float3 Position)
{
    float3 TLFBound = float3(CurrentOctree.TopLeftFrontPosition.x, CurrentOctree.TopLeftFrontPosition.y, CurrentOctree.TopLeftFrontPosition.z);
    float3 BRBBound = float3(CurrentOctree.BottomRightBackPosition.x, CurrentOctree.BottomRightBackPosition.y, CurrentOctree.BottomRightBackPosition.z);
     // If the point is out of bounds
    if (!inBoundary(Position, TLFBound, BRBBound))
    {
        return -1;
    }

    float midx = ((TLFBound.x
                 + BRBBound.x) / 2.0f);
    float midy = ((TLFBound.y
                + BRBBound.y) / 2.0f);
    float midz = ((TLFBound.z
                + BRBBound.z) / 2.0f);

    float3 position = Position;
    if (position.z <= midz)
    {
        if (position.y > midy)
        {
            if (position.x <= midx)
            {
                return TLF;
            }
            else
            {
                return TRF;
            }
        }
        else
        {
            if (position.x <= midx)
            {
                return BLF;
            }
            else
            {
                return BRF;
            }
        }
    }
    else
    {
        if (position.y > midy)
        {
            if (position.x <= midx)
            {
                return TLB;
            }
            else
            {
                return TRB;
            }
                
        }
        else
        {
            if (position.x <= midx)
            {
                return BLB;
            }
            else
            {
                return BRB;
            }
        }
    }
}

//#define MAX_STACK_SIZE 1024 // Define maximum stack size
#define MIN_SIZE 0.25f
#define MAX_DEPTH 20
//// Simulated stack structure
//struct Stack
//{
//    VoxelOctree data[MAX_STACK_SIZE];
//    int top;
//};

//// Function to push a node onto the stack
//void Push(Stack stack, VoxelOctree node)
//{
//    stack.data[stack.top++] = node;
//}

//// Function to pop a node from the stack
//VoxelOctree Pop(Stack stack)
//{
//    return stack.data[--stack.top];
//}


float3 GetTLFBound(VoxelOctree CurrentOctree, int OctantIndex)
{
    float3 TLFBound = float3(CurrentOctree.TopLeftFrontPosition.x, CurrentOctree.TopLeftFrontPosition.y, CurrentOctree.TopLeftFrontPosition.z);
    float3 BRBBound = float3(CurrentOctree.BottomRightBackPosition.x, CurrentOctree.BottomRightBackPosition.y, CurrentOctree.BottomRightBackPosition.z);
    
    float midx = ((TLFBound.r
                 + BRBBound.r) / 2.0f);
    float midy = ((TLFBound.g
                + BRBBound.g) / 2.0f);
    float midz = ((TLFBound.b
                + BRBBound.b) / 2.0f);
    
    switch (OctantIndex)
    {
        case TLF:
            return TLFBound;
        case TRF:
            return float3(midx, TLFBound.g, TLFBound.b);
        case BLF:
            return float3(TLFBound.r, midy, TLFBound.b);
        case BRF:
            return float3(midx, midy, TLFBound.b);
        case TLB:
            return float3(TLFBound.r, TLFBound.g, midz);
        case TRB:
            return float3(midx, TLFBound.g, midz);
        case BLB:
            return float3(TLFBound.r, midy, midz);
        case BRB:
            return float3(midx, midy, midz);
    }
    return float3(0, 0, 0);

}
float3 GetBRBBound(VoxelOctree CurrentOctree, int OctantIndex)
{
    float3 TLFBound = float3(CurrentOctree.TopLeftFrontPosition.x, CurrentOctree.TopLeftFrontPosition.y, CurrentOctree.TopLeftFrontPosition.z);
    float3 BRBBound = float3(CurrentOctree.BottomRightBackPosition.x, CurrentOctree.BottomRightBackPosition.y, CurrentOctree.BottomRightBackPosition.z);
    
    float midx = ((TLFBound.r
                 + BRBBound.r) / 2.0f);
    float midy = ((TLFBound.g
                + BRBBound.g) / 2.0f);
    float midz = ((TLFBound.b
                + BRBBound.b) / 2.0f);
    
    switch (OctantIndex)
    {
        case TLF:
            return float3(midx, midy, midz);;
        case TRF:
            return float3(BRBBound.r, midy, midz);
        case BLF:
            return float3(midx, BRBBound.g, midz);
        case BRF:
            return float3(BRBBound.r, BRBBound.g, midz);
        case TLB:
            return float3(midx, midy, BRBBound.b);
        case TRB:
            return float3(BRBBound.r, midy, BRBBound.b);
        case BLB:
            return float3(midx, BRBBound.g, BRBBound.b);
        case BRB:
            return BRBBound;
    }
    return float3(0, 0, 0);

}

bool ShouldSubdivide(VoxelOctree CurrentOctree, uint Depth)
{
    float3 TLFBound = float3(CurrentOctree.TopLeftFrontPosition.x, CurrentOctree.TopLeftFrontPosition.y, CurrentOctree.TopLeftFrontPosition.z);
    float3 BRBBound = float3(CurrentOctree.BottomRightBackPosition.x, CurrentOctree.BottomRightBackPosition.y, CurrentOctree.BottomRightBackPosition.z);
        
    if (Depth >= MAX_DEPTH)
    {
        return false;
    }
    //Could replace with depth
    if (abs(TLFBound.r - BRBBound.r) <= MIN_SIZE
           && abs(TLFBound.g - BRBBound.g) <= MIN_SIZE
           && abs(TLFBound.b - BRBBound.b) <= MIN_SIZE)
    {
        return false;
    }
    return true;
}

void InsertVoxel(VoxelOctree RootNode, Voxel Vox, RWStructuredBuffer<VoxelOctree> Octree)
{
    VoxelOctree currentNode = RootNode;
    uint Depth = 0;
    uint currentIndex = 0;
    // Traverse the octree to find the leaf node
    for (int i = 0; i < MAX_DEPTH; i++)
    {
        if (!ShouldSubdivide(currentNode, Depth))
        {
            Octree[currentIndex].RGB = Vox.color;
            return;
        }
        Depth++;
        
        // Determine the octant containing the voxel position
        int octantIndex = DetermineOctant(currentNode, Vox.voxPosition);
        //Octree[currentIndex].RGB = octantIndex; //DEBUGGING PURPOSES
        if (octantIndex < 0)//Outside of octant
        {
            return;
        }
        //If octant hasn't been created yet, allocate at the end of the buffer
        if (currentNode.Octants[octantIndex] == 0)
        {
            MaxStride++;
            //currentNode.Octants[octantIndex] = MaxStride;
            Octree[currentIndex].Octants[octantIndex] = MaxStride;
            Octree[MaxStride].BottomRightBackPosition = GetBRBBound(currentNode, octantIndex);
            Octree[MaxStride].TopLeftFrontPosition = GetTLFBound(currentNode, octantIndex);
        }
        uint strideIndex = currentNode.Octants[octantIndex];
        currentIndex = strideIndex;
            // Move to the child node in the appropriate octant
        currentNode = Octree[strideIndex];

    }
    Octree[currentIndex].RGB = Vox.color;
    
}

[numthreads(1, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
    uint numVoxels = 0;
    uint numStride = 0;
    inputOctree.GetDimensions(numVoxels, numStride);
    if (dispatchThreadID.x > numVoxels)
    {
        return;
    }
    outputOctree[0].TopLeftFrontPosition = float3(0, 1, 0);
    outputOctree[0].BottomRightBackPosition = float3(1, 0, 1);
    for (int i = 0; i < numVoxels; i++)
    {
        InsertVoxel(outputOctree[0], inputOctree[i], outputOctree);
        
    }
    outputOctree[0].RGB = numVoxels;
    //outputOctree[0].RGB = MaxStride;
    // Initialize stack
    //Stack stack;
    //stack.top = 0;
    //Push(stack, outputOctree[0]);

    //// Iterative depth-first traversal
    //while (stack.top > 0)
    //{
    //    VoxelOctree node = Pop(stack);

    //    // Determine occupancy and other node properties

    //    // Subdivide node if necessary and push child nodes onto stack
    //    if (ShouldSubdivide(node))
    //    {
    //        // Subdivide node and push child nodes onto stack
    //        for (int i = 0; i < 8; ++i)
    //        {
    //            OctreeNode childNode = SubdivideNode(node, i);
    //            Push(stack, childNode);
    //        }
    //    }
    //}
}