#pragma once

#include "Window.h"
#include "Graphics.h"

#define GFX_THROW_FAILED(hrcall) if(FAILED( hr = (hrcall))) throw Graphics::HrException(__LINE__,__FILE__,(hrcall))
//#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__, __FILE__, (hr))
#define WINDOW_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, (hr))
#define WINDOW_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)
//#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hrcall))
