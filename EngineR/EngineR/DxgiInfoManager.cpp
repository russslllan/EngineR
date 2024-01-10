#include "DxgiInfoManager.h"
#include "Graphics.h"
#include "Macros.h"

#pragma comment(lib, "dxguid.lib")


DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (hModDxgiDebug == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	const auto DxgiGetDebugInterface =
		reinterpret_cast<DXGIGetDebugInterface>(
			reinterpret_cast<void*>(
				GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	if (DxgiGetDebugInterface == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(
		DxgiGetDebugInterface(
			__uuidof(IDXGIInfoQueue),
			reinterpret_cast<void**>(pDxgiInfoQueue)));
}

DxgiInfoManager::~DxgiInfoManager()
{
	if (pDxgiInfoQueue != nullptr)
	{
		pDxgiInfoQueue->Release();
	}
}

void DxgiInfoManager::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(
			DXGI_DEBUG_ALL,
			i, nullptr,
			&messageLength));

		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}
