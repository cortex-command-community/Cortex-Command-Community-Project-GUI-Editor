//////////////////////////////////////////////////////////////////////////////////////////
// File:            FrameMan.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the FrameMan class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "Timer.h"
#include "FrameMan.h"
#include "UInputMan.h"
//#include "ConsoleMan.h"
//#include "SettingsMan.h"

#include "GUI.h"
#include "AllegroBitmap.h"
#include "AllegroScreen.h"


// I know this is a crime, but if I include it in FrameMan.h the whole thing will collapse due to int redefinitions in Allegro
std::mutex ScreenRelativeEffectsMutex[MAXSCREENCOUNT];

namespace RTE
{

const string FrameMan::m_ClassName = "FrameMan";

//////////////////////////////////////////////////////////////////////////////////////////
// Callback function for the allegro set_display_switch_callback. It will be called when
// focus is swtiched away to the game window. 

void DisplaySwitchOut(void)
{
    g_UInputMan.DisableMouseMoving(true);
//    g_ActivityMan.PauseActivity();
//    g_InActivity = false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Callback function for the allegro set_display_switch_callback. It will be called when
// focus is swtiched back to the game window. It will temporarily disable positioniong of
// the mouse. This is so that when focus is swtiched back to the game window, it avoids
// having the window fly away because the user clicked the title bar of the window.

void DisplaySwitchIn(void)
{
    g_UInputMan.DisableMouseMoving(false);
//    g_UInputMan.ReInitKeyboard();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this FrameMan, effectively
//                  resetting the members of this abstraction level only.

void FrameMan::Clear()
{
    m_ResX = 640;
    m_ResY = 480;
    m_BPP = 8;
    m_pBackBuffer8 = 0;
	m_pBackBuffer32 = 0;
    m_pScreendumpBuffer = 0;
    m_PaletteFile.Reset();
    m_BlackColor = 245;
    m_AlmostBlackColor = 245;
    m_PlayerScreenWidth = 0;
    m_PlayerScreenHeight = 0;
    m_pGUIScreen = 0;
    m_pLargeFont = 0;
    m_pSmallFont = 0;

    for (int i = 0; i < MAXSCREENCOUNT; ++i)
    {
        m_ScreenText[i].clear();
        m_TextDuration[i] = -1;
        m_TextDurationTimer[i].Reset();
        m_TextBlinking[i] = 0;
        m_TextCentered[i] = false;
        m_FlashScreenColor[i] = -1;
        m_FlashedLastFrame[i] = false;
        m_FlashTimer[i].Reset();

    }
    m_TextBlinkTimer.Reset();
}



//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the FrameMan object ready for use, which is to be used with
//                  SettingsMan first.

int FrameMan::Create()
{
    // Init allegro's graphics
    set_color_depth(m_BPP);

//    GFX_MODE_LIST *pList = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
//    destroy_gfx_mode_list(pList);

	int fullscreenGfxDriver = GFX_AUTODETECT_FULLSCREEN;
	int windowedGfxDriver = GFX_AUTODETECT_WINDOWED;


    fullscreenGfxDriver = GFX_DIRECTX_ACCEL;
	/*
	if (g_SettingsMan.ForceSoftwareGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SOFT;
	if (g_SettingsMan.ForceSafeGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SAFE;
	if (g_SettingsMan.ForceOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_OVL;
	if (g_SettingsMan.ForceNonOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN;
	if (g_SettingsMan.ForceVirtualFullScreenGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN_BORDERLESS;
	*/



	if (set_gfx_mode(windowedGfxDriver, m_ResX, m_ResY, 0, 0) != 0) 
	{
		//g_ConsoleMan.PrintString("Failed to set gfx mode, trying different windowed scaling.");
				// Oops, failed to set the resolution specified in the setting file, so default to a safe one instead
		allegro_message("Unable to set specified graphics mode because: %s!\n\nNow trying to default back to VGA...", allegro_error);
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0) 
		{
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to set any graphics mode because %s!", allegro_error);
			return 1;
		}
		// Successfully reverted to VGA, so set that as the current resolution
		m_ResX = 640;
		m_ResY = 480;
	}
		
   
    // Clear the screen buffer so it doesn't flash pink
    if (m_BPP == 8)
        clear_to_color(screen, m_BlackColor);
    else
        clear_to_color(screen, 0);

    // Sets the allowed color conversions when loading bitmaps from files
    set_color_conversion(COLORCONV_MOST);

    // Load and set the palette
    if (!LoadPalette(m_PaletteFile.GetDataPath()))
        return -1;

    // Set the switching mode; what happens when the app window is switched to and fro
    set_display_switch_mode(SWITCH_BACKGROUND);
//    set_display_switch_mode(SWITCH_PAUSE);
    set_display_switch_callback(SWITCH_OUT, DisplaySwitchOut);
    set_display_switch_callback(SWITCH_IN, DisplaySwitchIn);

    // Create transparency color table
    PALETTE ccpal;
    get_palette(ccpal);
    create_trans_table(&m_LessTransTable, ccpal, 192, 192, 192, 0);
    create_trans_table(&m_HalfTransTable, ccpal, 128, 128, 128, 0);
    create_trans_table(&m_MoreTransTable, ccpal, 64, 64, 64, 0);
    // Set the one Allegro currently uses
    color_map = &m_HalfTransTable;

    // Create the back buffer, this is still in 8bpp, we will do any postprocessing on the PostProcessing bitmap
    m_pBackBuffer8 = create_bitmap_ex(8, m_ResX, m_ResY);
    clear_to_color(m_pBackBuffer8, m_BlackColor);



    // Create the post processing buffer if in 32bpp video mode, it'll be used for glow effects etc
    if (get_color_depth() == 32 && m_BPP == 32)
    {
        // 32bpp so we can make ze cool effects. Everyhting up to this is 8bpp, including the back buffer
        m_pBackBuffer32 = create_bitmap_ex(32, m_ResX, m_ResY);
//        clear_to_color(m_pBackBuffer32, m_BlackColor);
	}

    m_PlayerScreenWidth = m_pBackBuffer8->w;
    m_PlayerScreenHeight = m_pBackBuffer8->h;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsValidResolution	
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns true if this resolution is supported
bool FrameMan::IsValidResolution(int width, int height) const
{
	int actualWidth = width;
	int actualHeight = height;

	// If width is greater than 1280, the game will switch itself in 2X mode
	// lowering actual resolution twice
	if (width >= 1280)
	{
		actualWidth = width / 2;
		actualHeight = height / 2;
	}

	if (actualWidth < 360 || actualHeight < 360)
		return false;
	else
		return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the FrameMan object.

void FrameMan::Destroy()
{
    destroy_bitmap(m_pBackBuffer8);
    destroy_bitmap(m_pBackBuffer32);
    delete m_pGUIScreen;
    delete m_pLargeFont;
    delete m_pSmallFont;

    g_TimerMan.Destroy();

    Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetLargeFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!

GUIFont * FrameMan::GetLargeFont()
{
    if (!m_pLargeFont)
    {
        if (!m_pGUIScreen)
        {
            m_pGUIScreen = new AllegroScreen(m_pBackBuffer8);
        }
        m_pLargeFont = new GUIFont("FatFont");
        m_pLargeFont->Load(m_pGUIScreen, "Base.rte/GUIs/Skins/Base/fatfont.bmp");
    }
    return m_pLargeFont;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSmallFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!

GUIFont * FrameMan::GetSmallFont()
{
    if (!m_pSmallFont)
    {
        if (!m_pGUIScreen)
        {
            m_pGUIScreen = new AllegroScreen(m_pBackBuffer8);
        }
        m_pSmallFont = new GUIFont("SmallFont");
        m_pSmallFont->Load(m_pGUIScreen, "Base.rte/GUIs/Skins/Base/smallfont.bmp");
    }
    return m_pSmallFont;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the message to be displayed on top of each player's screen

void FrameMan::SetScreenText(const std::string &msg, int which, int blinkInterval, int displayDuration, bool centered)
{
    if (which >= 0 && which < MAXSCREENCOUNT)
    {
        // See if we can overwrite the previous message
        if (m_TextDurationTimer[which].IsPastRealMS(m_TextDuration[which]))
        {
            m_ScreenText[which] = msg;
            m_TextDuration[which] = displayDuration;
            m_TextDurationTimer[which].Reset();
            m_TextBlinking[which] = blinkInterval;
            m_TextCentered[which] = centered;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the message to be displayed on top of each player's screen

std::string FrameMan::GetScreenText(int which) const
{
    if (which >= 0 && which < MAXSCREENCOUNT)
		return m_ScreenText[which];
	else
		return "";
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads a palette from a .dat file and sets it as the currently used
//                  screen palette.

bool FrameMan::LoadPalette(std::string palettePath)
{
    // Look for the '#' denoting the divider between the datafile and the datafile object's name is
    // If we find one, that means we're trying to load from a datafile, otehrwise it's from an exposed bitmap
    int separatorPos = palettePath.rfind('#');

    // Check whether we're trying to load a palette from an exposed bitmap or from a datafile
    if (separatorPos == -1 || separatorPos >= palettePath.length() - 1)
    {
        // Just going to discard the bitmap, we're only interested in the palette
        BITMAP *tempBitmap;
        PALETTE newPalette;
        if (!(tempBitmap = load_bitmap(palettePath.c_str(), newPalette)))
            RTEAbort(("Failed to load palette from bitmap with following path:\n\n" + palettePath).c_str());

        // Set the current palette
        set_palette(newPalette);

        // Update what black is now with the loaded palette
        m_BlackColor = bestfit_color(newPalette, 0, 0, 0);
        m_AlmostBlackColor = bestfit_color(newPalette, 5, 5, 5);

        // Free the temp bitmap that had the palette
        destroy_bitmap(tempBitmap);
    }

    // Indicate success
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeInPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette in from black at a specified speed.

void FrameMan::FadeInPalette(int fadeSpeed)
{
    if (fadeSpeed < 1)
        fadeSpeed = 1;
    if (fadeSpeed > 64)
        fadeSpeed = 64;

    PALETTE pal;
    get_palette(pal);
    fade_in(pal, fadeSpeed);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeOutPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette out to black at a specified speed.

void FrameMan::FadeOutPalette(int fadeSpeed)
{
    if (fadeSpeed < 1)
        fadeSpeed = 1;
    if (fadeSpeed > 64)
        fadeSpeed = 64;

    fade_out(fadeSpeed);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTransTable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific precalculated transperency table which is used for
//                  any subsequent transparency drawing.

void FrameMan::SetTransTable(TransperencyPreset transSetting)
{
    if (transSetting == LessTrans)
        color_map = &m_LessTransTable;
    else if (transSetting == MoreTrans)
        color_map = &m_MoreTransTable;
    else
        color_map = &m_HalfTransTable;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SaveScreenToBMP
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Dumps a bitmap to a 8bpp BMP file.

int FrameMan::SaveScreenToBMP(const char *namebase)
{
    int filenumber = 0;
    char fullfilename[256];
    int maxFileTrys = 1000;

    // Make sure its not a 0 namebase
    if (namebase == 0 || strlen(namebase) <= 0)
        return -1;

    do {
        // Check for the file namebase001.bmp; if it exists, try 002, etc.
        sprintf_s(fullfilename, sizeof(fullfilename), "%s%03i.bmp", namebase, filenumber++);
        if (!exists(fullfilename)) {
            break;
        }
    } while (filenumber < maxFileTrys);

    // Save out the screen bitmap, after making a copy of it, faster sometimes
    if (screen) {
        if (!m_pScreendumpBuffer)
            m_pScreendumpBuffer = create_bitmap(screen->w, screen->h);
        blit(screen, m_pScreendumpBuffer, 0, 0, 0, 0, screen->w, screen->h);
        PALETTE palette;
        get_palette(palette);
        save_bmp(fullfilename, m_pScreendumpBuffer, palette);

        //g_ConsoleMan.PrintString("SYSTEM: Screen was dumped to: " + string(fullfilename));
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FlipFrameBuffers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Flips the framebuffer.

void FrameMan::FlipFrameBuffers()
{
    if (get_color_depth() == 32 && m_BPP == 32 && m_pBackBuffer32)
    {
		blit(m_pBackBuffer32, screen, 0, 0, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h);
    }
    // 8bpp video mode
    else
    {
		blit(m_pBackBuffer8, screen, 0, 0, 0, 0, m_pBackBuffer8->w, m_pBackBuffer8->h);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a text string to the bitmap of choice, using the internal
//                  fontsets.

void FrameMan::DrawText(BITMAP *pTargetBitmap,
                        const string &str,
                        const Vector &pos,
                        bool black)
{
    RTEAbort("Old fonts are not ported yet!");
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted or with other effects.

int FrameMan::DrawLine(BITMAP *pBitmap, const Vector &start, const Vector &end, int color, int altColor, int skip, int skipStart, bool shortestWrap)
{
	/*
    RTEAssert(pBitmap, "Trying to draw line to null Bitmap");

//    acquire_bitmap(pBitmap);

    int hitCount = 0, error, dom, sub, domSteps, skipped = skip + (skipStart - skip);
    int intPos[2], delta[2], delta2[2], increment[2];
    bool drawAlt = false;

    // Just make the alt the same color as the main one if no one was specified
    if (altColor == 0)
        altColor = color;

    // Calculate the integer values
    intPos[X] = floorf(start.m_X);
    intPos[Y] = floorf(start.m_Y);

    // Wrap line around the scene if it makes it shorter
    if (shortestWrap)
    {
        Vector deltaVec = g_SceneMan.ShortestDistance(start, end, false);
        delta[X] = floorf(deltaVec.m_X);
        delta[Y] = floorf(deltaVec.m_Y);
    }
    // No wrap
    else
    {
        delta[X] = floorf(end.m_X) - intPos[X];
        delta[Y] = floorf(end.m_Y) - intPos[Y];
    }

    if (delta[X] == 0 &&  delta[Y] == 0)
        return 0;

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm preparation

    if (delta[X] < 0)
    {
        increment[X] = -1;
        delta[X] = -delta[X];
    }
    else
        increment[X] = 1;

    if (delta[Y] < 0)
    {
        increment[Y] = -1;
        delta[Y] = -delta[Y];
    }
    else
        increment[Y] = 1;

    // Scale by 2, for better accuracy of the error at the first pixel
    delta2[X] = delta[X] << 1;
    delta2[Y] = delta[Y] << 1;

    // If X is dominant, Y is submissive, and vice versa.
    if (delta[X] > delta[Y]) {
        dom = X;
        sub = Y;
    }
    else {
        dom = Y;
        sub = X;
    }

    error = delta2[sub] - delta[dom];

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm execution

    Vector glowPoint;
    for (domSteps = 0; domSteps < delta[dom]; ++domSteps)
    {
        intPos[dom] += increment[dom];
        if (error >= 0)
        {
            intPos[sub] += increment[sub];
            error -= delta2[dom];
        }
        error += delta2[sub];

        // Only draw pixel if we're not due to skip any
        if (++skipped > skip)
        {
            // Scene wrapping, if necessary
            g_SceneMan.WrapPosition(intPos[X], intPos[Y]);

            // Slap a regular pixel on there
            putpixel(pBitmap, intPos[X], intPos[Y], drawAlt ? color : altColor);

            drawAlt = !drawAlt;
            skipped = 0;
        }
    }

//    release_bitmap(pBitmap);
    // Return the end phase state of the skipping
    return skipped;*/
return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawDotLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted with bitmaps.

int FrameMan::DrawDotLine(BITMAP *pBitmap, const Vector &start, const Vector &end, BITMAP *pDot, int skip, int skipStart, bool shortestWrap)
{
	/*
    RTEAssert(pBitmap, "Trying to draw line to null Bitmap");
    RTEAssert(pDot, "Trying to draw line of dots without specifying a dot Bitmap");

//    acquire_bitmap(pBitmap);

    int hitCount = 0, error, dom, sub, domSteps, skipped = skip + (skipStart - skip);
    int intPos[2], delta[2], delta2[2], increment[2];
    bool drawAlt = false;
    int dotHalfHeight = pDot->h / 2;
    int dotHalfWidth = pDot->w / 2;

    // Calculate the integer values
    intPos[X] = floorf(start.m_X);
    intPos[Y] = floorf(start.m_Y);

    // Wrap line around the scene if it makes it shorter
    if (shortestWrap)
    {
        Vector deltaVec = g_SceneMan.ShortestDistance(start, end, false);
        delta[X] = floorf(deltaVec.m_X);
        delta[Y] = floorf(deltaVec.m_Y);
    }
    // No wrap
    else
    {
        delta[X] = floorf(end.m_X) - intPos[X];
        delta[Y] = floorf(end.m_Y) - intPos[Y];
    }

    if (delta[X] == 0 &&  delta[Y] == 0)
        return 0;

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm preparation

    if (delta[X] < 0)
    {
        increment[X] = -1;
        delta[X] = -delta[X];
    }
    else
        increment[X] = 1;

    if (delta[Y] < 0)
    {
        increment[Y] = -1;
        delta[Y] = -delta[Y];
    }
    else
        increment[Y] = 1;

    // Scale by 2, for better accuracy of the error at the first pixel
    delta2[X] = delta[X] << 1;
    delta2[Y] = delta[Y] << 1;

    // If X is dominant, Y is submissive, and vice versa.
    if (delta[X] > delta[Y]) {
        dom = X;
        sub = Y;
    }
    else {
        dom = Y;
        sub = X;
    }

    error = delta2[sub] - delta[dom];

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm execution

    Vector glowPoint;
    for (domSteps = 0; domSteps < delta[dom]; ++domSteps)
    {
        intPos[dom] += increment[dom];
        if (error >= 0)
        {
            intPos[sub] += increment[sub];
            error -= delta2[dom];
        }
        error += delta2[sub];

        // Only draw pixel if we're not due to skip any
        if (++skipped > skip)
        {
            // Scene wrapping, if necessary
            g_SceneMan.WrapPosition(intPos[X], intPos[Y]);

            // Slap the dot on there
            masked_blit(pDot, pBitmap, 0, 0, intPos[X] - dotHalfWidth, intPos[Y] - dotHalfHeight, pDot->w, pDot->h);

            drawAlt = !drawAlt;
            skipped = 0;
        }
    }

//    release_bitmap(pBitmap);
    // Return the end phase state of the skipping
    return skipped;
	*/
return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the current frame to the screen.

void FrameMan::Draw()
{
    // Count how many split screens we'll need
	int screenCount = 1;

    RTEAssert(screenCount <= 1, "Splitscreen surface not ready when needed!");
    // Choose which buffer to draw to. If there are no splitscreens and 8bit modes, draw directly to the back buffer, else use a intermediary splitscreen buffer
    char str[512];

    for (int whichScreen = 0; whichScreen < screenCount; ++whichScreen)
    {

		BITMAP *pDrawScreen = /*get_color_depth() == 8 && */screenCount == 1 ? m_pBackBuffer8 : m_pPlayerScreen;
		BITMAP *pDrawScreenGUI = pDrawScreen;


		AllegroBitmap pPlayerGUIBitmap(pDrawScreenGUI);

        Vector targetPos = g_SceneMan.GetOffset(whichScreen);
        // Adjust the drawing position on the target screen for if the target screen is larger than the scene in nonwrapping dimension.
        // Scene needs to be displayed centered on the target bitmap then, and that has to be adjusted for when drawing to the screen
        if (!g_SceneMan.SceneWrapsX() && pDrawScreen->w > g_SceneMan.GetSceneWidth())
            targetPos.m_X += (pDrawScreen->w - g_SceneMan.GetSceneWidth()) / 2;
        if (!g_SceneMan.SceneWrapsY() && pDrawScreen->h > g_SceneMan.GetSceneHeight())
            targetPos.m_Y += (pDrawScreen->h - g_SceneMan.GetSceneHeight()) / 2;

        // Draw the scene
			clear_to_color(pDrawScreen, g_KeyColor);
			clear_to_color(pDrawScreenGUI, g_KeyColor);
			g_SceneMan.Draw(pDrawScreen, pDrawScreenGUI, targetPos, true, true);
		
// TODO: Find out what keeps disabling the clipping on the draw bitmap
        // Enable clipping on the draw bitmap
        set_clip_state(pDrawScreen, 1);


        ///////////////////////////////////////
        // Draw screen texts
        int yTextPos = 0;

        // Only draw this stuff for actual human players
        if (whichScreen < g_ActivityMan.GetActivity()->GetHumanCount())
        {
            // Team of current screen's player
//            int team = g_ActivityMan.GetActivity()->GetTeamOfPlayer(g_ActivityMan.GetActivity()->PlayerOfScreen(whichScreen));

            yTextPos += 12;

            // Message
            if (!m_ScreenText[whichScreen].empty())
            {
				if (m_TextCentered[whichScreen])
					yTextPos = (GetPlayerScreenHeight() / 2) - 52;
				int occOffsetX = g_SceneMan.GetScreenOcclusion(whichScreen).m_X;

				// If there's really no room to offset the text into, then don't
				if (GetPlayerScreenWidth() <= GetResX() / 2)
					occOffsetX = 0;
				// Draw blinking effect, but not of the text message itself, but some characters around it (so it's easier to read the msg)
				if (m_TextBlinking[whichScreen] && m_TextBlinkTimer.AlternateReal(m_TextBlinking[whichScreen]))
					GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, (GetPlayerScreenWidth() + occOffsetX) / 2, yTextPos, (">>> " + m_ScreenText[whichScreen] + " <<<").c_str(), GUIFont::Centre);
				else
					GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, (GetPlayerScreenWidth() + occOffsetX) / 2, yTextPos, m_ScreenText[whichScreen].c_str(), GUIFont::Centre);

                yTextPos += 12;
            }

        ////////////////////////////////////////
        // If we are dealing with split screens, then deal with the fact that we need to draw the player screens to different locations on the final buffer

        // The position of the current drawscreen on the final screen
        Vector screenOffset;

        if (screenCount > 1)
        {
            // Always upper left corner
            if (whichScreen == 0)
                screenOffset.SetIntXY(0, 0);
            else if (whichScreen == 1)
            {
                // If both splits, or just Vsplit, then in upper right quadrant
                if ((m_VSplit && !m_HSplit) || (m_VSplit && m_HSplit))
                    screenOffset.SetIntXY(GetResX() / 2, 0);
                // If only hsplit, then lower left quadrant
                else
                    screenOffset.SetIntXY(0, GetResY() / 2);
            }
            // Always lower left quadrant
            else if (whichScreen == 2)
                screenOffset.SetIntXY(0, GetResY() / 2);
            // Always lower right quadrant
            else if (whichScreen == 3)
                screenOffset.SetIntXY(GetResX() / 2, GetResY() / 2);
        }

        // Flash the screen if we're supposed to
        if (m_FlashScreenColor[whichScreen] != -1)
        {
            // If set to flash for a period of time, first be solid and then start flashing slower
            float left = m_FlashTimer[whichScreen].LeftTillRealTimeLimitMS();
            if (/*left > 500 || */left < 10 || m_FlashTimer[whichScreen].AlternateReal(50))
            {
                // At most, only flash every other frame
                if (m_FlashedLastFrame[whichScreen])
                    m_FlashedLastFrame[whichScreen] = false;
                else
                {
                    // FLASH!
                    rectfill(pDrawScreenGUI, 0, 0, pDrawScreenGUI->w, pDrawScreenGUI->h, m_FlashScreenColor[whichScreen]);
                    // Show that we did indeed flash this frame
                    m_FlashedLastFrame[whichScreen] = true;
                }
            }
            // Make things go into slight slomo - DANGER
//            g_TimerMan.SetOneSimUpdatePerFrame(!m_SloMoTimer.IsPastSimTimeLimit());
//            g_TimerMan.IsOneSimUpdatePerFrame();
            // Stop with the flash after the designated period
            if (m_FlashTimer[whichScreen].IsPastRealTimeLimit())
                m_FlashScreenColor[whichScreen] = -1;
        }
    }
    release_bitmap(m_pBackBuffer8);
}
}