#pragma once

#include "Window.h"

//#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__, __FILE__, (hr))
#define WINDOW_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, (hr))
#define WINDOW_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)

