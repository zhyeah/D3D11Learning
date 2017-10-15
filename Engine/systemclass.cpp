#include "systemclass.h"
#include "EngineConfig.h"
using namespace EngineConfig;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ApplicationHandle->MessageHandler(hwnd, msg, wParam, lParam);
}

SystemClass::SystemClass(): m_input(0), m_graphics(0), m_timer(0)
{
}

SystemClass::SystemClass(const SystemClass &)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	// Initialize window at first.
	int screenHeight = 0, screenWidth = 0;
	InitializeWindows(screenWidth, screenHeight);

	// Initialize input class.
	m_input = new InputClass();
	if (m_input != NULL) 
	{
		m_input->Initialize();
	}

	// Initialize grahpic class.
	m_graphics = new GraphicsClass();
	if (m_graphics != NULL) 
	{
		m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	}

	// Initialize timer class.
	m_timer = new Timer();

	return true;
}

void SystemClass::Shutdown()
{
	// Release input.
	if (m_input != NULL) 
	{
		delete m_input;
		m_input = NULL;
	}

	// Release graphics.
	if (m_graphics != NULL) 
	{
		delete m_graphics;
		m_graphics = NULL;
	}

	// Release timer
	if (m_timer != NULL)
	{
		delete m_timer;
		m_timer = NULL;
	}

	// Shutdown window.
	ShutdownWindows();
}

void SystemClass::Run()
{
	// Clear memory of message.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Reset timer.
	m_timer->Reset();
	// Start timer.
	m_timer->Start();

	// Loop for the message.
	bool quit = false;
	while (!quit) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if (msg.wParam == WM_QUIT) 
		{
			quit = true;
		}
		else 
		{
			// Do rendering work.
			m_timer->Tick();

			if (!Frame(m_timer->DeltaTime())) 
			{
				quit = true;
			}
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYDOWN:
		m_input->KeyDown((unsigned int)wParam);
		return 0;
		break;
	case WM_KEYUP:
		m_input->KeyUp((unsigned int)wParam);
		return 0;
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}

	return LRESULT();
}

bool SystemClass::Frame(float dt)
{
	// Check if the user pressed escape and wants to exit the application.
	if (m_input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	bool result = m_graphics->Frame(dt);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitializeWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (DisplayConfig::UseFullScreen)
	{
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
		screenWidth = DisplayConfig::ScreenWidth;
		screenHeight = DisplayConfig::ScreenHeight;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create window.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide cursor.
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (DisplayConfig::UseFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;
}
