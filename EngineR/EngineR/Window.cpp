#include "Window.h"
#include "resource.h"
#include <sstream>
#include <locale> 
#include <codecvt>
#include "DXErr.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() 
	:
hInst(GetModuleHandle(NULL))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = reinterpret_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 48, 48, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = reinterpret_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	
	if (!RegisterClassEx(&wc))
	{
		WINDOW_LAST_EXCEPT();
	}
}

Window::WindowClass::~WindowClass() noexcept
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	LONG edge = 100;
	RECT rc = { edge, edge, width + edge, height + edge };
	DWORD windowFlag = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	AdjustWindowRect(&rc, windowFlag, FALSE);
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		windowFlag,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this);

	if (hWnd == nullptr)
	{
		WINDOW_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window() noexcept
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string & title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		WINDOW_LAST_EXCEPT();
	}
}

bool Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			return false;
		}
	}
	return true;
}

Graphics & Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		//HICON hIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0));
		//SendMessage(hWnd, WM_SETICON, 1, reinterpret_cast<LPARAM>(hIcon));
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<char>(wParam));
		break;
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		if (0 <= pt.x && width > pt.x && 0 <= pt.y && height > pt.y)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
		const POINTS pt = MAKEPOINTS(lParam);
		//const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		//mouse.OnWheelDelta(pt.x, pt.y, delta);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(pt.x, pt.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	RusException(line, file),
	hr(hr)
{}


const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::HrException::GetType() const noexcept
{
	return "Rus Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD nMsgLen = FormatMessage(
		flags,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr);
	if (nMsgLen == 0)
		return "Unidentified error code";
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	char* pMsgBuf = nullptr;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD nMsgLen = FormatMessage(
		flags,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr);
	if (nMsgLen == 0)
		return "Unidentified error code";
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Rus Graphics Exception [Error creating Graphics]";
}