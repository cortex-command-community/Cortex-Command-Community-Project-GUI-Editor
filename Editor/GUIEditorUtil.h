#ifndef _GUIEDITORLIB_
#define _GUIEDITORLIB_

// Forward declaration for HWND
struct HWND__;
using HWND = HWND__*;

namespace RTEGUI {

	/// <summary>
	/// GUI Editor Library class with static functions to handle Windows calls.
	/// </summary>
	class GUIEditorUtil {

	public:

		/// <summary>
		/// Display quit program OS dialog box.
		/// </summary>
		/// <param name="message">Message to display in the dialog box.</param>
		/// <param name="windowHandle">The window handle of the editor process.</param>
		/// <returns>1 for quit & save, -1 for quit, no save, 0 for cancel.</returns>
		static int QuitMessageBox(const std::string &message, const HWND &windowHandle);

		/// <summary>
		/// Display load GUI file OS dialog box.
		/// </summary>
		/// <param name="filename">File name to load.</param>
		/// <param name="windowHandle">The window handle of the editor process.</param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplayLoadGUIFile(std::string *filename, const HWND &windowHandle);

		/// <summary>
		/// Display save GUI file OS dialog box.
		/// </summary>
		/// <param name="filename">File name to save.</param>
		/// <param name="windowHandle">The window handle of the editor process.</param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplaySaveGUIFile(std::string *filename, const HWND &windowHandle);
	};
}
#endif