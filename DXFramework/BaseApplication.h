/**
* \class BaseApplication
*
* \brief Default application setup, inherit from this
*
* This class is the parent application to inherit from when creating a new application.
* Handles the default configuration of the renderer, camera, input, timer and texture manager.
*
* \author Paul Robertson
*/

#ifndef _BASEAPPLICATION_H_
#define _BASEAPPLICATION_H_

// Globals
//const bool FULL_SCREEN = false;
//const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 200.0f;	// 1000.0f
const float SCREEN_NEAR = 0.1f;		//0.1f

// Includes
#include "input.h"
#include "d3d.h"
#include "FPCamera.h"
#include "timer.h"
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_dx11.h"
#include "imGUI/imgui_impl_win32.h"
#include "TextureManager.h"


class BaseApplication
{
public:
	/// Create an empty BaseApplication
	BaseApplication();
	~BaseApplication();
	/** \brief Virtual function for class initialisation
	*
	* Virtual function for default initialisation of the BaseApplication. This should be overridden adding adding initialisation as required.
	*
	* @param hinstance is a handle to the instance
	* @param hwnd is a handle to the window
	* @param screenwidth is the desired screen width
	* @param screenHeight is the desired screen height
	* @param "in" is a pointer to the input class for handling user keyboard/mouse input
	* @param VSYNC is a boolean for if vsync should be enabled/disabled
	* @param FULL_SCREEN is a boolean for if the window is full screen
	*/
	virtual void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN);

	/** \brief Virtual frame/update function.
	* Contains default update/frame operations including calculating delta time, calling handle input and starting UI rendering
	*/
	virtual bool frame();

protected:
	/** \brief Protected Virtual function for handling input
	* Function provides default input handling for camera and UI functions. 
	* Need to override and add specific additional functionality
	* @param dt is delta time
	*/
	virtual void handleInput(float dt);
	/// Pure virtual function for render. Make your own.
	virtual bool render() = 0;

protected:
	HWND wnd;				///< handle to the window
	int sWidth, sHeight;	///< stores screen width and height
	int deltax, deltay;		///< for mouse movement
	POINT cursor;			///< Used for converting mouse coordinates for client to screen space
	Input* input;			///< Pointer to input class
	D3D* renderer;			///< Pointer to renderer
	FPCamera* camera;			///< Pointer to camera object
	Timer* timer;			///< Pointer to timer object (for delta time and FPS)
	TextureManager* textureMgr;	///< Pointer to texture manager (handles loading and storing of textures)
	bool wireframeToggle;	///< Boolean tracking if wireframe is de/activated
};

#endif