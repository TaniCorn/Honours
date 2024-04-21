#include "NaiveOctree.h"

#include <iostream>
NaiveOctree::NaiveOctree()
{
    topLeftFrontPoint = XMFLOAT3(0, 4, 0);
    bottomRightBackPoint = XMFLOAT3(4, 0,4);
     TLF = nullptr;
     TRF= nullptr;
     BLF= nullptr;
     BRF= nullptr;
    
     TLB= nullptr;
     TRB= nullptr;
     BLB= nullptr;
     BRB= nullptr;
}

NaiveOctree::NaiveOctree(XMFLOAT3 topLeft, XMFLOAT3 bottomRight)
{
    topLeftFrontPoint = topLeft;
    bottomRightBackPoint = bottomRight;
    TLF = nullptr;
    TRF= nullptr;
    BLF= nullptr;
    BRF= nullptr;
    
    TLB= nullptr;
    TRB= nullptr;
    BLB= nullptr;
    BRB= nullptr;
}

NaiveOctree::~NaiveOctree()
{
    if (TLF)
    {
        delete TLF;
    }    
    if (TRF)
    {
        delete TRF;
    }
    if (BLF)
    {
        delete BLF;
    }
    if (BRF)
    {
        delete BRF;
    }
    if (TLB)
    {
        delete TLB;
    }
    if (TRB)
    {
        delete TRB;
    }
    if (BLB)
    {
        delete BLB;
    }
    if (BRB)
    {
        delete BRB;
    }
}

void NaiveOctree::insert(Voxel* vox, const float sizeOfVoxels)
{

    // If the point is out of bounds
    if (!inBoundary(vox->point)) {
        inBoundary(vox->point);
        cout << "Point is out of bound" << endl;
        return;
    }
    // If the point already exists in the octree
    if (search(vox->point) != nullptr) {
        std::cout << "Voxel already exists in octree" << std::endl;
        return;
    }
 
        
    if (abs(topLeftFrontPoint.x - bottomRightBackPoint.x) <= sizeOfVoxels
           && abs(topLeftFrontPoint.y - bottomRightBackPoint.y) <= sizeOfVoxels
           && abs(topLeftFrontPoint.z - bottomRightBackPoint.z) <= sizeOfVoxels) 
    {
        storedVoxel = vox;
        return;
    }
        // Binary search to insert the point
    float midx = ((topLeftFrontPoint.x
                 + bottomRightBackPoint.x)/ 2.0f);
    float midy = ((topLeftFrontPoint.y
                + bottomRightBackPoint.y)/ 2.0f);
    float midz = ((topLeftFrontPoint.z
                + bottomRightBackPoint.z)/ 2.0f);

    //X is for left and right
    //Z is for front and back
    //Y is for top and down
    //T = TLF ,Y, BRB ,mY,
    //B = TLF ,mY, BRB ,Y,
    //R = TLF mX,, BRB X,,
    //L = TLF X,, BRB mX,,
    XMFLOAT3 point = vox->point;
        if (point.z <= midz) {
            if (point.y > midy) {
                if (point.x <= midx)
                {
                    if (TLF == nullptr)
                    {
                        TLF = new NaiveOctree(topLeftFrontPoint, XMFLOAT3(midx,midy,midz));
                        TLF->depth = depth  +1;
                    }
                    return TLF->insert(vox, sizeOfVoxels);
                }
                else
                {
                    if (TRF == nullptr)
                    {
                        TRF = new NaiveOctree(XMFLOAT3(midx, topLeftFrontPoint.y, topLeftFrontPoint.z), XMFLOAT3(bottomRightBackPoint.x,midy,midz));
                        TRF->depth = depth  +1;
                    }
                    return TRF->insert(vox, sizeOfVoxels);
                    
                }
            }
            else {
                if (point.x <= midx)
                {
                    if (BLF == nullptr)
                    {
                        BLF = new NaiveOctree(XMFLOAT3(topLeftFrontPoint.x, midy, topLeftFrontPoint.z), XMFLOAT3(midx,bottomRightBackPoint.y,midz));
                        BLF->depth = depth  +1;

                    }
                    return BLF->insert(vox, sizeOfVoxels);
                }
                else
                {
                    if (BRF == nullptr)
                    {
                        BRF = new NaiveOctree(XMFLOAT3(midx, midy, topLeftFrontPoint.z), XMFLOAT3(bottomRightBackPoint.x, bottomRightBackPoint.y, midz));
                        BRF->depth = depth  +1;
                    }
                    return BRF->insert(vox, sizeOfVoxels);
                    

                }
            }
        }
        else {
            if (point.y > midy) {
                if (point.x <= midx)
                {
                    if (TLB == nullptr)
                    {
                        TLB = new NaiveOctree(XMFLOAT3(topLeftFrontPoint.x, topLeftFrontPoint.y, midz), XMFLOAT3(midx,midy,bottomRightBackPoint.z));
                        TLB->depth = depth  +1;

                    }
                    return TLB->insert(vox, sizeOfVoxels);
                    
                }
                else
                {
                    if (TRB == nullptr)
                    {
                        TRB = new NaiveOctree(XMFLOAT3(midx, topLeftFrontPoint.y, midz), XMFLOAT3(bottomRightBackPoint.x,midy,bottomRightBackPoint.z));
                        TRB->depth = depth  +1;

                    }
                    return TRB->insert(vox, sizeOfVoxels);
                    
                }
                
            }
            else {
                if (point.x <= midx)
                {
                    if (BLB == nullptr)
                    {
                        BLB = new NaiveOctree(XMFLOAT3(topLeftFrontPoint.x, midy, midz), XMFLOAT3(midx,bottomRightBackPoint.y,bottomRightBackPoint.z));
                        BLB->depth = depth  +1;

                    }
                    return BLB->insert(vox, sizeOfVoxels);
                }
                else
                {
                    if (BRB == nullptr)
                    {
                        BRB = new NaiveOctree(XMFLOAT3(midx, midy, midz), XMFLOAT3(bottomRightBackPoint.x,bottomRightBackPoint.y,bottomRightBackPoint.z));
                        BRB->depth = depth  +1;

                    }
                    return BRB->insert(vox, sizeOfVoxels);
                }
            }
        }
 
        
}

Voxel* NaiveOctree::search(XMFLOAT3 point)
{
    // If point is out of bound
        if (!inBoundary(point))
            return nullptr;
 
        // Otherwise perform binary search
        // for each ordinate
        float midx = ((topLeftFrontPoint.x
                    + bottomRightBackPoint.x)/ 2.0f);
        float midy = ((topLeftFrontPoint.y
                    + bottomRightBackPoint.y)/ 2.0f);
        float midz = ((topLeftFrontPoint.z
                    + bottomRightBackPoint.z)/ 2.0f);
 
        int pos = -1;
 
        // Deciding the position
        // where to move
        if (point.x <= midx) {
            if (point.y > midy) {
                if (point.z <= midz)
                {
                    if (TLF == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return TLF->search(point);
                }
                else
                {
                    if (TRF == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return TRF->search(point);
                }
            }
            else {
                if (point.z <= midz)
                {
                    if (BLF == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return BLF->search(point);
                }
                else
                {
                    if (BRF == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return BRF->search(point);

                }
            }
        }
        else {
            if (point.y > midy) {
                if (point.z <= midz)
                {
                    if (TLB == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return TLB->search(point);
                    
                }
                else
                {
                    if (TRB == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return TRB->search(point);
                    
                }
                
            }
            else {
                if (point.z <= midz)
                {
                    if (BLB == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return BLB->search(point);
                }
                else
                {
                    if (BRB == nullptr)
                    {
                        if (this->storedVoxel == nullptr)
                        {
                            return nullptr;
                        }else
                        {
                            return this->storedVoxel;
                        }
                    }
                    return BRB->search(point);
                }
            }
        }
}

//AABB collision detection
bool NaiveOctree::inBoundary(XMFLOAT3 point)
{
    bool xCheck = point.x >= topLeftFrontPoint.x && point.x <= bottomRightBackPoint.x;
    bool yCheck = point.y >= bottomRightBackPoint.y && point.y <= topLeftFrontPoint.y;
    bool zCheck = point.z >= topLeftFrontPoint.z && point.z <= bottomRightBackPoint.z;

    return xCheck && yCheck && zCheck;

}


int NaiveOctree::bigger(int a, int b)
{
    return (a > b) ? a : b;
}
int NaiveOctree::getOctantAmount(NaiveOctree* oc)
{
    int octants = 1;

    if (oc->TLF != nullptr)
    {
        octants += getOctantAmount(oc->TLF);
    }
    if (oc->TLB != nullptr)
    {
        octants += getOctantAmount(oc->TLB);
    }
    if (oc->TRF != nullptr)
    {
        octants += getOctantAmount(oc->TRF);
    }
    if (oc->TRB != nullptr)
    {
        octants += getOctantAmount(oc->TRB);
    }
    if (oc->BLB != nullptr)
    {
        octants += getOctantAmount(oc->BLB);
    }
    if (oc->BLF != nullptr)
    {
        octants += getOctantAmount(oc->BLF);
    }
    if (oc->BRB != nullptr)
    {
        octants += getOctantAmount(oc->BRB);
    }
    if (oc->BRF != nullptr)
    {
        octants += getOctantAmount(oc->BRF);
    }
    return octants;
}
int NaiveOctree::getMaxDepth(NaiveOctree* oc, int depth)
{
    depth++;

    if (oc->TLF != nullptr)
    {
        int newDepth = getMaxDepth(oc->TLF, depth);
        depth = bigger(depth, newDepth);
    }
    if (oc->TLB != nullptr)
    {
        int newDepth = getMaxDepth(oc->TLB, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->TRF != nullptr)
    {
        int newDepth = getMaxDepth(oc->TRF, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->TRB != nullptr)
    {
        int newDepth = getMaxDepth(oc->TRB, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->BLB != nullptr)
    {
        int newDepth = getMaxDepth(oc->BLB, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->BLF != nullptr)
    {
        int newDepth = getMaxDepth(oc->BLF, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->BRB != nullptr)
    {
        int newDepth = getMaxDepth(oc->BRB, depth);
        depth = bigger(depth, newDepth);
    }    
    if (oc->BRF != nullptr)
    {
        int newDepth = getMaxDepth(oc->BRF, depth);
        depth = bigger(depth, newDepth);
    }
    return depth;
}

void NaiveCPUOctree::clear()
{
    delete octree;
    for (auto vox : voxels)
    {
        delete vox;
    }
    voxels.clear();
}
