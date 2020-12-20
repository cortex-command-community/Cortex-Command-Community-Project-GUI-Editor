#ifndef _GUIEDITORLIB_
#define _GUIEDITORLIB_

namespace RTEGUI {

	/// <summary>
	/// GUI Editor Library class with static functions to handle windows calls
	/// </summary>
	class GUIEditorUtil {

	public:

		/// <summary>
		/// Display quit program OS dialog box.
		/// </summary>
		/// <param name="message">Message to display in the dialog box.</param>
		/// <param name="title">The title of the main editor window. Used to get the window handle.</param>
		/// <returns>1 for quit & save, -1 for quit, no save, 0 for cancel.</returns>
		static int QuitMessageBox(const std::string &message, const std::string &title);

		/// <summary>
		/// Display load GUI file OS dialog box.
		/// </summary>
		/// <param name="filename">File name to load.</param>
		/// <param name="title">The title of the main editor window. Used to get the window handle.</param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplayLoadGUIFile(std::string *filename, const std::string &title);

		/// <summary>
		/// Display save GUI file OS dialog box.
		/// </summary>
		/// <param name="filename">File name to save.</param>
		/// <param name="title">The title of the main editor window. Used to get the window handle.</param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplaySaveGUIFile(std::string *filename, const std::string &title);
	};
}
#endif