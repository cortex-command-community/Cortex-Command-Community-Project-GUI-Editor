#ifndef _GUIFONT_
#define _GUIFONT_

namespace RTE {

/// <summary>
/// A class to handle the drawing of text.
/// </summary>
class GUIFont {

public:

    // Horizontal Text Alignment,
    enum HAlignment {
        Left = 0,
        Centre,
        Right
    };

    // Vertical Text Alignment,
    enum VAlignment {
        Top = 0,
        Middle,
        Bottom
    };

    // Character structure
    typedef struct {
        int            m_Width;
        int            m_Height;
        int            m_Offset;
    } Character;

    // Font Color structure
    typedef struct {
        int        m_Color;
        GUIBitmap    *m_Bitmap;
    } FontColor;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUIFont object in system memory.
	/// </summary>
	/// <param name="name"></param>
	explicit GUIFont(const std::string &name) : m_Name(name) {}
#pragma endregion


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Load
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads the font from an image file.
// Arguments:       Screen class, Filename of image.

    bool Load(GUIScreen *Screen, const std::string &Filename);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CacheColor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Pre-Calculates the font using a specific color.
// Arguments:       Color.

    void CacheColor(int Color);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetFontColor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Finds a font color structure from the cache.
// Arguments:       Color.

    FontColor * GetFontColor(int Color);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws text to a bitmap.
// Arguments:       Bitmap, Position, Text, Color, Drop-shadow, 0 = none.

    void Draw(GUIBitmap *Bitmap, int X, int Y, const std::string &Text, int Shadow = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawAligned
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws text to a bitmap aligned.
// Arguments:       Bitmap, Position, Text.

    void DrawAligned(GUIBitmap *Bitmap, int X, int Y, const std::string &Text, int HAlign, int VAlign = Top, int maxWidth = 0, int Shadow = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetColor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the current color.
// Arguments:       Color.

    void SetColor(int Color);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateWidth
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculates the width of a piece of text.
// Arguments:       Text.

    int CalculateWidth(const std::string &Text);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateWidth
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculates the width of a piece of text.
// Arguments:       Character.

    int CalculateWidth(const char Character);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculates the height of a piece of text, if it's wrapped within a
//                  max width.
// Arguments:       Text, and the max width. If 0, no wrapping is done.

    int CalculateHeight(const std::string &Text, int MaxWidth = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetFontHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the font height.
// Arguments:       None.

	int GetFontHeight() const { return m_FontHeight; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the name of the font
// Arguments:       None.

	std::string GetName() const { return m_Name; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys the font data
// Arguments:       None.

    void Destroy();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetKerning
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Get the character kerning (spacing)
// Arguments:       None.

	int GetKerning() const { return m_Kerning; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetKerning
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Set the character kerning (spacing), in pixels. 1 = one empty pixel
//                  between chars, 0 = chars are touching.
// Arguments:       None.

    void SetKerning(int newKerning = 1) { m_Kerning = newKerning; }

private:

	GUIBitmap *m_Font = nullptr;
	GUIScreen *m_Screen = nullptr;
    std::vector<FontColor > m_ColorCache;

	int m_FontHeight = 0;
	int m_MainColor = 15;
	int m_CurrentColor = m_MainColor;
	GUIBitmap *m_CurrentBitmap = nullptr;
	std::string m_Name = "";
    std::array<Character, 256> m_Characters;

    int m_CharIndexCap = 256; // The highest index of valid characters that was read in from the file

    int m_Kerning = 0; // Spacing between characters
    int m_Leading = 0; // Spacing between lines
};
};
#endif