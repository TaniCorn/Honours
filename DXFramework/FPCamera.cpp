// FPCamera class
// Represents a single First Person camera with basic movement.
#include "FPCamera.h"

// Configure defaul camera 
FPCamera::FPCamera(Input* in, int width, int height, HWND hnd)
{
	input = in;
	winWidth = width;
	winHeight = height;
	wnd = hnd;
}

void FPCamera::move(float dt)
{
	setFrameTime(dt);
	// Handle the input.
	if (input->isKeyDown('W'))
	{
		// forward
		moveForward();
	}
	if (input->isKeyDown('S'))
	{
		// back
		moveBackward();
	}
	if (input->isKeyDown('A'))
	{
		// Strafe Left
		strafeLeft();
	}
	if (input->isKeyDown('D'))
	{
		// Strafe Right
		strafeRight();
	}
	if (input->isKeyDown('Q'))
	{
		// Down
		moveDownward();
	}
	if (input->isKeyDown('E'))
	{
		// Up
		moveUpward();
	}
	if (input->isKeyDown(VK_UP))
	{
		// rotate up
		turnUp();
	}
	if (input->isKeyDown(VK_DOWN))
	{
		// rotate down
		turnDown();
	}
	if (input->isKeyDown(VK_LEFT))
	{
		// rotate left
		turnLeft();
	}
	if (input->isKeyDown(VK_RIGHT))
	{
		// rotate right
		turnRight();
	}

	if (input->isMouseActive())
	{
		// mouse look is on
		deltax = input->getMouseX() - (winWidth / 2);
		deltay = input->getMouseY() - (winHeight / 2);
		turn(deltax, deltay);
		cursor.x = winWidth / 2;
		cursor.y = winHeight / 2;
		ClientToScreen(wnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
	}

	if (input->isRightMouseDown() && !input->isMouseActive())
	{
		// re-position cursor
		cursor.x = winWidth / 2;
		cursor.y = winHeight / 2;
		ClientToScreen(wnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
		input->setMouseX(winWidth / 2);
		input->setMouseY(winHeight / 2);
		
		// set mouse tracking as active and hide mouse cursor
		input->setMouseActive(true);
		ShowCursor(false);
	}
	else if (!input->isRightMouseDown() && input->isMouseActive())
	{
		// disable mouse tracking and show mouse cursor
		input->setMouseActive(false);
		ShowCursor(true);
	}

	//if (input->isKeyDown(VK_SPACE))
	//{
	//	// re-position cursor
	//	cursor.x = winWidth / 2;
	//	cursor.y = winHeight / 2;
	//	ClientToScreen(wnd, &cursor);
	//	SetCursorPos(cursor.x, cursor.y);
	//	input->setMouseX(winWidth / 2);
	//	input->setMouseY(winHeight / 2);
	//	input->SetKeyUp(VK_SPACE);
	//	// if space pressed toggle mouse
	//	input->setMouseActive(!input->isMouseActive());
	//	if (!input->isMouseActive())
	//	{
	//		ShowCursor(true);
	//	}
	//	else
	//	{
	//		ShowCursor(false);
	//	}
	//}
	update();
}