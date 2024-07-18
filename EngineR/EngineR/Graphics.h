#pragma once

#include "RusWin.h"
#include <d3d11.h>
#include "RusExceptinon.h"
#include <vector>
#include "DxgiInfoManager.h"
#include <wrl.h>
//#include "Macros.h"

class Graphics
{
public:
	//class Exception : public RusException
	//{
	//	using RusException::RusException;
	//};
	class HrException : public RusException
	{
	public:
		HrException(
			int line,
			const char* file,
			HRESULT hr
			/*,std::vector<std::string> infoMsgs*/) noexcept;
		HrException(
			int line,
			const char* file,
			HRESULT hr,
			std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public RusException
	{
	public:
		InfoException(int line, const char* file,std::vector<std::string> infoMsgs);
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTestTriangle();


private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device>           pDevice;          
	Microsoft::WRL::ComPtr<IDXGISwapChain>         pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};
