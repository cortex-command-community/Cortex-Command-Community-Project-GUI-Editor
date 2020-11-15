#include "GUIEditorApp.h"
#include "GUIEditorLib.h"
#include "ContentFile.h"
#include "TimerMan.h"

using namespace RTE;

extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Quit Handler for Allegro.
/// </summary>
void QuitHandler(void) { g_GUIEditor.OnQuitButton(); }
END_OF_FUNCTION(QuitHandler)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Entry point for the GUI editor app.
/// </summary>
int main(int argc, char *argv[]) {
	allegro_init();

	// Enable the exit button on the window
	LOCK_FUNCTION(QuitHandler);
	set_close_button_callback(QuitHandler);

	new TimerMan();
	g_TimerMan.Create();

	new GUIEditorApp();
	if (!g_GUIEditor.Initialize()) {
		std::exit(EXIT_FAILURE);
	}

	while (true) {
		if (!g_GUIEditor.Update()) {
			break;
		}
	}

	g_TimerMan.Destroy();
	ContentFile::FreeAllLoaded();

	std::exit(EXIT_SUCCESS);
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return main(__argc, __argv); }