#include "BoundingVolumeHierarchy.h"

BoundingVolumeHierarchy::BoundingVolumeHierarchy()
{
}

BoundingVolumeHierarchy::BoundingVolumeHierarchy(BVHPoint br, BVHPoint tL)
{
	bottomRight = br;
	topLeft = tL;
}

void BoundingVolumeHierarchy::BuildBVH()
{
    //Validate all points are within bounds, discard any that are not.

	//Split our box by axis and into two boxes

	//Send points to children and call buildBVH

	//Repeat until there is only 4 points per box

	//After the recursive loop finishes, parents check their children to see if there is any points inside their box. If not the box gets deleted

	//Resize the boxes to the children boxes or to the points inside the box.

	int pointsSize = points.size();
	for (int i = 0; i < pointsSize; i++)
	{
		if (!inBoundary(points[i]))
		{
			points.erase(points.begin() + i);
			i--;
			pointsSize--;
		}
	}
	points.shrink_to_fit();
	pointsSize = points.size();

	if (pointsSize == 0 || pointsSize <= 5)
	{
		ResizeBounds();
		return;
	}

	//Split by x axis
	int randnum = rand() % 3;
	if (randnum == 1)
	{
		float xMid = 0;
		for (int i = 0; i < pointsSize; i++)
		{
			xMid += points[i].GetX();
		}
		xMid /= pointsSize;

		left = new BoundingVolumeHierarchy(BVHPoint(xMid, bottomRight.GetY(), 0), topLeft);
		right = new BoundingVolumeHierarchy(bottomRight, BVHPoint(xMid, topLeft.GetY(), 0));
	}
	else {
		float yMid = 0;
		for (int i = 0; i < pointsSize; i++)
		{
			yMid += points[i].GetY();
		}
		yMid /= pointsSize;

		left = new BoundingVolumeHierarchy(BVHPoint(bottomRight.GetX(), yMid, 0), topLeft);
		right = new BoundingVolumeHierarchy(bottomRight, BVHPoint(topLeft.GetX(), yMid, 0));
	}
	

	//Send points to children
	left->points = points;
	right->points = points;
	left->BuildBVH();
	right->BuildBVH();

	if (left->points.size() == 0)
	{
		delete left;
		left = nullptr;
	}
	if (right->points.size() == 0)
	{
		delete right; 
		right = nullptr;
	}

	ResizeBounds();

}

bool BoundingVolumeHierarchy::inBoundary(BVHPoint point)
{
	return (point.GetX() >= topLeft.GetX() && point.GetX() <= bottomRight.GetX()
		&& point.GetY() <= topLeft.GetY() && point.GetY() >= bottomRight.GetY());
}

void BoundingVolumeHierarchy::ResizeBounds()
{
	//If the elseif statmenets trigger, we should really subsume the single child box.
	if (left != nullptr && right != nullptr)
	{
		
		if (topLeft.GetX() > bottomRight.GetX())
		{
			float tlX = topLeft.GetX();
			topLeft.SetPoint(bottomRight.GetX(), topLeft.GetY(), 0);
			bottomRight.SetPoint(tlX, bottomRight.GetY(), 0);
		}
		if (topLeft.GetY() < bottomRight.GetY())
		{
			float tlY = topLeft.GetY();
			topLeft.SetPoint(topLeft.GetX(), bottomRight.GetY(), 0);
			bottomRight.SetPoint(bottomRight.GetX(), tlY, 0);

		}
	}
	else if (left != nullptr) 
	{

		//topLeft = left->topLeft;
		//bottomRight = left->bottomRight;
	}
	else if (right != nullptr) 
	{
		//topLeft = right->topLeft;
		//bottomRight = right->bottomRight;
	}
	else 
	{
		if (points.size() >= 2)//There is no point resizing if there are only 2 points to check
		{
			float minLeft = bottomRight.GetX();
			float maxRight = topLeft.GetX();
			float minBottom = topLeft.GetY();
			float maxTop = bottomRight.GetY();
			for (int i = 0; i < points.size(); i++)
			{
				float x = points[i].GetX();
				if (x < minLeft)
				{
					minLeft = x;
				}
				if (x > maxRight)
				{
					maxRight = x;
				}
				float y = points[i].GetY();
				if (y < minBottom)
				{
					minBottom = y;
				}
				if (y > maxTop) 
				{
					maxTop = y;
				}
			}
			topLeft = BVHPoint(minLeft, maxTop, 0);
			bottomRight = BVHPoint(maxRight, minBottom, 0);
		}
	}
}

void BoundingVolumeHierarchy::ReverseResizeRecursion()
{
	ResizeBounds();
	if (left != nullptr)
	{
		left->ReverseResizeRecursion();

	}
	if (right != nullptr)
	{
		right->ReverseResizeRecursion();

	}
}
