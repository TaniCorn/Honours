// Main.cpp
#include "System.h"
//#include "App1.h"
#include "GridApp.h"
#include "Quad/QuadTreeApp.h"
//#include "QuadTreeApp.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//App1* app = new App1();
	///GridApp* grid = new GridApp();
	QuadTreeApp* grid = new QuadTreeApp();
	System* system;

	// Create the system object.
	system = new System(grid, 1200, 675, true, false);

	// Initialize and run the system object.
	system->run();

	// Shutdown and release the system object.
	delete system;
	system = 0;

	return 0;
}