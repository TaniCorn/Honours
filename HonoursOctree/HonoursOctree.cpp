#include "System.h"
#include "OctreeTracerApp.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	OctreeTracerApp* grid = new OctreeTracerApp();
	System* system;

	// Create the system object.
	system = new System(grid, 1200, 675, true, false);

	// Initialize and run the system object.
	system->run();

	// Shutdown and release the system object.
	//delete system;
	system = 0;

	return 0;
}