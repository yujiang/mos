#include "window.h"
#include <windows.h>
#include "script.h"
#include <string>
#include <conio.h>
#include "graph/texture.h"
#include "graph/image.h"
#include "windowsx.h"
#include "mos.h"
#pragma comment(lib,"winmm.lib")

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
window::~window()
{
	delete m_image;
	m_image = 0;
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
	int width = (rc.right-rc.left);
	int height = (rc.bottom-rc.top);
	m_image = new image;
	m_image->create_image_dynamic(width,height,3);

	return true;
}

void window::render_start()
{
	//clear(0);
	m_image->clear(0);
}

void window::render_end()
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi,sizeof(bmi));
	BITMAPINFOHEADER& h = bmi.bmiHeader;
	h.biSize = sizeof(BITMAPINFOHEADER);
	h.biWidth = m_image->m_width;
	h.biHeight = -m_image->m_height;
	h.biPlanes = 1;
	h.biBitCount = 24;
	h.biCompression = BI_RGB;
	h.biSizeImage = m_image->m_width * m_image->m_height;

	//flip();
	HDC dc = GetDC((HWND)m_hWnd);
	int err = SetDIBitsToDevice(dc,0,0,m_image->m_width,m_image->m_height,
		0,0,0,m_image->m_height,
		m_image->get_buffer() ,
		&bmi,
		DIB_RGB_COLORS);
	if (err <= 0)
	{
		//GDI_ERROR
		printf("error! SetDIBitsToDevice %d %d\n",err,GetLastError());
	}
	ReleaseDC((HWND)m_hWnd,dc);
}

int window::draw_texture(const st_cell& cell,texture* tex, const g_rect* rc)
{
	return m_image->draw_image(cell,tex->m_image,rc,m_rc_clip);
}

int window::draw_box(const st_cell& cell,int w,int h)
{
	return m_image->draw_box(cell,w,h);
}

//int window::draw_text(const st_cell& cell,const st_cell& text)
//{
//	return 0;
//}

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
	return GetKeyState(key)&0x8000;
}

