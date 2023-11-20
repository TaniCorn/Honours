/**
* \class User Input class
*
* \brief Stores keyboard and mouse input events
*
* Simple input handler class. Functions for setting and getting keyboard and mouse events.
*
* \author Paul Robertson
*/

#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>

class Input
{
	/// Mouse stuct, store position, button click and active status
	struct Mouse
	{
		int x, y;
		bool left, right, isActive;
	};

public:
	void SetKeyDown(WPARAM key);	///< Sets key down value for specified key
	void SetKeyUp(WPARAM key);		///< Sets key up value for specified key

	bool isKeyDown(int key);		///< Check if specified key is down
	void setMouseX(int xPosition);	///< Set mouse X-axis position
	void setMouseY(int yPosition);	///< Set mouse y-axis position
	int getMouseX();				///< Get mouse x-axis position
	int getMouseY();				///< Get mouse y-axis position
	void setLeftMouse(bool down);	///< Set left mouse click
	void setRightMouse(bool down);	///< Set right mouse click
	bool isLeftMouseDown();			///< Check left mouse click
	bool isRightMouseDown();		///< Check right mouse click
	void setMouseActive(bool active);	///< Set monuse in/active
	bool isMouseActive();			///< Check if mouse is in/active

private:
	bool keys[256];		///< Array for storing key states
	Mouse mouse;		///< Mouse state variable

};

#endif