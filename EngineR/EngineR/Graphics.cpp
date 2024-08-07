#include "Graphics.h"
#include "DXErr.h"
#include <sstream>
#include <locale> 
#include <codecvt>
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ConstantBuffer.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx  = DirectX;

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
	sd.OutputWindow = hWnd;
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

	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(
		0, 
		__uuidof(ID3D11Resource),
		&pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		0,
		&pTarget));


	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = TRUE;
	dsd.StencilEnable = FALSE;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	wrl::ComPtr<ID3D11DepthStencilState> pDSState;

	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsd, &pDSState));

	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);


	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.Width = 800;
	descDepth.Height = 600;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.CPUAccessFlags = 0u;
	descDepth.MiscFlags = 0u;

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &pDSV));

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
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
		pTarget.Get(),
		color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	HRESULT hr;

	struct Vertex
	{
		struct {
			float x;
			float y;
			float z;
		} pos;
	};

	const Vertex vertices[] =
	{
		{-1.0f, -1.0f,  -1.0f},
		{ 1.0f, -1.0f , -1.0f},
		{-1.0f,  1.0f , -1.0f},
		{ 1.0f,  1.0f,  -1.0f},
		{-1.0f, -1.0f,   1.0f},
		{ 1.0f, -1.0f,   1.0f},
		{-1.0f,  1.0f,   1.0f},
		{ 1.0f,  1.0f,   1.0f},
	};

	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertices);
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &vertexBuffer));
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	const unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(indices);
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob>           pBlob1, pBlob2;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob1));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob1->GetBufferPointer(), pBlob1->GetBufferSize(), nullptr, &pVertexShader));
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	GFX_THROW_INFO(pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob1->GetBufferPointer(), pBlob1->GetBufferSize(), & pInputLayout));
	pContext->IASetInputLayout(pInputLayout.Get());

	struct ConstantBufferVS
	{
		dx::XMMATRIX transform;
	};

	float aspect = 3.0f / 4.0f;

	float normalizedX = x / 400 - 1.0f;
	float normalizedY = -y / 300.0f + 1.0f;
	
	const ConstantBufferVS trans
	{
		{dx::XMMatrixTranspose(
			dx::XMMatrixRotationY(angle) *
			dx::XMMatrixRotationX(angle)*
			/*dx::XMMatrixScaling(aspect, 1.0f, 1.0f) * */
			dx::XMMatrixTranslation(normalizedX, normalizedY, 4.0f) * 
			dx::XMMatrixPerspectiveLH(1.0f, aspect, 0.5f, 10.0f))
		}
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(trans);
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &trans;

	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"pixelShader.cso", &pBlob2));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob2->GetBufferPointer(), pBlob2->GetBufferSize(), nullptr, &pPixelShader));
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	struct ConstantBufferPS
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const ConstantBufferPS cbuffer = 
	{
		{
			{1.0f,0.0f,1.0f},
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
			{1.0f,1.0f,0.0f},
			{0.0f,1.0f,1.0f}
		}
	};

	D3D11_BUFFER_DESC cbdPS = {};
	cbdPS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbdPS.Usage = D3D11_USAGE_DYNAMIC;
	cbdPS.ByteWidth = sizeof(cbuffer);
	cbdPS.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;
	cbdPS.MiscFlags = 0u;
	cbdPS.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csdPS = {};
	csdPS.pSysMem = &cbuffer;

	wrl::ComPtr<ID3D11Buffer> pConstantBufferPS;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbdPS, &csdPS, &pConstantBufferPS));

	pContext->PSSetConstantBuffers(0u, 1u, pConstantBufferPS.GetAddressOf());


	D3D11_VIEWPORT vp;
	vp.Width    = 800;
	vp.Height   = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0));
}

void Graphics::DrawIndexed(UINT count) noexcept
{
	pContext->DrawIndexed(count, 0u, 0u);
}

Graphics::HrException::HrException(
	int line, 
	const char * file,
	HRESULT hr 
	/*,std::vector<std::string> infoMsgs*/) noexcept
	:
	RusException(line, file),
	hr(hr)
{

}

Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	RusException(line, file),
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
	int len = WideCharToMultiByte(CP_ACP, 0, &wstr[0], int(wstr.length()), 0, 0, 0, 0);
	std::string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length()), &str[0], len, 0, NULL);
	return str;
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	std::wstring wstr(512, '\0');
	DXGetErrorDescription(hr, &wstr[0], 512);
	size_t lenStr = wcslen(wstr.c_str());
	int len = WideCharToMultiByte(CP_ACP, 0, &wstr[0], int(lenStr), 0, 0, 0, 0);
	std::string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length()), &str[0], len, 0, NULL);
	return str;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Rus Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs)
	:
	RusException(line, file)
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

const char* Graphics::InfoException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
	    << "\n[Error Info]\n" << GetErrorInfo() << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "RUS Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
