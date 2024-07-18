#pragma once

#include "Window.h"
#include "Graphics.h"

#define GFX_THROW_FAILED(hrcall) if(FAILED( hr = (hrcall))) throw Graphics::HrException(__LINE__,__FILE__,(hr))
//#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__, __FILE__, (hr))
#define WINDOW_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, (hr))
#define WINDOW_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)
//#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__,__FILE__,(hr))

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED( hr = hrcall)) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); { auto v = infoManager.GetMessages(); if(!v.empty()) throw Graphics::InfoException(__LINE__, __FILE__, v); }
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException(__LINE__,__FILE__,(hr),infoManager.getMessages())
#define GFX_THROW_INFO_ONLY(call) (call)
#endif
