#include "Drawable.h"

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for (auto& b : binds)
	{
		std::unique_ptr<int> ptr;
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer != nullptr);
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}


