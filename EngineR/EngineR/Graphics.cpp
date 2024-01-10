#include "Graphics.h"
#include "DXErr.h"
#include <sstream>
#include <locale> 
#include <codecvt>

#pragma comment(lib, "d3d11.lib")

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__,__FILE__,(hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED( hr = hrcall)) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__, __FILE__, (hr), infoManager.GetMessages())
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException(__LINE__,__FILE__,(hr),infoManager.getMessages())
#endif


#ifdef _DEBUG
	UINT flag = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flag = 0;
#endif

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = (HWND)696969;//hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;
	// Create device and front/back buffers, and swap chain  and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flag,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext));

	ID3D11Resource* pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(
		0, 
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(&pBackBuffer)));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer,
		0,
		&pTarget));
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pSwap != nullptr)
	{
		pSwap->Release();
	}
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
	
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[4] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(
		pTarget,
		color);
}

Graphics::HrException::HrException(
	int line, 
	const char * file,
	HRESULT hr, 
	std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::HrException::HrException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
const char* Graphics::HrException::HrException::GetType() const noexcept
{
	return "Rus Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	std::wstring wstr = DXGetErrorString(hr);
	int len = WideCharToMultiByte(CP_ACP, 0, &wstr[0], wstr.length(), 0, 0, 0, 0);
	std::string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), &str[0], len, 0, NULL);
	return str;
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	std::wstring wstr(512, '\0');
	DXGetErrorDescription(hr, &wstr[0], wstr.length());
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), 0, 0, 0, 0);
	std::string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), &str[0], len, 0, NULL);
	return str;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return std::string();
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Rus Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
