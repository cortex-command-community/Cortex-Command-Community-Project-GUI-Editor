#include "RTEError.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ShowMessageBox(const std::string &message) { allegro_message(message.c_str()); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAbortFunc(const std::string &description, const std::string &file, int line) {
		// Ditch the video mode so the message box appears without problems
		if (screen != nullptr) { set_gfx_mode(GFX_TEXT, 0, 0, 0, 0); }
		set_window_title("RTE Aborted! (x_x)");

		std::string abortMessage;

#ifdef DEBUG_BUILD
		// Show message box with explanation
		abortMessage = "Runtime Error in file " + file + ", line " + std::to_string(line) + ", because:\n\n" + description + "\n\nThe last frame has been dumped to 'AbortScreen.bmp'";
#else
		// Shortened and less confusing one. users have no use of knowing which source file and where.
		abortMessage = description + "\n\nThe last frame has been dumped to 'AbortScreen.bmp'";
#endif
		ShowMessageBox(abortMessage);

		// True so that the debugbreak code is run and the debugger goes there.
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAssertFunc(bool expression, const char *description, const char *file, int line, bool &alwaysIgnore) {
		if (!expression) {
			// TODO: Make this display a box in the game asking whether to ignore or abort. For now, always abort.
			RTEAbortFunc(description, __FILE__, __LINE__);

			// True so that the debugbreak code is run and the debugger goes there.
			return true;
		}
		return false;
	}
}