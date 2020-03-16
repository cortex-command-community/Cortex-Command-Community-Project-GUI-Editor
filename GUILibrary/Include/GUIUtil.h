#ifndef _GUIUTIL_
#define _GUIUTIL_

namespace RTE {

#pragma region Rectangle Structure
	/// <summary>
	/// The GUIRect structure defines a rectangle by the coordinates of its upper-left and lower-right corners.
	/// </summary>
	struct GUIRect { long left; long top; long right; long bottom; };

	/// <summary>
	/// Sets the bounds of a GUIRect.
	/// </summary>
	/// <param name="pRect">Pointer to the GUIRect.</param>
	/// <param name="left">Position of top left corner on X axis.</param>
	/// <param name="top">Position of top left corner on Y axis.</param>
	/// <param name="right">Position of bottom right corner on X axis.</param>
	/// <param name="bottom">Position of bottom right corner on Y axis.</param>
	void SetRect(GUIRect *pRect, int left, int top, int right, int bottom);
#pragma endregion

#pragma region GUI Utility Class
	/// <summary>
	/// A utility class with misc static functions for different things.
	/// </summary>
	class GUIUtil {

	public:

		/// <summary>
		/// Constructor method used to instantiate a GUIUtil object in system memory.
		/// </summary>
		GUIUtil() {};

		/// <summary>
		/// Removes the preceding and ending spaces from a C type string.
		/// </summary>
		/// <param name="String">String.</param>
		/// <returns>Trimmed string.</returns>
		static char *TrimString(char *String);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dst">Destination string.</param>
		/// <param name="src">Source string.</param>
		/// <returns></returns>
		static char* SafeOverlappingStrCpy(char* dst, char* src);

		/// <summary>
		/// Gets the text from the clipboard.
		/// </summary>
		/// <param name="Text">Pointer to string receiving the text.</param>
		/// <returns>Boolean. True if text was available in the clipboard.</returns>
		static bool GetClipboardText(std::string *Text);

		/// <summary>
		/// Sets the text in the clipboard.
		/// </summary>
		/// <param name="Text">String to put into the clipboard.</param>
		/// <returns>Boolean. True if text was added to the clipboard.</returns>
		static bool SetClipboardText(std::string Text);
	};
#pragma endregion

#pragma region Windows Specific GUI Utilities
#ifdef WIN32
	/// <summary>
	/// A utility class with misc static functions for different things, platform specific to Windows.
	/// </summary>
	class WinUtil {

	public:

		/// <summary>
		/// Constructor method used to instantiate a WinUtil object in system memory.
		/// </summary>
		WinUtil() {};

		/// <summary>
		/// Gets the text from the clipboard.
		/// </summary>
		/// <param name="Text">Pointer to string receiving the text.</param>
		/// <returns>Boolean. True if text was available in the clipboard.</returns>
		static bool GetClipboardText(std::string *Text);

		/// <summary>
		/// Sets the text in the clipboard.
		/// </summary>
		/// <param name="Text">String to put into the clipboard.</param>
		/// <returns>Boolean. True if text was added to the clipboard.</returns>
		static bool SetClipboardText(std::string Text);
	};
#endif
#pragma endregion
}
#endif