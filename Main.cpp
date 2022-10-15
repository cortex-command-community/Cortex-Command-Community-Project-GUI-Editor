#include "EditorApp.h"
#include "EditorUtil.h"
#include "ContentFile.h"
#include "winalleg.h"
#include "loadpng.h"

using namespace RTEGUI;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Entry point for the GUI editor app.
/// </summary>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//allegro_init();
	//loadpng_init();
	//g_GUIEditor.Initialize();

	//std::chrono::steady_clock::time_point frameTimeStart;
	//while (true) {
	//	if (!g_GUIEditor.UpdateEditor()) {
	//		break;
	//	}
	//	frameTimeStart = std::chrono::high_resolution_clock::now();
	//	vsync();
	//	g_GUIEditor.DrawEditor();
	//	EditorApp::s_FrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - frameTimeStart).count();
	//}
	//g_GUIEditor.DestroyBackBuffers();
	//ContentFile::FreeAllLoaded();

	std::exit(EXIT_SUCCESS);
}