#pragma once
#include "Window.h"
#include "RusTimer.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	RusTimer timer;
};
