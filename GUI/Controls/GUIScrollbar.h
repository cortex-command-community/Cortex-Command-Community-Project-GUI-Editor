#ifndef _GUISCROLLBAR_
#define _GUISCROLLBAR_

namespace RTE {

/// <summary>
/// A Scrollbar control class.
/// </summary>
class GUIScrollbar : public GUIControl {

public:

	// Scroll panel orientation
	enum Orientation {
		Horizontal,
		Vertical
	};

	// Pre-built draw bitmaps
	enum {
		ButtonStates = 0,
		KnobStates,
		Back
	};

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUIScrollbar object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUIScrollbar(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

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
	~GUIScrollbar() override;
#pragma endregion


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          StoreProperties
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the control to store the values into properties.
	// Arguments:       None.

    void StoreProperties() override { SaveProps(&m_Properties); }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          ApplyProperties
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Applies new properties to the control.
	// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetMinimum
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the minimum value for the scrollpanel
	// Arguments:       Minimum value.

	void SetMinimum(int Min);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetMinimum
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the minimum value for the scrollpanel
	// Arguments:       None.

	int GetMinimum() const { return m_Minimum; }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetMaximum
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the maximum value for the scrollpanel
	// Arguments:       Maximum value.

	void SetMaximum(int Max);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetMaximum
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the maximum value for the scrollpanel
	// Arguments:       None.

	int GetMaximum() const { return m_Maximum; }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetValue
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the current value for the scrollpanel
	// Arguments:       Value.

	void SetValue(int Value);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetValue
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the current value of the scrollpanel.
	// Arguments:       None.

	int GetValue() const { return m_Value; }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetPageSize
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the page size value for the scrollpanel.
	// Arguments:       PageSize.

	void SetPageSize(int PageSize);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetPageSize
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the size of the page.
	// Arguments:       None.

	int GetPageSize() const { return m_PageSize; }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetOrientation
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the orientation of the scrollpanel.
	// Arguments:       Orientation.

	void SetOrientation(int Orientation);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetOrientation
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the orientation of the scrollpanel.
	// Arguments:       None.

	int GetOrientation() const { return m_Orientation; }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SetSmallChange
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Sets the small change value.
	// Arguments:       SmallChange.

	void SetSmallChange(int SmallChange);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          GetSmallChange
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Gets the small change value.
	// Arguments:       None.

	int GetSmallChange() const { return m_SmallChange; }

	/// <summary>
	/// Gets the value resolution for this scroll panel.
	/// </summary>
	/// <returns>The value resolution</returns>
	int GetValueResolution() const { return m_ValueResolution; }


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

	/// <summary>
	/// Called when the mouse is hovering over the panel (has to be enabled).
	/// </summary>
	/// <param name="mousePosX">Mouse Position.</param>
	/// <param name="mousePosY"></param>
	/// <param name="buttons">Mouse Buttons.</param>
	/// <param name="modifier">Modifier.</param>
	void OnMouseHover(int mousePosX, int mousePosY, int buttons, int modifier) override;

	/// <summary>
	/// Called when the mouse scroll wheel is moved.
	/// </summary>
	/// <param name="x">Mouse X position.</param>
	/// <param name="y">Mouse Y position.</param>
	/// <param name="modifier">Activated modifier buttons.</param>
	/// <param name="mouseWheelChange">The amount of wheel movement. Positive is scroll up, negative is scroll down.</param>
	void OnMouseWheelChange(int mousePosX, int mousePosY, int modifier, int mouseWheelChange) override;
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

	static constexpr int m_MinWidth = 9;
	static constexpr int m_MinHeight = 9;
	static constexpr int m_DefaultWidth = 50; // Note:: Default is Horizontal
	static constexpr int m_DefaultHeight = 12;

	// User attributes
	int m_Orientation = Orientation::Horizontal;
	int m_Minimum = 0;
	int m_Maximum = 100;
	int m_Value = 0;
	int m_PageSize = 1;
	int m_SmallChange = 1;
	int m_ValueResolution = 1; //!< How much the value increases/decreases on each mouse wheel change when scrolling.

	int m_KnobPosition = 0;
	std::array<GUIBitmap *, 3> m_DrawBitmaps = { nullptr, nullptr, nullptr };

	// Internal attributes
	bool m_RebuildSize = true;
	bool m_RebuildKnob = true;
	int m_ButtonSize = 17;
	int m_MinimumKnobSize = 9;

	int m_KnobLength = 0;
	std::array<bool, 2> m_ButtonPushed = { false, false };
	bool m_GrabbedKnob = false;
	bool m_GrabbedBackg = false;
	int m_GrabbedPos = 0;
	int m_GrabbedSide = 0;


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          SaveProps
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Save values to a property class.
	// Arguments:       Properties.

	void SaveProps(GUIProperties *Props) const;


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildBitmap
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Build the bitmap.
	// Arguments:       None.

	void BuildBitmap(bool UpdateSize, bool UpdateKnob);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildButton
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Build a button.
	// Arguments:       ArrowName, Width, Height.

	void BuildButton(const std::string &ArrowName, int Y, int Width, int Height);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildBackground
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Build the background.
	// Arguments:       None.

	void BuildBackground();


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildKnob
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Build the knob.
	// Arguments:       None.

	void BuildKnob(const std::string &Section, int X, int Y, int Width, int Height);


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          CalculateKnob
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Calculate the knob size and position.
	// Arguments:       None.

	void CalculateKnob();


	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          AdjustValue
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Adjusts the value.
	// Arguments:       Delta movement.

	void AdjustValue(int Delta);
};
};
#endif