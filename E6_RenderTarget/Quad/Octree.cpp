#include "Octree.h"

#include <iostream>

Octree::Octree()
{
    topLeftFrontPoint = OctPoint(0, 4, 0);
    bottomRightBackPoint = OctPoint(4, 0,4);
     TLF = nullptr;
     TRF= nullptr;
     BLF= nullptr;
     BRF= nullptr;
    
     TLB= nullptr;
     TRB= nullptr;
     BLB= nullptr;
     BRB= nullptr;
}

Octree::Octree(OctPoint topLeft, OctPoint bottomRight)
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

Octree::~Octree()
{
}

void Octree::insert(OctVoxel* vox)
{

    float minSize = 0.5f;
    // If the point is out of bounds
    if (!inBoundary(vox->point)) {
        cout << "Point is out of bound" << endl;
        return;
    }
    // If the point already exists in the octree
        if (search(vox->point) != nullptr) {
            std::cout << "Voxel already exists in octree" << std::endl;
            return;
        }
 
        
    if (abs(topLeftFrontPoint.GetX() - bottomRightBackPoint.GetX()) <= minSize
           && abs(topLeftFrontPoint.GetY() - bottomRightBackPoint.GetY()) <= minSize
           && abs(topLeftFrontPoint.GetZ() - bottomRightBackPoint.GetZ()) <= minSize) 
    {
        storedVoxel = vox;
        return;
    }
        // Binary search to insert the point
    float midx = ((topLeftFrontPoint.GetX()
                 + bottomRightBackPoint.GetX())/ 2.0f);
    float midy = ((topLeftFrontPoint.GetY()
                + bottomRightBackPoint.GetY())/ 2.0f);
    float midz = ((topLeftFrontPoint.GetZ()
                + bottomRightBackPoint.GetZ())/ 2.0f);


    OctPoint point = vox->point;
        if (point.GetX() <= midx) {
            if (point.GetY() > midy) {
                if (point.GetZ() <= midz)
                {
                    if (TLF == nullptr)
                    {
                        TLF = new Octree(topLeftFrontPoint, OctPoint(midx,midy,midz));
                        TLF->depth = depth  +1;
                    }
                    return TLF->insert(vox);
                }
                else
                {
                    if (TRF == nullptr)
                    {
                        TRF = new Octree(OctPoint(midx, topLeftFrontPoint.GetY(), topLeftFrontPoint.GetZ()), OctPoint(bottomRightBackPoint.GetX(),midy,midz));
                        TRF->depth = depth  +1;

                    }
                    return TRF->insert(vox);
                    
                }
            }
            else {
                if (point.GetZ() <= midz)
                {
                    if (BLF == nullptr)
                    {
                        BLF = new Octree(OctPoint(topLeftFrontPoint.GetX(), midy, topLeftFrontPoint.GetZ()), OctPoint(midx,bottomRightBackPoint.GetY(),midz));
                        BLF->depth = depth  +1;

                    }
                    return BLF->insert(vox);
                }
                else
                {
                    if (BRF == nullptr)
                    {
                        BRF = new Octree(OctPoint(midx, midy, topLeftFrontPoint.GetZ()), OctPoint(midx,bottomRightBackPoint.GetY(),midz));
                        BRF->depth = depth  +1;

                    }
                    return BRF->insert(vox);
                    

                }
            }
        }
        else {
            if (point.GetY() > midy) {
                if (point.GetZ() <= midz)
                {
                    if (TLB == nullptr)
                    {
                        TLB = new Octree(OctPoint(topLeftFrontPoint.GetX(), topLeftFrontPoint.GetY(), midz), OctPoint(midx,midy,bottomRightBackPoint.GetZ()));
                        TLB->depth = depth  +1;

                    }
                    return TLB->insert(vox);
                    
                }
                else
                {
                    if (TRB == nullptr)
                    {
                        TRB = new Octree(OctPoint(midx, topLeftFrontPoint.GetY(), midz), OctPoint(bottomRightBackPoint.GetX(),midy,bottomRightBackPoint.GetZ()));
                        TRB->depth = depth  +1;

                    }
                    return TRB->insert(vox);
                    
                }
                
            }
            else {
                if (point.GetZ() <= midz)
                {
                    if (BLB == nullptr)
                    {
                        BLB = new Octree(OctPoint(topLeftFrontPoint.GetX(), midy, midz), OctPoint(midx,bottomRightBackPoint.GetY(),bottomRightBackPoint.GetZ()));
                        BLB->depth = depth  +1;

                    }
                    return BLB->insert(vox);
                }
                else
                {
                    if (BRB == nullptr)
                    {
                        BRB = new Octree(OctPoint(midx, midy, midz), OctPoint(bottomRightBackPoint.GetX(),bottomRightBackPoint.GetY(),bottomRightBackPoint.GetZ()));
                        BRB->depth = depth  +1;

                    }
                    return BRB->insert(vox);
                }
            }
        }
 
        
}

OctVoxel* Octree::search(OctPoint point)
{
    // If point is out of bound
        if (!inBoundary(point))
            return nullptr;
 
        // Otherwise perform binary search
        // for each ordinate
        float midx = ((topLeftFrontPoint.GetX()
                    + bottomRightBackPoint.GetX())/ 2.0f);
        float midy = ((topLeftFrontPoint.GetY()
                    + bottomRightBackPoint.GetY())/ 2.0f);
        float midz = ((topLeftFrontPoint.GetZ()
                    + bottomRightBackPoint.GetZ())/ 2.0f);
 
        int pos = -1;
 
        // Deciding the position
        // where to move
        if (point.GetX() <= midx) {
            if (point.GetY() > midy) {
                if (point.GetZ() <= midz)
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
                if (point.GetZ() <= midz)
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
                    return BLB->search(point);

                }
            }
        }
        else {
            if (point.GetY() > midy) {
                if (point.GetZ() <= midz)
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
                if (point.GetZ() <= midz)
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
bool Octree::inBoundary(OctPoint point)
{
    return (point.GetX() > topLeftFrontPoint.GetX() && point.GetX() <= bottomRightBackPoint.GetX() &&
        point.GetY() > bottomRightBackPoint.GetY() && point.GetY() <= topLeftFrontPoint.GetY() &&
        point.GetZ() > topLeftFrontPoint.GetZ() && point.GetZ() <= bottomRightBackPoint.GetZ());

}