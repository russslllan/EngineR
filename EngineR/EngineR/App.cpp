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
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Timer elapsed: " << t;
	std::cout << oss.str();
	//wnd.SetTitle(oss.str()); // Not recommended! Because the window freezes
}
