/**
* \class System/Window
*
* \brief Defines and initialises a window
*
* Defines and creates a simple window for our application.
* Captures and handles window events.
*
* \author Paul Robertson
*/

// System.h
// Handles the setup and initialisation of the window.
// Captures window events.

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define WIN32_LEAN_AND_MEAN

// INCLUDES 
#include <windows.h>
#include "baseapplication.h"
#include "Input.h"

class System
{
public:
	/** Initialises the windows based on screen width and height, vsync on or off and full screen or not. Initialises the app to run within the window */
	System(BaseApplication* application, int screenWidth, int screenHeight, bool VSYNC, bool FULL_SCREEN);
	~System();

	/// start the application
	void run();

	LRESULT CALLBACK messageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool frame();	///< Frame/update function, called during game loop
	void initialiseWindows(int& width, int& height);	///< Creates the window
	void shutdownWindows();	///< Safely destroys the window

private:
	LPCWSTR applicationName;	
	HINSTANCE hinstance;
	HWND hwnd;
	BaseApplication* app;		///< Pointer to application to run
	Input input;				///< Input class
	bool FULL_SCREEN;			///< Variable to tracker full screen state

	//Prototypes
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};

//Globals
static System* ApplicationHandle = 0;

#endif