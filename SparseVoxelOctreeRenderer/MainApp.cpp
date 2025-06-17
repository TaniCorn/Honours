#include "System.h"
#include "AppScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	AppScene* app = new AppScene();
	System* system;

	// Create the system object.
	system = new System(app, 1200, 675, true, false);

	// Initialize and run the system object.
	system->run();

	// Shutdown and release the system object.
	//delete system;
	system = 0;

	return 0;
}