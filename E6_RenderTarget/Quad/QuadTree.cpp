#include "QuadTree.h"

QuadTree::QuadTree()
{
    topLeftPoint = QuadPoint(-1, 1);
    bottomRightPoint = QuadPoint(0, 0);
    topLeftQuad = nullptr;
    topRightQuad = nullptr;
    bottomRightQuad = nullptr;
    bottomLeftQuad = nullptr;
}

QuadTree::QuadTree(QuadPoint topLeft, QuadPoint bottomRight)
{
    topLeftPoint = topLeft;
    bottomRightPoint = bottomRight;
    topLeftQuad = nullptr;
    topRightQuad = nullptr;
    bottomRightQuad = nullptr;
    bottomLeftQuad = nullptr;
}

QuadTree::~QuadTree()
{
}

void QuadTree::insert(QuadVoxel* vox)
{
    if (vox == nullptr)
        return;

    // Current quad cannot contain it
    if (!inBoundary(vox->point))
        return;

    // We are at a quad of unit area
    // We cannot subdivide this quad further
    if (abs(topLeftPoint.GetX() - bottomRightPoint.GetX()) <= 0.5
        && abs(topLeftPoint.GetY() - bottomRightPoint.GetY()) <= 0.5) 
    {
        n = vox;
        return;
    }

    if (vox->point.GetX() >= (topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2.0f) {//Indicates right side of the tree

        // Indicates topRightQuad
        if ( vox->point.GetY() >= (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2) {
            if (topRightQuad == nullptr)
                topRightQuad = new QuadTree(
                    QuadPoint((topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2,
                        topLeftPoint.GetY()),
                    QuadPoint(bottomRightPoint.GetX(),
                        (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2));
            topRightQuad->depth = depth + 1;
            topRightQuad->insert(vox);
        }

        // Indicates botRightTree
        else {
            if (bottomRightQuad == nullptr)
                bottomRightQuad = new QuadTree(
                    QuadPoint((topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2,
                        (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2),
                    QuadPoint(bottomRightPoint.GetX(), bottomRightPoint.GetY()));
            bottomRightQuad->depth = depth + 1;
            bottomRightQuad->insert(vox);
        }
        
    }
    else {//Indicates left side of the tree
        
        // Indicates topLeftTree
        if ( vox->point.GetY() >= (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2 ) {
            if (topLeftQuad == nullptr)
                topLeftQuad = new QuadTree(
                    QuadPoint(topLeftPoint.GetX(), topLeftPoint.GetY()),
                    QuadPoint((topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2,
                        (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2));
            topLeftQuad->depth = depth + 1;
            topLeftQuad->insert(vox);
        }

        // Indicates bottomLeftQuad
        else {
            if (bottomLeftQuad == nullptr)
                bottomLeftQuad = new QuadTree(
                    QuadPoint(topLeftPoint.GetX(),
                        (topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2),
                    QuadPoint((topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2,
                        bottomRightPoint.GetY()));
            bottomLeftQuad->depth = depth + 1;
            bottomLeftQuad->insert(vox);
        }
    }
}

QuadVoxel* QuadTree::search(QuadPoint point)
{
    // Current quad cannot contain it
    if (!inBoundary(point))
        return nullptr;
 
    // We are at a quad of unit length
    // We cannot subdivide this quad further
    if (n != nullptr)
        return n;
 
    if ((topLeftPoint.GetX() + bottomRightPoint.GetX()) / 2 >= point.GetX()) {
        // Indicates topLeftTree
        if ((topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2 >= point.GetY()) {
            if (topLeftQuad == nullptr)
                return nullptr;
            return topLeftQuad->search(point);
        }
 
        // Indicates botLeftTree
        else {
            if (bottomLeftQuad == nullptr)
                return nullptr;
            return bottomLeftQuad->search(point);
        }
    }
    else {
        // Indicates topRightTree
        if ((topLeftPoint.GetY() + bottomRightPoint.GetY()) / 2 >= point.GetY()) {
            if (topRightQuad == nullptr)
                return nullptr;
            return topRightQuad->search(point);
        }
 
        // Indicates botRightTree
        else {
            if (bottomRightQuad == nullptr)
                return nullptr;
            return bottomRightQuad->search(point);
        }
    }
}

//AABB collision detection
bool QuadTree::inBoundary(QuadPoint point)
{
    return (point.GetX() >= topLeftPoint.GetX() && point.GetX() <= bottomRightPoint.GetX()
        && point.GetY() <= topLeftPoint.GetY() && point.GetY() >= bottomRightPoint.GetY());
}

