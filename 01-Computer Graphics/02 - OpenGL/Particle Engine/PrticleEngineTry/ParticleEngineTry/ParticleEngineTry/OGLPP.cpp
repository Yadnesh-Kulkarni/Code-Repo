// common headers
#include <windows.h>
#include <stdio.h> // for FILE I/O

// OpenGL headers
#include <C:\glew\include\GL\glew.h> // for GLSL exentensions IMPORTANT : This Line Should Be Before #include <gl\GL.h>
#include <gl/GL.h>

#include "Geometry.h"

#include "vmath.h"
#include "perlin.h"

#include "OGLPP.h"

#include "SOIL.h"
// import libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")
#pragma comment(lib, "opengl32.lib")

// macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

// Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
FILE *gpFile = NULL;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbFullscreen = false;
bool gbEscapeKeyIsPressed = false;
bool gStartAnimation = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao;
GLuint gVbo_position;
GLuint gVbo_texture;
GLuint gVbo_velocity;
GLuint gVbo_starttime;
GLuint gVbo_normal;

GLuint gModelViewMatrixUniform, gProjectionMatrixUniform;
GLuint gLightPositionUniform;
GLuint gSamplerUniform;
GLuint gScaleUniform;
GLuint gSkyColorUniform;
GLuint gCloudColorUniform;
GLuint gOffsetUniform;
GLuint gParticleLifeTimeUniform;

int noise3DTexSize = 128;
GLuint gNoise3DTexName = 0;
GLubyte *noise3DTexPtr;

GLuint gLocation;
GLfloat ParticleTime = 0.0f;

static GLint arrayWidth, arrayHeight;
static GLfloat *verts = NULL;
static GLfloat *norms = NULL;
static GLfloat *textures = NULL;
static GLfloat *velocities = NULL;
static GLfloat *startTimes = NULL;

mat4 gPerspectiveProjectionMatrix;

GLuint gVertexShaderObject1;
GLuint gFragmentShaderObject1;
GLuint gShaderProgramObject1;

GLuint model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;

GLuint L_KeyPressed_uniform;

GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

//mat4 gPerspectiveProjectionMatrix;

bool gbLight;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 0.0f, 0.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

GLuint flameTexturePng;
GLuint smokeTexturePng;

float rotAngle = 0.0f;
static float a = -3.0;

//Smoke Sampler
GLuint uniformSamplerSmoke;

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	// Function prototype
	void initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("OpenGLPP");
	bool bDone = false;
	int iWidth;
	int iHeight;

	// Code
	// create log file
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExiting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "Log File Is Successfully Opened.\n");
	}

	// Initializating member of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Registering Class
	RegisterClassEx(&wndclass);

	// Retrive Width
	iWidth = GetSystemMetrics(SM_CXSCREEN);

	// Retrive Height
	iHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create Window
	// Parallel to glutInitWindow(), glutInitWindowPosition() and glutCreateWindow() all three together
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OGL Programmable Pipeline : Rocket Ignition Fire"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((iWidth - WIN_WIDTH) / 2),
		((iHeight - WIN_HEIGHT) / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// initialize
	initialize();

	// Message Loop
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
				if (gbEscapeKeyIsPressed == true) // Continuation to glutLeaveMainLoop();
					bDone = true;
				update();
				display();
			}
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

// WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function prototype
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	// variable declarations
	static bool bIsLKeyPressed = false;

	// Code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) // if 0, the window is Active
			gbActiveWindow = true;
		else // if non-zero, the window is not active
			gbActiveWindow = false;
		break;
	case WM_SIZE: // Parallel to glutReshapeFunc();
		resize(LOWORD(lParam), HIWORD(lParam)); // Parallel to glutReshapeFunc(resize);
		break;
	case WM_KEYDOWN: // Parallel to glutKeyboardFunc();
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE: // Case 27
			if (gbEscapeKeyIsPressed == false) // Parallel to glutLeaveMainLoop();
			{
				gbEscapeKeyIsPressed = true;
			}
			break;
		case 0x31: // Case 1
			if (gStartAnimation == false) // Parallel to glutLeaveMainLoop();
			{
				gStartAnimation = true;
			}
			break;
		case 0x46: // for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		case 0x4C: // for 'l' or 'L'
			if (bIsLKeyPressed == false)
			{
				gbLight = true;
				bIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				bIsLKeyPressed = false;
			}
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN: // Parallel to glutMouseFunc();
		break;
	case WM_CLOSE: // Parallel to glutCloseFunc();
		uninitialize(); // // Parallel to glutCloseFunc(uninitialize);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// Variable declaration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// Code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	// function prototype
	void resize(int, int);
	void uninitialize(void);
	void createPoints(GLint, GLint);
	void init3DNoiseTexture(void);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// Initialization of structure 'PIXELFORMATDESCRIPTOR'
	// Parallel to glutInitDisplayMode()
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1; // structure version
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd); // Painter

	// choose a pixel format which best matches with that of 'pfd'
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// set the pixel format chosen above
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// create OpenGL rendering context
	ghrc = wglCreateContext(ghdc); // Bridge between OpenGL and Window
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// make the rendering context created above as current n the current hdc
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// GLEW Initialization Code For GLSL (IMPORTANT : It Must Be Here. Means After Creating Context But Before Using Any OpenGL Function)
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	/* get OpenGL version */
	fprintf_s(gpFile, "------------------------------------------------------------------\n");
	fprintf_s(gpFile, "%s\n", glGetString(GL_VERSION));
	fprintf_s(gpFile, "------------------------------------------------------------------\n");
	/* get GLSL version */
	fprintf_s(gpFile, "%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf_s(gpFile, "------------------------------------------------------------------\n");

	// *** VERTEX SHADER FIRE ***
	// create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 460"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 Velocity;"\
		"in float StartTime;"\
		"out float transparacy;"\
		"out vec3 cam_position;"\
		"out vec2 texCoords;"\
		"uniform float u_scale;"\
		"uniform float Time;"\
		"uniform mat4 u_model_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"out float out_time;"\
		"uniform float ParticleLifeTime;"\
		"out float fMapped;"\
		"out vec3 pos_out;"\
		"void main(void)"\
		"{"\
		"vec4 vert;"
		"texCoords = vPosition.xy;"\
		"transparacy = 0.0;"\
		"float t = Time - StartTime;"\
		"if (t >= 0.0)"\
		"{"\
		"vert = vPosition + vec4(Velocity * t, 0.0);"\
		"vert.z -= -0.2f * t * t;"\
		"transparacy = 1.0 - 0.8* (t / ParticleLifeTime);"\
		"}"\
		"else"\
		"{"\
		"vert = vPosition;"
		"}"\
		"out_time = t;"\
		"cam_position = vec3(vert) * u_scale;"\
		"vec4 pos = u_projection_matrix * u_model_view_matrix * vert;"\
		"gl_Position = pos;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADER FIRE ***
	// create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 460"\
		"\n"\
		"in float transparacy;\n"\
		"in vec3 cam_position;\n"\
		"in float out_time;\n"\
		"in vec2 texCoords;\n"\
		"out vec4 FragColor;\n"\
		"uniform vec3 u_SkyColor;\n"\
		"uniform vec3 u_CloudColor;\n"\
		"uniform vec3 u_Offset;\n"\
		"uniform sampler2D u_sampler_noise;\n"\
		"uniform sampler2D u_sampler_smoke;\n"\
		"uniform float ParticleLifeTime;\n"\
		"in vec3 pos_out;\n"\
		"void main(void)"\
		"{"\
		"vec2 texCoordsNormalized;\n"\
		"texCoordsNormalized.x = (texCoords.x+0.8)/2;\n"\
		"texCoordsNormalized.y = (texCoords.y+0.8)/2;\n"\
		"vec4 finalColor;"\
		"if(out_time <= 1.8f)"\
		"{"\
		"vec4 fireColor = texture(u_sampler_noise, texCoordsNormalized);\n"\
		"finalColor = vec4(mix(vec3(0,0,0),fireColor.xyz,transparacy),fireColor.a);\n"\
		"finalColor.a *= transparacy;\n"\
		"}"\
		"else if(out_time > 1.8f && out_time <= 2.0f)"\
		"{"\
		"vec4 fireColor = texture(u_sampler_noise, texCoordsNormalized);\n"\
		"vec4 smokeColor = texture(u_sampler_smoke, texCoordsNormalized);\n"\
		"vec3 prefinal = mix(fireColor.xyz,smokeColor.xyz,vec3(0.5f,0.5f,0.5f));\n"\
		"finalColor = vec4(mix(vec3(0.0f,0.0f,0.0f),prefinal,transparacy),(fireColor.a+smokeColor.a)/2);\n"\
		"finalColor.a *= transparacy;"\
		"}"\
		"else if(out_time > 2.0f)"\
		"{"\
		"vec4 smokeColor = texture(u_sampler_smoke, texCoordsNormalized);\n"\
		"finalColor = vec4(mix(vec3(0,0,0),smokeColor.xyz,transparacy),smokeColor.a);\n"\
		"finalColor.a *= transparacy;\n"\
		"}"\
		"FragColor = finalColor;"\
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject);
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM FIRE ***
	// create
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-link binding of shader program objeect with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, VELOCITY_ARRAY, "Velocity");
	glBindAttribLocation(gShaderProgramObject, START_TIME_ARRAY, "StartTime");

	// link shader
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	createPoints(2500, 50);
	fprintf_s(gpFile, "w = %d h = %d\n", arrayWidth, arrayHeight);

	// get MVP uniform location
	gModelViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	gLocation = glGetUniformLocation(gShaderProgramObject, "Time");
	gSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_sampler_noise");
	gScaleUniform = glGetUniformLocation(gShaderProgramObject, "u_scale");
	gSkyColorUniform = glGetUniformLocation(gShaderProgramObject, "u_SkyColor");
	gCloudColorUniform = glGetUniformLocation(gShaderProgramObject, "u_CloudColor");
	gOffsetUniform = glGetUniformLocation(gShaderProgramObject, "u_Offset");
	gParticleLifeTimeUniform = glGetUniformLocation(gShaderProgramObject, "ParticleLifeTime");
	uniformSamplerSmoke = glGetUniformLocation(gShaderProgramObject,"u_sampler_smoke");

	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	// Vao
	glGenVertexArrays(1, &gVao); // recorder
	glBindVertexArray(gVao);

	// Vbo_position
	glGenBuffers(1, &gVbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_position);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * 3 * sizeof(float)), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind with Vbo_position

	// Vbo_velocity
	glGenBuffers(1, &gVbo_velocity);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_velocity);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * 3 * sizeof(float)), velocities, GL_STATIC_DRAW);
	glVertexAttribPointer(VELOCITY_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VELOCITY_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind with Vbo_velocity

	// Vbo_starttime
	glGenBuffers(1, &gVbo_starttime);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_starttime);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * sizeof(float)), startTimes, GL_STATIC_DRAW);
	glVertexAttribPointer(START_TIME_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(START_TIME_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind with Vbo_velocity

	glBindVertexArray(0); // Unbind with Vao_pyramid

	// *** VERTEX SHADER CYLINDER ***
	// create shader
	gVertexShaderObject1 = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode1 =
		"#version 460"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform int u_lighting_enabled;"\
		"uniform vec4 u_light_position;"\
		"out vec3 transformed_normals;"\
		"out vec3 light_direction;"\
		"out vec3 viewer_vector;"\
		"void main(void)"\
		"{"\
		"if (u_lighting_enabled == 1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
		"transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"\
		"light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);"\
		"viewer_vector = normalize(-eye_coordinates.xyz);"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject1, 1, (const GLchar **)&vertexShaderSourceCode1, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject1);
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject1, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject1, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADER CYLINDER ***
	// create shader
	gFragmentShaderObject1 = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode1 =
		"#version 460"\
		"\n"\
		"in vec3 transformed_normals;"\
		"in vec3 light_direction;"\
		"in vec3 viewer_vector;"\
		"out vec4 FragColor;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld;"\
		"uniform vec3 u_Ls;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"uniform int u_lighting_enabled;"\
		"void main(void)"\
		"{"\
		"vec3 phong_ads_color;"\
		"if (u_lighting_enabled == 1)"\
		"{"\
		"vec3 normalized_transformed_normals = normalize(transformed_normals);"\
		"vec3 normalized_light_direction = normalize(light_direction);"\
		"vec3 normalized_viewer_vector = normalize(viewer_vector);"\
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);"\
		"vec3 ambient = u_La * u_Ka;"\
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"\
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"\
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"else"\
		"{"\
		"phong_ads_color = vec3(1.0, 1.0, 1.0);"\
		"}"\
		"FragColor = vec4(phong_ads_color, 1.0);"\
		"}";

	glShaderSource(gFragmentShaderObject1, 1, (const GLchar **)&fragmentShaderSourceCode1, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject1);
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject1, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject1, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM CYLINDER ***
	// create
	gShaderProgramObject1 = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject1, gVertexShaderObject1);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject1, gFragmentShaderObject1);

	// pre-link binding of shader program objeect with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject1, VDG_ATTRIBUTE_POSITION, "vPosition");

	// pre-link binding of shader program objeect with vertex shader normal attribute
	glBindAttribLocation(gShaderProgramObject1, VDG_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObject1);
	iShaderProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject1, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject1, iInfoLogLength, &written, szInfoLog);
				fprintf_s(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// get uniform locations
	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject1, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject1, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject1, "u_projection_matrix");

	// L/l key is pressed or not
	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject1, "u_lighting_enabled");

	// ambient color intensity of light
	La_uniform = glGetUniformLocation(gShaderProgramObject1, "u_La");
	// diffuse color intensity of light
	Ld_uniform = glGetUniformLocation(gShaderProgramObject1, "u_Ld");
	// specular color intensity of light
	Ls_uniform = glGetUniformLocation(gShaderProgramObject1, "u_Ls");
	// position of light
	light_position_uniform = glGetUniformLocation(gShaderProgramObject1, "u_light_position");

	// ambient reflective color intensity of material
	Ka_uniform = glGetUniformLocation(gShaderProgramObject1, "u_Ka");
	// diffuse reflective color intensity of material
	Kd_uniform = glGetUniformLocation(gShaderProgramObject1, "u_Kd");
	// specular reflective color intensity of material
	Ks_uniform = glGetUniformLocation(gShaderProgramObject1, "u_Ks");
	// shininess of material (value is conventionally between 1 to 200)
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject1, "u_material_shininess");

	make_cylinder(0.75f, 3.0f, 30, 30);

	glShadeModel(GL_SMOOTH);
	// set-up depth buffer
	glClearDepth(1.0f);
	// deth test to do
	//glDepthFunc(GL_LEQUAL);
	// set really nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// We will always cull back faces for better performance
	//glEnable(GL_CULL_FACE);

	// New Change Taken From David Wolff
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glPointSize(20.0f);
	init3DNoiseTexture();
	//glEnable(GL_TEXTURE_3D); // enable texture mapping
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_PROGRAM_POINT_SIZE); // enable point size used in shader

	flameTexturePng = SOIL_load_OGL_texture("fire.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
	if (flameTexturePng == 0)
	{
		MessageBox(NULL,TEXT("Could Not Load PNG"),TEXT("Error"),MB_ICONERROR);
		uninitialize();
		exit(0);
	}

	smokeTexturePng = SOIL_load_OGL_texture("smoke.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
	if (smokeTexturePng == 0)
	{
		MessageBox(NULL, TEXT("Could Not Load PNG Smoke"), TEXT("Error"), MB_ICONERROR);
		uninitialize();
		exit(0);
	}

	// set background color to which it will display even if it will empty.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black

	// set orthographicMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// resize
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void init3DNoiseTexture(void)
{
	// function declaration
	void make3DNoiseTexture(void);

	// code
	make3DNoiseTexture();

	glGenTextures(1, &gNoise3DTexName);
	glBindTexture(GL_TEXTURE_3D, gNoise3DTexName);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // as we are not using so use GL_NEAREST
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, noise3DTexSize, noise3DTexSize, noise3DTexSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise3DTexPtr);
}

void make3DNoiseTexture(void)
{
	void uninitialize(void);

	// code
	int f, i, j, k, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	float ni[3];
	double inci, incj, inck;
	int frequency = startFrequency;
	GLubyte *ptr;
	double amp = 0.45;

	if ((noise3DTexPtr = (GLubyte *)malloc(noise3DTexSize * noise3DTexSize * noise3DTexSize * 4)) == NULL)
	{
		fprintf_s(gpFile, "ERROR: Could not allocate 3D noise texture\n");
		uninitialize();
		exit(0);
	}

	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		setNoiseFrequency(frequency);
		ptr = noise3DTexPtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (noise3DTexSize / frequency);
		for (i = 0; i < noise3DTexSize; ++i, ni[0] += inci)
		{
			incj = 1.0 / (noise3DTexSize / frequency);
			for (j = 0; j < noise3DTexSize; ++j, ni[1] += incj)
			{
				inck = 1.0 / (noise3DTexSize / frequency);
				for (k = 0; k < noise3DTexSize; ++k, ni[2] += inck, ptr += 4)
				{
					*(ptr + inc) = (GLubyte)(((noise3(ni) + 1.0) * amp) * 128.0);
				}
			}
		}
	}
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// start using OpenGL program object
	//static float a = -3.0;
	
	if (gStartAnimation)
	{
		a += 0.01;
		glUseProgram(gShaderProgramObject);

		glUniform1f(gScaleUniform, 1.2f);
		glUniform3f(gSkyColorUniform, 0.6f, 0.8f, 0.0f);
		glUniform3f(gCloudColorUniform, 0.6f, 0.1f, 0.0f);
		glUniform1f(gParticleLifeTimeUniform, 2.5f);

		float offset[] = { 0.0f, 0.0f, 0.0f };

		static float offset_inc = 0.0f;
		//offset_inc += 0.00006f;
		offset_inc -= 0.0005f;

		// Change offset to animate noise over th surface
		offset[0] = offset_inc;
		offset[1] = 0.0f;
		offset[2] = 0.0f;

		glUniform3fv(gOffsetUniform, 1, (GLfloat *)offset);

		// OpenGL Drawing
		mat4 modelViewMatrix;
		mat4 modelViewProjectionMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		// set modelview, modelviewprojection and rotation matrices to identity
		modelViewMatrix = mat4::identity();
		rotationMatrix = mat4::identity();
		scaleMatrix = mat4::identity();

		modelViewMatrix = translate(0.0f, -0.5f+2.0f, -5.0f);

		rotationMatrix = rotate(120.0f, 1.0f, 0.0f, 0.0f); // Y axis rotation
	
		modelViewMatrix = modelViewMatrix * rotationMatrix;

		// multiply the modelview and orthographic matrix to get modelviewprojection matrix
		modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // Order is Important

		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		glUniformMatrix4fv(gModelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix); // 1 for how many matrices
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix); // 1 for how many matrices

		glUniform1f(gLocation, ParticleTime);

		// *** bind vao ***
		glBindVertexArray(gVao);

		// bind with texture
		glActiveTexture(GL_TEXTURE0); // 0th texture corresponds to VDG_ATTTRIBUTE_TEXTURE0
		glBindTexture(GL_TEXTURE_2D, flameTexturePng);
		glUniform1i(gSamplerUniform, 0); // 0th sampler enable (as we have only 1 texture sampler in fragment shader)
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, smokeTexturePng);
		glUniform1i(uniformSamplerSmoke, 1);

		glDrawArrays(GL_POINTS, 0, arrayWidth * arrayHeight);

		// *** unbind vao ***
		glBindVertexArray(0);

		// stop using OpenGL program object
		glUseProgram(0);
	}

	if (gStartAnimation) {
	glUseProgram(gShaderProgramObject1);

	if (gbLight == true)
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 1);

		// setting light's properties
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);
		glUniform4fv(light_position_uniform, 1, lightPosition);

		// setting material's properties
		glUniform3fv(Ka_uniform, 1, materialAmbient);
		glUniform3fv(Kd_uniform, 1, materialDiffuse);
		glUniform3fv(Ks_uniform, 1, materialSpecular);
		glUniform1f(material_shininess_uniform, materialShininess);
	}
	else
	{
		// reset 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 0);
	}

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	modelMatrix = translate(0.0f, 1.4f+a, -5.0f);
	scaleMatrix = scale(0.7f, 0.7f, 0.7f);

	modelMatrix = modelMatrix * scaleMatrix;

	rotationMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f); // Y axis rotation
	modelMatrix = modelMatrix * rotationMatrix;

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix); // 1 for how many matrices
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix); // 1 for how many matrices
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix); // 1 for how many matrices
	
//	draw_cylinder();
	glUseProgram(0);
	}

	SwapBuffers(ghdc);
}

void update(void)
{
	if (gStartAnimation)
	{
		ParticleTime += 0.08f;
		if (ParticleTime >= 4.5f)
			ParticleTime = 2.5f;
		
		rotAngle = rotAngle + 0.1f;
		if (rotAngle >= 360.0f)
			rotAngle = rotAngle - 360.0f;
		if (a >= 4.0f)
			a = -3.0f;
	}
}

void createPoints(GLint w, GLint h)
{
	GLfloat *vptr, *tptr, *velptr, *stptr;
	GLfloat i, j;
	GLfloat theta, phi;
	int counter = 0;
	float radius = 0.5f;

	if (verts != NULL)
		free(verts);

	verts = (GLfloat *)malloc(w * h * 3 * sizeof(float));
	textures = (GLfloat *)malloc(w * h * 2 * sizeof(float));
	velocities = (GLfloat *)malloc(w * h * 3 * sizeof(float));
	startTimes = (GLfloat *)malloc(w * h * sizeof(float));

	vptr = verts;
	tptr = textures;
	velptr = velocities;
	stptr = startTimes;

	for (i = 0.5 / w - 0.5; i < 0.5; i = i + 1.0 / w) {
		for (j = 0.5 / h - 0.5; j < 0.5; j = j + 1.0 / h)
		{
			theta = mix(0.0f, (float)M_PI / 6.0f, (float)rand());
			phi = mix(0.0f, (float)M_PI * 2 * i * j, (float)rand());

			*vptr = radius * cosf(phi);
			*(vptr + 1) = radius * sinf(phi);
			*(vptr + 2) = 0.0f;
			vptr += 3;

			*tptr = ((float)rand() / RAND_MAX) * 1.0;
			*(tptr + 1) = ((float)rand() / RAND_MAX) * 1.0;
			tptr += 2;

			*velptr = 0.0f;
			*(velptr + 1) = 0.0f;
			*(velptr + 2) = ((float)rand() / RAND_MAX) / 8 ;
			velptr += 3;

			*stptr = ((float)rand() / RAND_MAX) * 5.0;
			stptr++;
			counter++;
		}
	}
		arrayWidth = w;
		arrayHeight = h;
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void uninitialize(void)
{
	// code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	// destroy vbo_position
	if (gVbo_position)
	{
		glDeleteBuffers(1, &gVbo_position);
		gVbo_position = 0;
	}

	// destroy vbo_texure
	if (gVbo_texture)
	{
		glDeleteBuffers(1, &gVbo_texture);
		gVbo_texture = 0;
	}

	// destroy vbo_velocuty
	if (gVbo_velocity)
	{
		glDeleteBuffers(1, &gVbo_velocity);
		gVbo_velocity = 0;
	}

	// destroy vbo_starttime
	if (gVbo_starttime)
	{
		glDeleteBuffers(1, &gVbo_starttime);
		gVbo_starttime = 0;
	}

	// destroy vao
	if (gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}

	// detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	// detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	// delete vertex shader object
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	// delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	// delete shader program object
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	// unlink shader program
	glUseProgram(0);

	// Deselect the rendering context
	wglMakeCurrent(NULL, NULL);

	// Delete the rendering context
	wglDeleteContext(ghrc);
	ghrc = NULL;

	// Delete the devide context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;

	if (gpFile)
	{
		fprintf_s(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}