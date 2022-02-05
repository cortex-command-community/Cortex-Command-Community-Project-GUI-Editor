#ifndef _GUITEXTPANEL_
#define _GUITEXTPANEL_

namespace RTE {

/// <summary>
/// A text panel class.
/// </summary>
class GUITextPanel : public GUIControl {

public:

    // Text panel signals
    enum Signals {
        Clicked = 0,
        MouseDown,
        Changed,
        Enter
    };


#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUITextPanel object in system memory.
	/// </summary>
	GUITextPanel() = default;

	/// <summary>
	/// Constructor method used to instantiate a GUITextPanel object in system memory.
	/// </summary>
	/// <param name="owningManager"></param>
	explicit GUITextPanel(GUIControlManager *owningManager) { m_OwningManager = owningManager; }
#pragma endregion


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the panel.
// Arguments:       Position, Size.

    void Create(int X, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ChangeSkin
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the skin has been changed.
// Arguments:       New skin pointer.

    void ChangeSkin(GUISkin *Skin);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the panel
// Arguments:       Screen class

    void Draw(GUIScreen *Screen) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseDown
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
// Method:          OnKeyPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key is pressed (OnDown & repeating).
// Arguments:       KeyCode, Modifier.

    void OnKeyPress(int KeyCode, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the text in the textpanel.
// Arguments:       Text.

    void SetText(const std::string_view &Text);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetRightText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the extra text which appears right-justified in the textpanel.
// Arguments:       Text.

    void SetRightText(const std::string_view&rightText);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the text in the textpanel.
// Arguments:       None.

    std::string GetText() const { return m_Text; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRightText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the extra text which appears right-justified in the textpanel.
// Arguments:       None.

    std::string GetRightText() const { return m_RightText; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetSelection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the start and end indexes of the selection text.
// Arguments:       Start, End.

    void SetSelection(int Start, int End);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelectionStart
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the start index of the selection.
// Arguments:       None.
// Returns:         Index of the start of the selection. -1 if no selection

    int GetSelectionStart() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelectionEnd
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the end index of the selection.
// Arguments:       None.
// Returns:         Index of the end of the selection. -1 if no selection

    int GetSelectionEnd() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearSelection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the selection. Does NOT remove the selection text though.
// Arguments:       None.

    void ClearSelection() { m_GotSelection = false; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelectionText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the selection text.
// Arguments:       None.

    std::string GetSelectionText() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RemoveSelectionText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes the characters in the selection.
// Arguments:       None.

    void RemoveSelectionText();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCursorPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets where the cursor should be. This will clear any selection.
// Arguments:       The index of the new cursor position.

    void SetCursorPos(int cursorPos);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetLocked
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the locked state on the textbox.
// Arguments:       Locked.

    void SetLocked(bool Locked);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetLocked
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the locked state on the textbox.
// Arguments:       None.

    bool GetLocked() const { return m_Locked; }

	/// <summary>
	/// Gets whether this text panel accepts numeric symbols only.
	/// </summary>
	/// <returns>Whether this text panel accepts numeric symbols only.</returns>
	bool GetNumericOnly() const { return m_NumericOnly; }

	/// <summary>
	/// Sets this text panel to accept numeric symbols only.
	/// </summary>
	/// <param name="numericOnly">Whether to accept numeric symbols only or not.</param>
	void SetNumericOnly(bool numericOnly) { m_NumericOnly = numericOnly; }

	/// <summary>
	/// Gets this text panel's maximum numeric value when in numeric only mode.
	/// </summary>
	/// <returns>Maximum numeric value when in numeric only mode.</returns>
	int GetMaxNumericValue() const { return m_MaxNumericValue; }

	/// <summary>
	/// Sets this text panel's maximum numeric value when in numeric only mode.
	/// </summary>
	/// <param name="maxValue">The maximum numeric value. 0 means no maximum value.</param>
	void SetMaxNumericValue(int maxValue) { m_MaxNumericValue = maxValue; }

	/// <summary>
	/// Gets the maximum length of the text this text panel can contain.
	/// </summary>
	/// <returns>The maximum length of the text this text panel can contain.</returns>
	int GetMaxTextLength() const { return m_MaxTextLength; }

	/// <summary>
	/// Sets the maximum length of the text this text panel can contain.
	/// </summary>
	/// <param name="maxLength">The maximum length of the text this text panel can contain.</param>
	void SetMaxTextLength(int maxLength) { m_MaxTextLength = maxLength; }


private:

	std::string m_Text;
	std::string m_RightText; // Appears right-justified in the text field
	bool m_Focus = false;
	bool m_Locked = false;

	// The distance from the side and top of the text box, to the side and top of the first line of text
	int m_WidthMargin = 3;
	int m_HeightMargin = 0;

	// Cursor
	int m_CursorX = 0;
	int m_CursorY = 0;
	int m_CursorIndex = 0;
	int m_CursorColor = 0;
	int m_CursorBlinkCount = 0; // Hacky way to make cursor blink without a timer

	int m_StartIndex = 0;

	// Selection
	bool m_GotSelection = false;
	int m_StartSelection = 0;
	int m_EndSelection = 0;
	int m_SelectedColorIndex = 0;
	int m_SelectionX = 0;
	int m_SelectionWidth = 0;

	int m_MaxTextLength = 0; //!< The maximum length of the text this text panel can contain.
	bool m_NumericOnly = false; //!< Whether this text panel only accepts numeric symbols.
	int m_MaxNumericValue = 0; //!< The maximum numeric value when in numeric only mode. 0 means no maximum value.

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the cursor and start positions.
// Arguments:       Typing, Increment.

    void UpdateText(bool Typing = false, bool DoIncrement = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DoSelection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Update the selection.
// Arguments:       Start, End.

    void DoSelection(int Start, int End);

    /// <summary>
    /// Gets the index of the start of the next contiguous group of letters or special characters in the given string, or the end of the string if there is none.
    /// Generally used to deal with ctrl + arrows style behavior.
    /// </summary>
    /// <param name="stringToCheck">A string_view of the string to look for the next word in.</param>
    /// <param name="currentIndex">The index in the string to start looking from.</param>
    /// <returns>The index of the start of the next contiguous group of letters or special characters in the given string, or the end of the string if there is none.</returns>
    int GetStartOfNextCharacterGroup(const std::string_view &stringToCheck, int currentIndex) const;

    /// <summary>
    /// Gets the index of the start of the previous contiguous group of letters or special characters in the given string, or the end of the string if there is none.
    /// Generally used to deal with ctrl + arrows style behavior.
    /// </summary>
    /// <param name="stringToCheck">A string_view of the string to look for the next word in.</param>
    /// <param name="currentIndex">The index in the string to start looking from.</param>
    /// <returns>The index of the start of the previous contiguous group of letters or special characters in the given string, or the end of the string if there is none.</returns>
    int GetStartOfPreviousCharacterGroup(const std::string_view &stringToCheck, int currentIndex) const;
};
};
#endif