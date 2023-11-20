//System.cpp
#include "system.h"

// Initialise the window and application.
System::System(BaseApplication* application, int screenWidth, int screenHeight, bool VSYNC, bool lFULL_SCREEN)
{
	//int screenWidth, screenHeight;
	//screenWidth = 0;
	//screenHeight = 0;

	FULL_SCREEN = lFULL_SCREEN;

	initialiseWindows(screenWidth, screenHeight);

	RECT rect;
	GetClientRect(hwnd, &rect);

	app = application;
	app->init(hinstance, hwnd, rect.right, rect.bottom, &input, VSYNC, FULL_SCREEN);

}

// Release resources.
System::~System()
{
	if (app)
	{
		delete app;
		app = 0;
	}

	shutdownWindows();
}

// Contains the game loop.
void System::run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}
		else
		{
			// If windows signals to end the application then exit out.
			//if (msg.message == WM_QUIT)
			//{
			//	done = true;
			//}
			//else
			//{
				// Otherwise do the frame processing.
				result = frame();
				if (!result)
				{
					done = true;
				}
			//}
		}

	}
	PostQuitMessage(0);
	return;
}

// Called once per frame.
// If false is returned, either user has activated application close or error has occurred.
bool System::frame()
{
	bool result;
	result = app->frame();
	if (!result)
	{
		return false;
	}

	return true;
}

// Captures window events.
LRESULT CALLBACK System::messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void System::initialiseWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// Determine the resolution of the clients desktop screen.
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		//screenWidth = 1024;
		//screenHeight = 576;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		/*WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,*/ WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);

	return;
}


void System::shutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(applicationName, hinstance);
	hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

//imGUI should be uncommented.
//extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK System::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{

	switch (umessage)
	{
		// Handle keyboard and mouse input.
		case WM_KEYDOWN:
		{
			ApplicationHandle->input.SetKeyDown(wparam);
			break;
		}
		case WM_KEYUP:
		{
			ApplicationHandle->input.SetKeyUp(wparam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			ApplicationHandle->input.setMouseX(LOWORD(lparam));
			ApplicationHandle->input.setMouseY(HIWORD(lparam));
			break;
		}
		case WM_LBUTTONDOWN:
		{
			ApplicationHandle->input.setLeftMouse(true);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			ApplicationHandle->input.setRightMouse(true);
			break;
		}
		case WM_LBUTTONUP:
		{
			ApplicationHandle->input.setLeftMouse(false);
			break;
		}
		case WM_RBUTTONUP:
		{
			 ApplicationHandle->input.setRightMouse(false);
			 break;
		}
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			return 0;
		}

	}

	//if (ImGui_ImplDX11_WndProcHandler(hwnd, umessage, wparam, lparam))
	//	return true;
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;

	return ApplicationHandle->messageHandler(hwnd, umessage, wparam, lparam);
}