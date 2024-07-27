#include "VertexShader.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
	GFX_THROW_INFO(
		GetDevice(gfx)->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVertexShader));

}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBlob.Get();
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}
