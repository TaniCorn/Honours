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
float UnpackVoxelColor(uint color, int rgba)
{
    //Unpacks a uint color to individual channels rgba
    const float coefficient = 255.f;
    switch (rgba)
    {
        case 0:
            //return float((color & 0x000000ff)) * coefficient;
            return float((color & 0x000000ff)) / coefficient;
        case 1:
            //return float((color & 0x0000ff00)) * coefficient;
            return float(((color >> 8) & 0x000000ff)) / coefficient;
        case 2:
            //return float((color & 0x00ff0000)) * coefficient;
            return float(((color >> 16) & 0x000000ff)) / coefficient;
        case 3:
            //return float((color & 0xff000000)) * coefficient;
            return 1.0f;
            return float(((color >> 24) & 0x000000ff)) / coefficient;
        default:
            return 1;
    }
}