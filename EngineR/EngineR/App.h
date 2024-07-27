#pragma once
#include "Window.h"
#include "RusTimer.h"
#include <memory>
#include <vector>
#include "Box.h"
#include <memory>

class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	Window wnd;
	RusTimer timer;
	std::vector<std::unique_ptr<class Box>> pBoxes;
};
