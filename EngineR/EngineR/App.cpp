#include "App.h"
#include <sstream>
#include <iostream>


App::App()
	:
	wnd(800, 600, "The Donkey Fart box")
{
	const float PI = 3.1415926535f;
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, PI * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, PI * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, PI * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		pBoxes.push_back(std::make_unique<Box>(
		wnd.Gfx(),rng,adist,
			ddist,odist,rdist));
	}
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 100.0f));
}

int App::Go()
{
	while(wnd.ProcessMessages())
	{
		DoFrame();
	}
	return 0;
}

App::~App()
{
}

void App::DoFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : pBoxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}
