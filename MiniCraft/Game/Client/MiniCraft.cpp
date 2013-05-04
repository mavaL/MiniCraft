#include "stdafx.h"
#include "AppStateManager.h"
#include "Application.h"






Applicaton g_app;



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		g_app.Init();
		g_app.Run();
	}
	catch (Ogre::Exception& e)
	{
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_ICONERROR | MB_TASKMODAL);
	}
	g_app.Shutdown();
	
	return 0;
}
