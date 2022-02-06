#ifndef _GUILABEL_
#define _GUILABEL_

#ifdef GUI_STANDALONE
#include "Timer.h"
#endif

namespace RTE {

/// <summary>
/// A label control class.
/// </summary>
class GUILabel : public GUIControl {

public:

    enum class OverflowScrollState {
        Deactivated = 0,
        WaitAtStart,
        Scrolling,
        WaitAtEnd
    };

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUILabel object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUILabel(GUIControlManager *owningManager) { m_OwningManager = owningManager; }
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUILabel() override = default;
#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been created.
// Arguments:       Name, Position.

    void Create(const std::string &Name, int X, int Y, int Width = -1, int Height = -1) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been created.
// Arguments:       Properties.

    void Create(GUIProperties *Props) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ChangeSkin
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the skin has been changed.
// Arguments:       New skin pointer.

    void ChangeSkin(GUISkin *Skin) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the panel
// Arguments:       Screen class

    void Draw(GUIScreen *Screen) override;

    /// <summary>
    /// Draws the Label to the given GUIBitmap.
    /// </summary>
    /// <param name="Bitmap">The GUIBitmap to draw the label to.</param>
    /// <param name="overwiteFontColorAndKerning">Whether to overwrite the font's color and kerning with the stored values. Defaults to true, which is usually what you want.</param>
    void Draw(GUIBitmap *Bitmap, bool overwiteFontColorAndKerning = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  OnMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes down on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseDown(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  OnMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes up on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseUp(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Move
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be moved.
// Arguments:       New position.

	void Move(int X, int Y) override { GUIControl::SetPositionAbs(X, Y); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be resized.
// Arguments:       New size.

    void Resize(int Width, int Height) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ResizeHeightToFit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resize the height of the label to fit the amount of text it has to
//                  display.
// Arguments:       None.
// Returns:         The new height in pixels.

    int ResizeHeightToFit();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the text of the label.
// Arguments:       text.

    void SetText(const std::string_view &text) { m_Text = text; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the text of the label.
// Arguments:       None.

    const std::string & GetText() const { return m_Text; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTextHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows how tall the current text is with the current width and font etc.
// Arguments:       None.
// Returns:         The text height, in pixels

    int GetTextHeight();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetHAlignment
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the horizontal alignment of the text of this label.
// Arguments:       The desired alignment.

    void SetHAlignment(int HAlignment = GUIFont::HAlignment::Left) { m_HAlignment = HAlignment; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetVAlignment
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the vertical alignment of the text of this label.
// Arguments:       The desired alignment.

    void SetVAlignment(int VAlignment = GUIFont::VAlignment::Top) { m_VAlignment = VAlignment; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetHAlignment
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the horizontal alignment of the text of this label.
// Arguments:       None.

    int GetHAlignment() const { return m_HAlignment; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetVAlignment
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the vertical alignment of the text of this label.
// Arguments:       The desired alignment.

    int GetVAlignment() const { return m_VAlignment; }

    /// <summary>
    /// Gets whether or not this GUILabel should scroll horizontally (right) when it overflows.
    /// </summary>
    /// <returns>Whether or not this GUILabel should scroll horizontally when it overflows.</returns>
    bool GetHorizontalOverflowScroll() const { return m_HorizontalOverflowScroll; }

    /// <summary>
    /// Sets whether or not this GUILabel should scroll horizontally (right) when it overflows. Mutually exclusive with horizontal overflow scrolling.
    /// </summary>
    /// <param name="newOverflowScroll">Whether or not this GUILabel should scroll horizontally when it overflows.</param>
    void SetHorizontalOverflowScroll(bool newOverflowScroll);

    /// <summary>
    /// Gets whether or not this GUILabel should scroll vertically (down) when it overflows.
    /// </summary>
    /// <returns>Whether or not this GUILabel should scroll vertically when it overflows.</returns>
    bool GetVerticalOverflowScroll() const { return m_VerticalOverflowScroll; }

    /// <summary>
    /// Sets whether or not this GUILabel should scroll vertically (down) when it overflows. Mutually exclusive with horizontal overflow scrolling.
    /// </summary>
    /// <param name="newOverflowScroll">Whether or not this GUILabel should scroll vertically when it overflows.</param>
    void SetVerticalOverflowScroll(bool newOverflowScroll);

    /// <summary>
    /// Gets whether or not horizontal or vertical overflow scrolling is turned on.
    /// </summary>
    /// <returns>Whether or not horizontal or vertical overflow scrolling is turned on.</returns>
    bool OverflowScrollIsEnabled() const { return m_HorizontalOverflowScroll || m_VerticalOverflowScroll; }

    /// <summary>
    /// Gets whether or not horizontal/vertical scrolling is happening.
    /// </summary>
    /// <returns>Whether or not horizontal/vertical scrolling is happening.</returns>
    bool OverflowScrollIsActivated() const { return OverflowScrollIsEnabled() && m_OverflowScrollState != OverflowScrollState::Deactivated; }

    /// <summary>
    /// Sets whether or not horizontal/vertical scrolling should be happening. When it's deactivated, text will instantly go back to un-scrolled.
    /// </summary>
    /// <param name="activateScroll">Whether the overflow scrolling should activate (true) or deactivate (false).</param>
    void ActivateDeactivateOverflowScroll(bool activateScroll);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

    void StoreProperties() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;

private:

	static const std::string_view c_ControlType;

    std::string m_Text = "";
    int m_HAlignment = GUIFont::HAlignment::Left;
    int m_VAlignment = GUIFont::VAlignment::Middle;
    bool m_HorizontalOverflowScroll = false; //!< Note that horizontal overflow scrolling means text will always be on one line.
    bool m_VerticalOverflowScroll = false;
    OverflowScrollState m_OverflowScrollState = OverflowScrollState::Deactivated;

	Timer m_OverflowScrollTimer = Timer();
};
};
#endif