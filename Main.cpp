#include "GUIEditorApp.h"
#include "GUIEditorLib.h"

using namespace RTE;

extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }
//extern HINSTANCE g_hInstance = 0;
//extern HWND g_hWnd = 0;

/// <summary>
/// Entry point for the GUI Library editor.
/// </summary>
int main(int argc, char *argv[]) {
	//if (!g_GUIEditor.Initialize()) { return -1; }

	// Get windows settings
	//g_hWnd = win_get_window();
	//g_hInstance = (HINSTANCE)GetWindowLong(g_hWnd, GWL_HINSTANCE);

	// Run editor loop
	while (true) {
		bool bContinue = g_GUIEditor.Update();
		if (!bContinue) { break; }
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return main(__argc, __argv); }