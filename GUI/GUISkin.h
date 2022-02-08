#ifndef _GUISKIN_
#define _GUISKIN_

#include "GUIConstants.h"

namespace RTE {

	/// <summary>
	/// Skin class used for the controls to get skin details.
	/// </summary>
	class GUISkin {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUISkin object in system memory.
		/// </summary>
		/// <param name="screen">GUIScreen Interface.</param>
		explicit GUISkin(GUIScreen *screen) : m_Screen(screen) {}

		/// <summary>
		/// Loads a skin for a directory
		/// </summary>
		/// <param name="directory">Skin directory.</param>
		/// <param name="fileName">File within to use.</param>
		/// <returns></returns>
		bool Create(const std::string_view &directory, const std::string &fileName = "skin.ini");
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to free a GUISkin object in system memory.
		/// </summary>
		~GUISkin() { Destroy(); }

		/// <summary>
		/// Frees the allocated data.
		/// </summary>
		void Destroy();
#pragma endregion

#pragma region Getters
		/// <summary>
		/// Gets a cached copy of a font. Ownership is NOT transferred!
		/// </summary>
		/// <param name="fontName"></param>
		/// <returns></returns>
		GUIFont * GetFont(const std::string &fontName);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="cursorType"></param>
		/// <returns></returns>
		GUIBitmap * GetMouseCursorBitmap(MouseCursorType cursorType) const { return (cursorType >= MouseCursorType::Pointer && cursorType < MouseCursorType::MouseCursorTypeCount) ? m_MouseCursors.at(cursorType).second : nullptr; }

		/// <summary>
		/// Gets a string value.
		/// </summary>
		/// <param name="sectionName">Section.</param>
		/// <param name="propName">Property.</param>
		/// <param name="propValue">String pointer.</param>
		/// <returns></returns>
		bool GetValue(const std::string &sectionName, const std::string &propName, std::string *propValue);

		/// <summary>
		/// Gets a single integer value.
		/// </summary>
		/// <param name="sectionName">Section.</param>
		/// <param name="propName">Property.</param>
		/// <param name="propValue">Integer pointer.</param>
		/// <returns></returns>
		bool GetValue(const std::string &sectionName, const std::string &propName, int *propValue);

		/// <summary>
		/// Gets an integer array of values.
		/// </summary>
		/// <param name="sectionName">Section.</param>
		/// <param name="propName">Property.</param>
		/// <param name="propValueArray">Integer array.</param>
		/// <param name="arraySize">Max size of array.</param>
		/// <returns></returns>
		int GetValue(const std::string &sectionName, const std::string &propName, int *propValueArray, int arraySize);
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Converts an 8bit palette index to a valid pixel format.
		/// </summary>
		/// <param name="color">Color value in any bit depth. Will be converted to the format specified.</param>
		/// <param name="targetDepth">An optional target color depth that will determine what format the color should be converted to. If this is 0, then the current video color depth will be used as target.</param>
		/// <returns></returns>
		int ConvertColor(int color, int targetDepth = 0) const { return m_Screen->ConvertColor(color, targetDepth); }

		/// <summary>
		/// Creates a blank bitmap.
		/// </summary>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		/// <returns></returns>
		GUIBitmap * CreateBitmap(int width, int height) { return m_Screen->CreateBitmap(width, height); }

		/// <summary>
		/// Creates a bitmap from a filename.
		/// </summary>
		/// <param name="fileName">Filename.</param>
		/// <returns></returns>
		GUIBitmap * CreateBitmap(const std::string &fileName);

		/// <summary>
		/// Builds a bitmap from a standard skin property section.
		/// </summary>
		/// <param name="destBitmap">Destination bitmap.</param>
		/// <param name="sectionName">Section name.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		/// <param name="buildBG">Whether to draw the background and frame.</param>
		/// <param name="buildFrame"></param>
		/// <param name="borderSizes">GUIRect to be filled in with the border sizes of the four sides of the built standard rect.</param>
		void BuildStandardRect(GUIBitmap *destBitmap, const std::string &sectionName, int posX, int posY, int width, int height, bool buildBG = true, bool buildFrame = true, GUIRect *borderSizes = nullptr);
#pragma endregion

	private:

		GUIScreen *m_Screen = nullptr;
		std::string m_SkinDirectory = "";

		std::unordered_map<std::string, GUIProperties *> m_Properties;
		std::unordered_map<std::string, GUIBitmap *> m_BitmapCache;
		std::unordered_map<std::string, GUIFont *> m_FontCache;

		std::unordered_map<MouseCursorType, std::pair<const std::string, GUIBitmap *>> m_MouseCursors = {
			{ MouseCursorType::Pointer, {"Mouse_Pointer", nullptr} },
			{ MouseCursorType::Text, {"Mouse_Text", nullptr} },
			{ MouseCursorType::HorizSize, {"Mouse_HSize", nullptr} }
		};

		// Disallow the use of some implicit methods.
		GUISkin(const GUISkin &reference) = delete;
		GUISkin & operator=(const GUISkin &rhs) = delete;
	};
};
#endif