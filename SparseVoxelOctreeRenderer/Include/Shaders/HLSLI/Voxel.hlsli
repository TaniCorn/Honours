#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7

//Reminder: Make sure this matches VoxelOctree.h

struct VoxelOctree
{
    float3 TopLeftFrontPosition;
    float3 BottomRightBackPosition;
    float3 VoxelPosition;
    uint RGB;
    uint Depth;
    uint Octants[8];
};
struct Voxel
{
    float3 voxPosition;
    uint color;
};
