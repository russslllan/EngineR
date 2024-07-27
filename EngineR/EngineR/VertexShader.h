#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	ID3DBlob* GetByteCode() const noexcept;
	void Bind(Graphics& gfx) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
};
