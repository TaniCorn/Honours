// Input class
// Functions for retrieving input events/state.
#include "Input.h"

void Input::SetKeyDown(WPARAM key)
{
	keys[key] = true;
}

void Input::SetKeyUp(WPARAM key)
{
	keys[key] = false;
}

bool Input::isKeyDown(int key)
{
	return keys[key];
}

void Input::setMouseX(int xPosition)
{
	mouse.x = xPosition;
}

void Input::setMouseY(int yPosition)
{
	mouse.y = yPosition;
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input::getMouseY()
{
	return mouse.y;
}

void Input::setLeftMouse(bool down)
{
	mouse.left = down;
}

void Input::setRightMouse(bool down)
{
	mouse.right = down;
}

bool Input::isLeftMouseDown()
{
	return mouse.left;
}

bool Input::isRightMouseDown()
{
	return mouse.right;
}

void Input::setMouseActive(bool active)
{
	mouse.isActive = active;
}
bool Input::isMouseActive()
{
	return mouse.isActive;
}

void Input::resetAllInputs()
{
	// Set all keys up
	for (size_t i = 0; i < 256; i++)
	{
		keys[i] = false;
	}

	mouse.left = false;
	mouse.right = false;
	mouse.x = 0;
	mouse.y = 0;
}
