#include<Windows.h>
#include<gl\GL.h>
#include<gl\GLU.h>

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib")

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghWnd = NULL;
HDC ghDC = NULL;
HGLRC ghGLRC = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
bool gbLight = false;

GLUquadric *quadric = NULL;

GLfloat lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat lightSpecular[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition[] = {1.0f,1.0f,1.0f,0.0f};
GLfloat matrialSpecular[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat materialShininess[] = {50.0f};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	//function prototypes
	void initialize(void);
	void display(void);
	void uninitialize(void);
	void ToggleFullScreen();

	WNDCLASSEX wndclass;
	HWND hWnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("MyOpenGLCLASS");
	bool bDone = false;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Native Windowing : Sphere Lighting"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghWnd = hWnd;

	initialize();



	ShowWindow(hWnd, nCmdShow);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	//message loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				display();
			}
		}
	}


	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void display(void);
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	switch (iMsg)
	{
	case WM_CREATE:
		break;
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;

		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		case 0x4C:
			if (gbLight == false)
			{
				glEnable(GL_LIGHTING);
				gbLight = true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbLight = false;
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		uninitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hWnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			HMONITOR hm;
			hm = MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY);
			if (GetWindowPlacement(ghWnd, &wpPrev) && GetMonitorInfo(hm, &mi))
			{
				SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghWnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED
				);
			}
		}

		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
		);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghDC = GetDC(ghWnd);

	iPixelFormatIndex = ChoosePixelFormat(ghDC, &pfd);

	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghWnd, ghDC);
		ghDC = NULL;
	}

	if (SetPixelFormat(ghDC, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghWnd, ghDC);
		ghDC = NULL;
	}

	ghGLRC = wglCreateContext(ghDC);
	if (ghGLRC == NULL)
	{
		ReleaseDC(ghWnd, ghDC);
		ghDC = NULL;
	}

	if (wglMakeCurrent(ghDC, ghGLRC) == false)
	{
		wglDeleteContext(ghGLRC);
		ghGLRC = NULL;
		ReleaseDC(ghWnd, ghDC);
		ghDC = NULL;
	}

	quadric = gluNewQuadric();
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,lightSpecular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);
	glMaterialfv(GL_FRONT,GL_SPECULAR,matrialSpecular);
	glMaterialfv(GL_FRONT,GL_SHININESS,materialShininess);

	glEnable(GL_LIGHT0);

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f,0.0f,-3.0f);

	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.75, 70, 70);

	SwapBuffers(ghDC);
}


void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

}

void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
		);

		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghGLRC);
	ghGLRC = NULL;

	ReleaseDC(ghWnd, ghDC);
	ghDC = NULL;

	DestroyWindow(ghWnd);
}

