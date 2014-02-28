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
LARGE_INTEGER ws_frequency = {1,1};
__int64 _org_time;

window::window() 
{
	m_destroy = false;
	m_render = 0;

	LARGE_INTEGER cur_time;
	QueryPerformanceFrequency(&ws_frequency);
	QueryPerformanceCounter(&cur_time);
	_org_time = cur_time.QuadPart;
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

double get_time_ex()
{
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	return (double)(cur_time.QuadPart-_org_time) * 1000 / ws_frequency.QuadPart;
}

bool is_key_down(int key)
{
	return (GetKeyState(key)&0x8000) != 0;
}

//////////////////////////////////////////////////////////////////////////
//use a thread
extern bool g_exit;
static char buffer[1024] = {0};

#include "core/wait_notify.h"

std::thread* g_threadInput = NULL;
std::mutex   g_mx_input;
bool g_input_threadexit = true;

void input_loop()
{
	g_input_threadexit = false;
	while(!g_exit) 
	{
		{
			//std::lock_guard<std::mutex> lock(g_mx_input);
			gets_s(buffer);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_input_threadexit = true;
}


const char* get_input_string()
{
	if (!g_threadInput) 
	{
		g_threadInput = new std::thread(input_loop);
		//g_threadInput->detach();
		return "";
	}
	static std::string s;
	s.clear();
	//if (g_mx_input.try_lock())
	{
		s = buffer;
		buffer[0] = 0;
		//g_mx_input.unlock();
		return s.c_str();
	}
	return "";
}

void write_stdin_return()
{
	HANDLE conIn = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD irec[2];
	irec[0].EventType = KEY_EVENT;
	irec[0].Event.KeyEvent.bKeyDown = TRUE;
	irec[0].Event.KeyEvent.dwControlKeyState = 0;
	irec[0].Event.KeyEvent.uChar.AsciiChar = '\r';
	irec[0].Event.KeyEvent.wRepeatCount = 1;
	irec[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN; /* virtual keycode is always uppercase */
	irec[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKeyA(VK_RETURN & 0x00ff, 0);
	irec[1] = irec[0];
	irec[1].Event.KeyEvent.bKeyDown = FALSE;
	DWORD dw;
	BOOL rt = WriteConsoleInput(conIn, irec, 2 , &dw);
}

void end_thread_input()
{
	//because get_s£¬there is no way safe delete thread.
	if (g_threadInput)
	{
		write_stdin_return();
		g_threadInput->join();
		delete g_threadInput;
		g_threadInput = NULL;
	}
}

