#ifndef _GUIBUTTON_
#define _GUIBUTTON_

namespace RTE {

    class GUILabel;

/// <summary>
/// A button control class.
/// </summary>
class GUIButton : public GUIControl {

public:

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUIButton object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUIButton(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

	/// <summary>
	/// Called when the control has been created.
	/// </summary>
	/// <param name="name">Name.</param>
	/// <param name="posX">Position.</param>
	/// <param name="posY"></param>
	/// <param name="width">Size.</param>
	/// <param name="height"></param>
	void Create(const std::string_view &name, int posX, int posY, int width = -1, int height = -1) override;
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUIButton() override = default;
#pragma endregion


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
// Method:          OnMouseMove
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse moves (over the panel, or when captured).
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseMove(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseEnter
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse enters the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseEnter(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseLeave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse leaves the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseLeave(int X, int Y, int Buttons, int Modifier) override;

 //////////////////////////////////////////////////////////////////////////////////////////
// Method:  OnGainFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel gains focus.
// Arguments:       None.

    void OnGainFocus() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  OnLoseFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel looses focus.
// Arguments:       None.

    void OnLoseFocus() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnKeyDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key goes down.
// Arguments:       KeyCode, Modifier.

	void OnKeyDown(int KeyCode, int Modifier) override {}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

    void StoreProperties() override;

    /// <summary>
    /// Gets whether or not this button is currently pushed.
    /// </summary>
    /// <returns>Whether or not this button is currently pushed.</returns>
    bool IsPushed() const { return m_Pushed; }

    /// <summary>
    /// Sets whether or not this button is currently pushed.
    /// </summary>
    /// <param name="pushed">Whether or not this button should be pushed.</param>
    void SetPushed(bool pushed = false);

    /// <summary>
    /// Gets whether or not this button is currently being moused over.
    /// </summary>
    /// <returns>Whether or not this button is currently being moused over.</returns>
    bool IsMousedOver() const { return m_Over; }

    /// <summary>
    /// Gets the text of this GUIButton's GUILabel.
    /// </summary>
    /// <returns>The text of this GUIButton's GUILabel.</returns>
    const std::string & GetText() const;

    /// <summary>
    /// Sets the text of this GUIButton's GUILabel.
    /// </summary>
    /// <param name="newText">The new text for this GUIButton's GUILabel.</param>
    /// <param name="noBitmapRebuild">Lets this method NOT rebuild the button bitmap, even if the icon has changed. Defaults to false and should almost always stay that way.</param>
    void SetText(const std::string_view &newText, bool noBitmapRebuild = false);

    /// <summary>
    /// Sets whether or not this GUIButton's text should scroll horizontally (right) when it overflows the button.
    /// </summary>
    /// <param name="newOverflowScroll">Whether or not this GUIButton's text should scroll horizontally when it overflows.</param>
    void SetHorizontalOverflowScroll(bool newOverflowScroll);

    /// <summary>
    /// Sets whether or not this GUIButton's text should scroll vertically (down) when it overflows the button.
    /// </summary>
    /// <param name="newOverflowScroll">Whether or not this GUIButton's text should scroll vertically when it overflows.</param>
    void SetVerticalOverflowScroll(bool newOverflowScroll);

    /// <summary>
    /// Gets whether or not this GUIButton has an icon with a Bitmap.
    /// </summary>
    bool HasIcon() const { return m_Icon->GetBitmap(); }

    /// <summary>
    /// Sets the icon for this GUIButton. Ownership is NOT transferred.
    /// </summary>
    /// <param name="newIcon">A pointer to the new icon BITMAP for this GUIButton.</param>
    /// <param name="noBitmapRebuild">Lets this method NOT rebuild the button bitmap, even if the icon has changed. Defaults to false and should almost always stay that way.</param>
    void SetIcon(BITMAP *newIcon, bool noBitmapRebuild = false);

    /// <summary>
    /// Helper method to set both text and icon for this GUIButton at the same time.
    /// </summary>
    /// <param name="newIcon">A pointer to the new icon BITMAP for this GUIButton.</param>
    /// <param name="newText">The new text for this GUIButton's GUILabel.</param>
    void SetIconAndText(BITMAP *newIcon, const std::string_view &newText);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 10;
	static constexpr int m_MinHeight = 10;
	static constexpr int m_DefaultWidth = 60;
	static constexpr int m_DefaultHeight = 40;

	bool m_Pushed = false;
	bool m_Over = false;
	std::unique_ptr<GUILabel> m_Text = nullptr;
	std::unique_ptr<GUIBitmap> m_Icon = nullptr;
	std::unique_ptr<GUIRect> m_BorderSizes = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the button bitmap to draw.
// Arguments:       None.

    void BuildBitmap();
};
};
#endif