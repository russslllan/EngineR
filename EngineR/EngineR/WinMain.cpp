#include "Window.h"
#include <sstream>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdShow,
	int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdShow);

	try
	{
		Window wnd1(800, 600, "Engine");
		//Window wnd2(400, 300, "Engine2");

		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, NULL, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			while (!wnd1.mouse.IsEmpty())
			{
				auto e = wnd1.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::Leave:
					wnd1.SetTitle("Gone!");
					break;
				case Mouse::Event::Type::Move:
					{
						std::ostringstream oss;
						oss << "Mouse moved to(" << e.GetPosX()
							<< " ," << e.GetPosY() << ")";
						wnd1.SetTitle(oss.str());
					}
					break;
				}
			}
			
			/*std::ostringstream os;
			os << "Mouse position "
				<< "(" << e.GetPosX() 
				<< "," << e.GetPosY() 
				<< ")";
			if (e.GetType() == Mouse::Event::Type::Enter)
				wnd1.SetTitle(os.str());
			else if (e.GetType() == Mouse::Event::Type::Leave)
				wnd1.SetTitle(std::string("Gone!"));*/
		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
	}
	catch (const RusException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}