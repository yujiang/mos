#include "window.h"
#include "gdi/window_render_gdi.h"
#include "opengl/window_render_gl.h"
#include <windows.h>
#include "script.h"
#include <string>
#include <conio.h>
#include "windowsx.h"
#include "mos.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int xPos,yPos;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//printf("WM_PAINT\n");
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		g_window->on_destroy();
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		lua_call_va("on_key_down","ibbb",wParam,is_key_down(VK_MENU),is_key_down(VK_CONTROL),is_key_down(VK_SHIFT));
		break;
	case WM_MOUSEWHEEL:
		//printf("WM_MOUSEWHEEL %d\n",GET_WHEEL_DELTA_WPARAM(wParam));
		lua_call_va("on_mouse_wheel","iii",
			GET_WHEEL_DELTA_WPARAM(wParam),
			get_window()->get_mouse_point().x,
			get_window()->get_mouse_point().y);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		xPos = GET_X_LPARAM(lParam); 
		yPos = GET_Y_LPARAM(lParam); 
		int type;
		switch (message)
		{
		case WM_LBUTTONDOWN:
			type = 1;
			break;
		case WM_LBUTTONUP:
			type = 2;
			break;
		case WM_RBUTTONDOWN:
			type = 3;
			break;
		case WM_RBUTTONUP:
			type = 4;
			break;
		case WM_MOUSEMOVE:
			type = 5;
			break;
		}
		get_window()->set_mouse_point(g_point(xPos,yPos));
		lua_call_va("on_mouse_event","iiii",type,xPos,yPos,wParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance,const char* szWindowClass)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex,sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN);
	wcex.lpszClassName	= szWindowClass;
	//wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, const char* szWindowClass, const char* szTitle,st_window_param& param)
{
	HWND hWnd;

	//hInst = hInstance; // Store instance handle in our global variable
	DWORD style=WS_OVERLAPPEDWINDOW&~WS_SIZEBOX&~WS_MAXIMIZEBOX;
	RECT rect={0,0,param.width(),param.height()};
	AdjustWindowRect(&rect,style,false);

	hWnd = CreateWindow(szWindowClass, szTitle, style,
		param.l, param.t,  rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return NULL;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}


//////////////////////////////////////////////////////////////////////////

window::window() 
{
	m_destroy = false;
	m_render = 0;
}

window::~window()
{
	delete m_render;
	m_render = 0;
}

void window::on_destroy()
{
	m_destroy = true;
	m_render->on_destroy();
}

bool window::create_window(const char* name,const char* title,st_window_param& st)
{
	m_name = name;
	m_param = st;

	//win32 code
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance,name);
	m_hWnd = InitInstance (hInstance, name,title,st);
	if (!m_hWnd)
	{
		return FALSE;
	}

	RECT rc;
	GetClientRect((HWND)m_hWnd,&rc);
	
	//create offscreen buf
	m_width = (rc.right-rc.left);
	m_height = (rc.bottom-rc.top);

	if (st.render_type == "opengl")
		m_render = new window_render_gl(this);
	else
		m_render = new window_render_gdi(this);
	return m_render->create_render(m_width,m_height);
}

//////////////////////////////////////////////////////////////////////////
//static func
void window::message_loop()
{
	// Main message loop:
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

const char* get_line_timeout(unsigned int timeout)
{
	unsigned int start = get_time();
	static char buffer[1024] = {0};

	int n = 0;

	for (; ; ) {
		if (n == 0 && get_time() - start >= timeout)
			return "";
		if (_kbhit()) {
			gets_s(buffer, 1024);
			break;
		}
		Sleep(0);
	}
	return buffer;
}

unsigned long get_time()
{
	return timeGetTime();
}

bool is_key_down(int key)
{
	return (GetKeyState(key)&0x8000) != 0;
}

//////////////////////////////////////////////////////////////////////////
//use a thread
HANDLE g_threadInput = NULL;
static char buffer[1024] = {0};

DWORD WINAPI InputLoop(void * param)
{
	for (; ; ) 
	{
		gets_s(buffer, 1024);
		Sleep(100);
	}
}

HANDLE begin_thread(LPTHREAD_START_ROUTINE Thread, LPVOID lParam , int nPriority = THREAD_PRIORITY_NORMAL)
{
	nPriority;
	DWORD id;
	HANDLE handle = CreateThread(NULL,0,Thread,lParam,0,&id);
	return handle;
}

const char* get_input_string()
{
	static std::string s;
	if (g_threadInput == NULL)
	{
		g_threadInput = begin_thread(InputLoop,0,0);
		return "";
	}
	s = buffer;
	buffer[0] = 0;
	return s.c_str();
}


