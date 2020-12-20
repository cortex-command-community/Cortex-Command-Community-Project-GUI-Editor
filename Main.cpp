#include "GUIEditorApp.h"
#include "GUIEditorUtil.h"
#include "ContentFile.h"
#include "allegro.h"
#include "winalleg.h"

using namespace RTEGUI;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Entry point for the GUI editor app.
/// </summary>
int main(int argc, char **argv) {
	allegro_init();
	if (!g_GUIEditor.Initialize()) { std::exit(EXIT_FAILURE); }

	while (true) {
		g_GUIEditor.ClearBackBuffer();
		if (!g_GUIEditor.Update()) {
			break;
		}
		g_GUIEditor.FlipFrameBuffers();
	}
	ContentFile::FreeAllLoaded();

	std::exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return main(__argc, __argv); }