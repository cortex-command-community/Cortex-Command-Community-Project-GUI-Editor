#ifndef _GUIEDITORLIB_
#define _GUIEDITORLIB_

namespace RTE {

	/// <summary>
	/// GUI Editor Library class with static functions to handle windows calls
	/// </summary>
	class GUIEditorLib {

	public:

		/// <summary>
		/// Call quit messagebox.
		/// </summary>
		/// <param name="strMessage"></param>
		/// <param name="strTitle"></param>
		/// <returns>1 for quit & save, -1 for quit, no save, 0 for cancel.</returns>
		static int QuitMessageBox(std::string strMessage, std::string strTitle);

		/// <summary>
		/// Display load GUI file OS dialog box.
		/// </summary>
		/// <param name="strFilename"></param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplayLoadGUIFile(std::string *strFilename);

		/// <summary>
		/// Display save GUI file OS dialog box.
		/// </summary>
		/// <param name="strFilename"></param>
		/// <returns>True if a file was selected.</returns>
		static bool DisplaySaveGUIFile(std::string *strFilename);
	};
}
#endif