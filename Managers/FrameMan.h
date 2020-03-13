#ifndef _GUIFRAMEMAN_
#define _GUIFRAMEMAN_

#include "RTEError.h"
#include "Serializable.h"
#include "Timer.h"
#include "Vector.h"
#include "Singleton.h"
#include "ContentFile.h"
#define g_FrameMan FrameMan::Instance()

typedef int MOID;
typedef int MID;

namespace RTE
{

class AllegroScreen;
class GUIFont;

#define MAXSCREENCOUNT 4

enum
{
    g_KeyColor = 0,
	//g_KeyColorS = 16711935,// Key color to clear 32-bit bitmaps used for MOID silhouettes layers
	g_KeyColorS = 0xF81F,// Key color to clear 16-bit bitmaps used for MOID silhouettes layers
	//g_KeyColorS = 0, //Key color to cleat 8-bit bitmaps used for MOID silhouettes layers
    g_BlackColor = 245,
    g_WhiteColor = 254,
    g_RedColor = 13,
    g_YellowGlowColor = 117,
    g_NoMOID = 255
};

enum TransperencyPreset
{
    LessTrans = 0,
    HalfTrans,
    MoreTrans
};

//////////////////////////////////////////////////////////////////////////////////////////
// Class:           FrameMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     The singleton manager over the composition and display of frames.
// Parent(s):       Singleton, Serializable.
// Class history:   12/26/2001 FrameMan created.

class FrameMan:
    public Singleton<FrameMan>,
    public Serializable
{

public:

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     FrameMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a FrameMan object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    FrameMan() { Clear(); }

//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~FrameMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a FrameMan object before deletion
//                  from system memory.
// Arguments:       None.

    virtual ~FrameMan() { Destroy(); }

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the FrameMan object ready for use, which is to be used with
//                  SettingsMan first.
// Arguments:       None.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Create();

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Reset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resets the entire FrameMan, including its inherited members, to
//                  their default settings or values.
// Arguments:       None.
// Return value:    None.

    virtual void Reset() { Clear(); }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the FrameMan object.
// Arguments:       None.
// Return value:    None.

    void Destroy();

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:   GetClassName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the class name of this Entity.
// Arguments:       None.
// Return value:    A string with the friendly-formatted type name of this object.

#undef GetClassName
    virtual const std::string & GetClassName() const { return m_ClassName; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetResX
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the horizontal resolution of the screen.
// Arguments:       None.
// Return value:    An int describing the horizontal resolution of the screen in pixels.

    int GetResX() const { return m_ResX; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetResY
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the vertical resolution of the screen.
// Arguments:       None.
// Return value:    An int describing the vertical resolution of the screen in pixels.

    int GetResY() const { return m_ResY; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetBPP
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the bits per pixel color depth
// Arguments:       None.
// Return value:    An int describing the number of bits per pixel of the current color depth.

    int GetBPP() const { return m_BPP; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPaletteFile
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the ContentFile describing the locaiton of the color palette.
// Arguments:       None.
// Return value:    An reference to a ContentFile which described the palette location.

    const ContentFile & GetPaletteFile() const { return m_PaletteFile; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetBackBuffer8
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the 8bpp back buffer bitmap
// Arguments:       None.
// Return value:    A pointer to the BITMAP 8bpp back buffer. OWNERSHIP IS NOT TRANSFERRED!

    BITMAP * GetBackBuffer8() const { return m_pBackBuffer8; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetBackBuffer32
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the 32bpp back buffer bitmap, if available. If not, the 8bpp is
//                  returned. Make sure you don't do any blending stuff to the 8bpp one!
// Arguments:       None.
// Return value:    A pointer to the BITMAP 32bpp back buffer. OWNERSHIP IS NOT TRANSFERRED!

    BITMAP * GetBackBuffer32() const { return m_pBackBuffer32 ? m_pBackBuffer32 : m_pBackBuffer8; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetBlackIndex
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a font from the GUI engine's current skin. Ownership is NOT
//                  transferred!
// Arguments:       The font filename in the currently used skin, NOT including the skin
//                  path.
// Return value:    A pointer to the requested font, or 0 if no font with that filename
//                  is in the current skin was found. Ownership is NOT transferred!

    unsigned char GetBlackIndex() const { return m_BlackColor; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetLargeFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!
// Arguments:       None.
// Return value:    A pointer to the requested font, or 0 if no large font was found.
//                  Ownership is NOT transferred!

    GUIFont * GetLargeFont();

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSmallFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!
// Arguments:       None.
// Return value:    A pointer to the requested font, or 0 if no large font was found.
//                  Ownership is NOT transferred!

    GUIFont * GetSmallFont();

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the message to be displayed on top of each player's screen
// Arguments:       An std::string that specifies what should be displayed.
//                  Which screen you want to set text to.
//                  The interval with which the screen will be blinking, in ms. 0 means no
//                  blinking.
//                  The duration, in MS to force this message to display. No other message
//                  can be displayed before this expires. ClearScreenText overrides it though.
//                  Vertically centered on the screen
// Return value:    None.

    void SetScreenText(const std::string &msg, int which = 0, int blinkInterval = 0, int displayDuration = -1, bool centered = false);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the message to be displayed on top of each player's screen
// Arguments:       None.
// Return value:    Current message shown to player.

	std::string GetScreenText(int which = 0) const;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the message to be displayed on top of each player's screen
// Arguments:       Which screen you want to set text to.
// Return value:    None.

    void ClearScreenText(int which = 0) { if (which >= 0 && which < MAXSCREENCOUNT) { m_ScreenText[which].clear(); m_TextDuration[which] = -1; m_TextDurationTimer[which].Reset(); m_TextBlinking[which] = 0; } }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads a palette from a .dat file and sets it as the currently used
//                  screen palette.
// Arguments:       String with the data path to the palette data object within a .dat.
// Return value:    Whether palette laoded successfully or not.

    bool LoadPalette(std::string palettePath);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeInPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette in from black at a specified speed.
// Arguments:       Speed specififed from (slowest) 1 - 64 (fastest)
// Return value:    None.

    void FadeInPalette(int fadeSpeed = 1);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeOutPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette out to black at a specified speed.
// Arguments:       Speed specififed from (slowest) 1 - 64 (fastest)
// Return value:    None.

    void FadeOutPalette(int fadeSpeed = 1);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTransTable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific precalculated transperency table which is used for
//                  any subsequent transparency drawing.
// Arguments:       The transperency setting, see the enum.
// Return value:    None.

    void SetTransTable(TransperencyPreset transSetting);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SaveScreenToBMP
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Dumps a bitmap of the screen back buffer to a 8bpp BMP file.
// Arguments:       The filename of the file to save to, WITHOUT EXTENSION. eg, If "Test"
//                  is passed in, this func will save to Test000.bmp, if that file does
//                  not already exist. If it does exist, it will attempt 001, and so on.
// Return value:    Success >=0, or failure < 0.

    int SaveScreenToBMP(const char *namebase);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearBackBuffer8
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the 8bpp backbuffer with black.
// Arguments:       None.
// Return value:    None.

    void ClearBackBuffer8() { clear_to_color(m_pBackBuffer8, m_BlackColor); }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearBackBuffer32
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the 32bpp backbuffer with black.
// Arguments:       None.
// Return value:    None.

    void ClearBackBuffer32() { if (m_pBackBuffer32) clear_to_color(m_pBackBuffer32, 0); }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FlipFrameBuffers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Flips the framebuffer.
// Arguments:       None.
// Return value:    None.

    void FlipFrameBuffers();

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FlashScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Flashes any of the players' screen red for this frame.
// Arguments:       Which screen to flash.
//                  What color to flash it. -1 means no color or flash.
//                  How long a period to fill the frame with color. If 0, a single-frame
//                  flash will happen.
// Return value:    None.

    void FlashScreen(int screen, int color, float periodMS = 0) { m_FlashScreenColor[screen] = color; m_FlashTimer[screen].SetRealTimeLimitMS(periodMS); m_FlashTimer[screen].Reset(); }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a text string to the bitmap of choice, using the internal
//                  fontsets.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The string containing the text to draw.
//                  the position the upper right corner of the first char of the text
//                  string will have on the bitmap.
//                  Whether to draw text with black chars or not (white).
// Return value:    None.
#undef DrawText
    void DrawText(BITMAP *pTargetBitmap,
                  const std::string &str,
                  const Vector &pos,
                  bool black = true);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted or with other effects.
// Arguments:       The Bitmap to draw to. Ownership is NOT transferred.
//                  The absolute Start point.
//                  The absolute end point.
//                  The color value of the line.
//                  A color to alternate with every other pixel drawn will have this if !0.
//                  How many pixels to skip drawing between drawn ones. 0 means solid line
//                  2 means there's a gap of two pixels between each drawn one.
//                  The start of the skipping phase. If skip is 10 and this is 5, the first
//                  dot will will be drawn after 5 pixels.
//                  Whether the line should take the shortest possible route across scene
//                  wraps.
// Return value:    The end state of the skipping phase. Eg if 4 is returned here the last
//                  dot was placed 4 pixels ago.

    int DrawLine(BITMAP *pBitmap, const Vector &start, const Vector &end, int color, int altColor = 0, int skip = 0, int skipStart = 0, bool shortestWrap = false);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawDotLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted with bitmaps.
// Arguments:       The Bitmap to draw to. Ownership is NOT transferred.
//                  The absolute Start point.
//                  The absolute end point.
//                  The bitmap to be used for dots; will be centered.
//                  How many pixels to gap between drawing dots. Should be more than 0
//                  The start of the skipping phase. If skip is 10 and this is 5, the first
//                  dot will will be drawn after 5 pixels.
//                  Whether the line should take the shortest possible route across scene
//                  wraps.
// Return value:    The end state of the skipping phase. Eg if 4 is returned here the last
//                  dot was placed 4 pixels ago.

    int DrawDotLine(BITMAP *pBitmap, const Vector &start, const Vector &end, BITMAP *pDot, int skip = 0, int skipStart = 0, bool shortestWrap = false);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the current frame to the screen.
// Arguments:       None.
// Return value:    None.

    void Draw();

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsValidResolution	
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns true if this resolution is supported
// Arguments:       Resolution width and height
// Return value:    True if resolution is supported
	bool IsValidResolution(int width, int height) const;

protected:

    // Member variables
    static const std::string m_ClassName;

    // Resolution
    int m_ResX;
    int m_ResY;
    // Color depth (bits per pixel)
    int m_BPP;

    // Screen back buffer, always 8bpp, gets copied to the 32bpp buffer if post processing is used
    BITMAP *m_pBackBuffer8;
    // 32Bits per pixel back buffer, only used if player elects, and only if in 32bpp video mode
    BITMAP *m_pBackBuffer32;

    // Temporary buffer for making quick screencaps
    BITMAP *m_pScreendumpBuffer;

    // Data file of the screen palette
    ContentFile m_PaletteFile;

    // Transparency color tables
    COLOR_MAP m_LessTransTable;
    COLOR_MAP m_HalfTransTable;
    COLOR_MAP m_MoreTransTable;

    // Color black index
    unsigned char m_BlackColor;
    unsigned char m_AlmostBlackColor;

    // Dimensions of each of the screens of each player. Will be smaller than resolution only if the screen is split
    int m_PlayerScreenWidth;
    int m_PlayerScreenHeight;

    // GUI screen object kept and owned just for the fonts
    AllegroScreen *m_pGUIScreen;
    // Standard fonts for quick access.
    GUIFont *m_pLargeFont;
    GUIFont *m_pSmallFont;
    // The text to be displayed on each player's screen
    std::string m_ScreenText[MAXSCREENCOUNT];
    // The minimum duration the current message is supposed to show vefore it can be overwritten
    int m_TextDuration[MAXSCREENCOUNT];
    // Screen text display duration time
    Timer m_TextDurationTimer[MAXSCREENCOUNT];
    // Screen text messages blinking interval in ms. 0 is no blink at all, just show message.
    int m_TextBlinking[MAXSCREENCOUNT];
    // Whether screen text is centered vertically
    bool m_TextCentered[MAXSCREENCOUNT];
    // Screen text blink timer
    Timer m_TextBlinkTimer;
    // Whether to flash a player's screen a specific color this frame. -1 means no flash
    int m_FlashScreenColor[MAXSCREENCOUNT];
    // Whether we flashed last frame or not
    bool m_FlashedLastFrame[MAXSCREENCOUNT];
    // Flash screen timer
    Timer m_FlashTimer[MAXSCREENCOUNT];
	// Frame target pos for network players
	Vector m_TargetPos[2][MAXSCREENCOUNT];

private:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this FrameMan, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();

    // Disallow the use of some implicit methods.
    FrameMan(const FrameMan &reference);
    FrameMan & operator=(const FrameMan &rhs);

};
}
#endif