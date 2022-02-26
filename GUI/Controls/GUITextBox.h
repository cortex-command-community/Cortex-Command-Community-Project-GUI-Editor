#ifndef _GUITEXTBOX_
#define _GUITEXTBOX_

namespace RTE {

/// <summary>
/// A TextBox control class.
/// </summary>
class GUITextBox : public GUIControl {

public:

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUITextBox object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUITextBox(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

	/// <summary>
	/// Called when the control has been created.
	/// </summary>
	/// <param name="name">Name.</param>
	/// <param name="posX">Position.</param>
	/// <param name="posY"></param>
	/// <param name="width">Size.</param>
	/// <param name="height"></param>
	void Create(const std::string_view &name, int posX, int posY, int width = -1, int height = -1) override {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);
	}

	/// <summary>
	/// Called when the control has been created.
	/// </summary>
	/// <param name="reference">Properties.</param>
	void Create(GUIProperties *reference) override;
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUITextBox() override = default;
#pragma endregion

#pragma region Getters and Setters
	/// <summary>
	/// Gets the locked state on the textbox.
	/// </summary>
	/// <returns>None.</returns>
	bool GetReadOnly() const { return m_ReadOnly; }

	/// <summary>
	/// Sets the locked state on the textbox.
	/// </summary>
	/// <param name="locked">Locked.</param>
	void SetReadOnly(bool readOnly) { m_ReadOnly = readOnly; if (m_ReadOnly) { ClearSelection(); } }

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
#pragma endregion




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

	/// <summary>
	/// Gets the start index of the selection.
	/// </summary>
	/// <returns>Index of the start of the selection. -1 if no selection.</returns>
	int GetSelectionStart() const { return m_GotSelection ? m_StartSelection : -1; }

	/// <summary>
	/// Gets the end index of the selection.
	/// </summary>
	/// <returns>Index of the end of the selection. -1 if no selection.</returns>
	int GetSelectionEnd() const { return m_GotSelection ? m_EndSelection : -1; }



#pragma region Mouse Event Handling
	/// <summary>
	/// Called when the mouse goes down on the panel.
	/// </summary>
	/// <param name="mousePosX">Mouse Position.</param>
	/// <param name="mousePosY"></param>
	/// <param name="buttons">Mouse Buttons.</param>
	/// <param name="modifier">Modifier.</param>
	void OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) override;

	/// <summary>
	/// Called when the mouse goes up on the panel.
	/// </summary>
	/// <param name="mousePosX">Mouse Position.</param>
	/// <param name="mousePosY"></param>
	/// <param name="buttons">Mouse Buttons.</param>
	/// <param name="modifier">Modifier.</param>
	void OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) override;

	/// <summary>
	/// Called when the mouse moves (over the panel, or when captured).
	/// </summary>
	/// <param name="mousePosX">Mouse Position.</param>
	/// <param name="mousePosY"></param>
	/// <param name="buttons">Mouse Buttons.</param>
	/// <param name="modifier">Modifier.</param>
	void OnMouseMove(int mousePosX, int mousePosY, int buttons, int modifier) override;
#pragma endregion

#pragma region Keyboard Event Handling
	/// <summary>
	/// Called when a key is pressed (OnDown & repeating).
	/// </summary>
	/// <param name="keyCode">KeyCode.</param>
	/// <param name="modifier">Modifier.</param>
	void OnKeyPress(int keyCode, int modifier) override;
#pragma endregion

#pragma region Virtual Override Methods
	/// <summary>
	/// Called when the skin has been changed.
	/// </summary>
	/// <param name="newSkin">New skin pointer.</param>
	void ChangeSkin(GUISkin *newSkin) override;

	/// <summary>
	/// Draws the panel.
	/// </summary>
	/// <param name="targetScreen">Screen class.</param>
	void Draw(GUIScreen *targetScreen) override;
#pragma endregion

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 30;
	static constexpr int m_MinHeight = 10;
	static constexpr int m_DefaultWidth = 60;
	static constexpr int m_DefaultHeight = 16;

	bool m_ReadOnly = false;

    int m_HAlignment = GUIFont::HAlignment::Left;
    int m_VAlignment = GUIFont::VAlignment::Top;

	std::string m_Text = "";
	std::string m_RightText = ""; //!< Appears right-justified in the text field.

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