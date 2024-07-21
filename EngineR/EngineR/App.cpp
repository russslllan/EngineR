#include "App.h"
#include <sstream>
#include <iostream>

App::App()
	:
	wnd(800, 600, "The Donkey Fart box")
{
}

int App::Go()
{
	while(wnd.ProcessMessages())
	{
		DoFrame();
	}
	return 0;
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(),
		400.0f,
		300.0f);
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(),
		float(wnd.mouse.GetPosX()),
		float(wnd.mouse.GetPosY()));
	wnd.Gfx().EndFrame();
}
