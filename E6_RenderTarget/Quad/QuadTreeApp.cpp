#include "QuadTreeApp.h"
QuadTreeApp::QuadTreeApp()
{

}

void QuadTreeApp::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenh = screenHeight;
	screenw = screenWidth;

	quadTreeShader = new QuadTreeShader(renderer->getDevice(), hwnd);

	camera->setPosition(0, 0, 0);
	camera->setRotation(0, 0, 0);
	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1);
	bottomRightPoint = new QuadPoint(resolution,0);
	topLeftPoint = new QuadPoint(0,resolution);
	quadTree = new QuadTree(*topLeftPoint, *bottomRightPoint);
	plane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 2);
	quadTree->insert(point);
	QuadVoxel* vox = new QuadVoxel();
	vox->identifier = 0;
	vox->point = QuadPoint(1,1);
	points.push_back(vox);
	vox = new QuadVoxel();
	vox->identifier = 1;
	vox->point = QuadPoint(3,1);
	points.push_back(vox);
	vox = new QuadVoxel();
	vox->identifier = 2;
	vox->point = QuadPoint(3,3);
	points.push_back(vox);
	vox = new QuadVoxel();
	vox->identifier = 2;
	vox->point = QuadPoint(3.7,3.8);
	points.push_back(vox);
	
	for (int i = 0; i < points.size(); ++i)
	{
		quadTree->insert(points[i]);
	}
	

	OctVoxel* oct = new OctVoxel();
	oct->identifier = 0;
	oct->point = OctPoint(1,1,1);
	octpoints.push_back(oct);
	oct = new OctVoxel();
	oct->identifier = 1;
	oct->point = OctPoint(0.5,3.5,0.5);
	octpoints.push_back(oct);
	oct = new OctVoxel();
	oct->identifier = 2;
	oct->point = OctPoint(2,2,2);
	octpoints.push_back(oct);
	oct = new OctVoxel();
	oct->identifier = 2;
	oct->point = OctPoint(3.1,0.9,3.1);
	octpoints.push_back(oct);
	oct = new OctVoxel();
	oct->identifier = 2;
	oct->point = OctPoint(6.1,0.9,6.1);
	octpoints.push_back(oct);
	octree = new Octree(OctPoint(0,resolution,0), OctPoint(resolution,0,resolution));
	for (int i = 0; i < octpoints.size(); ++i)
	{
		octree->insert(octpoints[i]);
	}
	//octree->insert(octpoints.back());

	maxDepth = GetMaxDepth(quadTree, 0);

}


QuadTreeApp::~QuadTreeApp()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}


bool QuadTreeApp::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	camera->move(0.01);

	return true;
}

bool QuadTreeApp::render()
{

	//texturepass();
	octpass();
	return true;


}

void QuadTreeApp::RecursiveQuadTreeLoop(QuadTree* quad_tree, float depth)
{
	depth += 0.1f;
	if (quad_tree->bottomLeftQuad != nullptr)
	{
		RenderPlaneQuad(depth, quad_tree->bottomLeftQuad);
		RecursiveQuadTreeLoop(quad_tree->bottomLeftQuad, depth);
	}
	if (quad_tree->bottomRightQuad != nullptr)
	{
		RenderPlaneQuad(depth + 0.01f, quad_tree->bottomRightQuad);
		RecursiveQuadTreeLoop(quad_tree->bottomRightQuad, depth);

	}
	if (quad_tree->topLeftQuad != nullptr)
	{
		RenderPlaneQuad(depth + 0.02f, quad_tree->topLeftQuad);
		RecursiveQuadTreeLoop(quad_tree->topLeftQuad, depth);

	}
	if (quad_tree->topRightQuad != nullptr)
	{
		RenderPlaneQuad(depth + 0.03f, quad_tree->topRightQuad);
		RecursiveQuadTreeLoop(quad_tree->topRightQuad, depth);

	}
}

void QuadTreeApp::RenderPlaneQuad(const float depth, QuadTree* childQuad)
{
	
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	int divis = childQuad->depth ;
	int fact = pow(2, divis);
	// if (divis == 5)
	// {
	// 	divis = 16;
	// }
	// if (divis == 4)
	// {
	// 	divis = 8;
	// }
	// if (divis == 3)
	// {
	// 	divis = 4;
	// }
	// if (divis == 2)
	// {
	// 	divis = 2;
	// }
	// float resdivis = resolution / divis;
	const float res = resolution/static_cast<float>(fact);
	XMMATRIX scale = XMMatrixScaling(res, res, res);
	float x = childQuad->topLeftPoint.GetX();
	float y = depth;
	float z = childQuad->bottomRightPoint.GetY();
	XMMATRIX translation = XMMatrixTranslation(x,y,z);

	
	plane->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * scale * translation, viewMatrix, projectionMatrix, depth, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());
	
}

int Bigger(int a, int b)
{
	return (a > b) ? a : b;
}
int QuadTreeApp::GetMaxDepth(QuadTree* quad_tree, int depth)
{
	depth++;

	if (quad_tree->bottomLeftQuad != nullptr)
	{
		int newDepth = GetMaxDepth(quad_tree->bottomLeftQuad, depth);
		depth = Bigger(depth, newDepth);
	}
	if (quad_tree->bottomRightQuad != nullptr)
	{
		int newDepth = GetMaxDepth(quad_tree->bottomRightQuad, depth);
		depth = Bigger(depth, newDepth);

	}
	if (quad_tree->topLeftQuad != nullptr)
	{
		int newDepth = GetMaxDepth(quad_tree->topLeftQuad, depth);
		depth = Bigger(depth, newDepth);

	}
	if (quad_tree->topRightQuad != nullptr)
	{
		int newDepth = GetMaxDepth(quad_tree->topRightQuad, depth);
		depth = Bigger(depth, newDepth);
	}
	return depth;
}

void QuadTreeApp::RecursiveOctreeLoop(Octree* octree, float depth)
{
	depth += 0.1f;
	if (octree->BLF != nullptr)
	{
		RenderCubeTree(depth, octree->BLF);
		RecursiveOctreeLoop(octree->BLF, depth);
	}
	if (octree->BRF != nullptr)
	{
		RenderCubeTree(depth, octree->BRF);
		RecursiveOctreeLoop(octree->BRF, depth);
	}
	if (octree->TLF != nullptr)
	{
		RenderCubeTree(depth, octree->TLF);
		RecursiveOctreeLoop(octree->TLF, depth);
	}
	if (octree->TRF != nullptr)
	{
		RenderCubeTree(depth, octree->TRF);
		RecursiveOctreeLoop(octree->TRF, depth);
	}

	
	if (octree->BLB != nullptr)
	{
		RenderCubeTree(depth, octree->BLB);
		RecursiveOctreeLoop(octree->BLB, depth);
	}
	if (octree->BRB != nullptr)
	{
		RenderCubeTree(depth, octree->BRB);
		RecursiveOctreeLoop(octree->BRB, depth);
	}
	if (octree->TLB != nullptr)
	{
		RenderCubeTree(depth, octree->TLB);
		RecursiveOctreeLoop(octree->TLB, depth);
	}
	if (octree->TRB != nullptr)
	{
		RenderCubeTree(depth, octree->TRB);
		RecursiveOctreeLoop(octree->TRB, depth);
	}
}

void QuadTreeApp::RenderCubeTree(const float depth, Octree* childQuad)
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	int divis = childQuad->depth +1;
	int fact = pow(2, divis);

	const float res = resolution/static_cast<float>(fact);
	XMMATRIX scale = XMMatrixScaling(res, res, res);
	float x = childQuad->topLeftFrontPoint.GetX();
	float y =  childQuad->topLeftFrontPoint.GetY();;
	float z =  childQuad->topLeftFrontPoint.GetZ();
	XMMATRIX translation = XMMatrixTranslation(x,y,z);
	XMMATRIX localMoveMatrix = XMMatrixTranslation(1,-1,1);

	
	cube->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix *localMoveMatrix* scale * translation, viewMatrix, projectionMatrix, depth, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
}


void QuadTreeApp::texturepass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX scale = XMMatrixScaling(resolution, resolution, resolution);

	plane->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * scale, viewMatrix, projectionMatrix,0.0f, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());

	plane->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(resolution,1,resolution), viewMatrix, projectionMatrix,0.0f, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());

	// For my own understanding. Creates planes underneath to show the position relationships.
	// What I've learnt: Everything renders from the bottom left spreading towards the top right. So we have to use the topleft x, and bottom right y to render the plane at the intended location
	// plane->sendData(renderer->getDeviceContext());
	// quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(topLeftPoint->GetX(),-1,topLeftPoint->GetY()), viewMatrix, projectionMatrix);
	// quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());
	// plane->sendData(renderer->getDeviceContext());
	// quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(bottomRightPoint->GetX(),-1,bottomRightPoint->GetY()), viewMatrix, projectionMatrix);
	// quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());
	//
	// scale = XMMatrixScaling(resolution/2, resolution/2, resolution/2);
	// plane->sendData(renderer->getDeviceContext());
	// quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(topLeftPoint->GetX(),-2,bottomRightPoint->GetY()) * scale, viewMatrix, projectionMatrix);
	// quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());

		float depth = 1.0f; 
		RecursiveQuadTreeLoop(quadTree, depth);

	for (int i = 0; i < points.size(); ++i)
	{
		XMMATRIX scale = XMMatrixScaling(0.1, 0.1, 0.1);

		cube->sendData(renderer->getDeviceContext());
		quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * scale* XMMatrixTranslation(points[i]->point.GetX(),2,points[i]->point.GetY()) , viewMatrix, projectionMatrix,1, 1);
		quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
	}



	
	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void QuadTreeApp::octpass()
{
		// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX scale = XMMatrixScaling(resolution/2, resolution/2, resolution/2);
	XMMATRIX localMoveMatrix = XMMatrixTranslation(1,-1,1);
	//We have to move the local to the TopLeftfront, and scale down to half resolution
	
	cube->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix*localMoveMatrix * scale * XMMatrixTranslation(0,resolution,0), viewMatrix, projectionMatrix,0.0f, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	cube->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix *localMoveMatrix* XMMatrixTranslation(resolution,0,resolution), viewMatrix, projectionMatrix,0.0f, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
	plane->sendData(renderer->getDeviceContext());
	quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(resolution,0,resolution), viewMatrix, projectionMatrix,0.0f, maxDepth);
	quadTreeShader->render(renderer->getDeviceContext(), plane->getIndexCount());

		float depth = 1.0f;
	RecursiveOctreeLoop(octree, depth);
	// 	RecursiveQuadTreeLoop(quadTree, depth);
	//
	for (int i = 0; i < octpoints.size(); ++i)
	{
		XMMATRIX scale = XMMatrixScaling(0.05, 0.05, 0.05);
	
		cube->sendData(renderer->getDeviceContext());
		quadTreeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * scale* XMMatrixTranslation(octpoints[i]->point.GetX(),octpoints[i]->point.GetY(),octpoints[i]->point.GetZ()) , viewMatrix, projectionMatrix,1, 1);
		quadTreeShader->render(renderer->getDeviceContext(), cube->getIndexCount());
	}



	
	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}


void QuadTreeApp::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

