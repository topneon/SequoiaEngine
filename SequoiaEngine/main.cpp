#include "Application.h"

// start point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	seq::Application app{};
	app.Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	// delete app;
	return 0;
}