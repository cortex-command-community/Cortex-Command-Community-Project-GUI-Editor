#ifndef _GUISLIDER_
#define _GUISLIDER_

namespace RTE {

/// <summary>
/// A slider control class.
/// </summary>
class GUISlider : public GUIControl {

public:

    // Slider orientation
    enum Orientation {
        Horizontal,
        Vertical
    };

    // Tick Direction
    enum TickDirection {
        TopLeft,
        BottomRight
    };

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUISlider object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUISlider(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

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
	~GUISlider() override;
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


	/// <summary>
	/// Called when the mouse scroll wheel is moved.
	/// </summary>
	/// <param name="x">Mouse X position.</param>
	/// <param name="y">Mouse Y position.</param>
	/// <param name="modifier">Activated modifier buttons.</param>
	/// <param name="mouseWheelChange">The amount of wheel movement. Positive is scroll up, negative is scroll down.</param>
	void OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) override;


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
// Method:          SetOrientation
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the orientation of the slider.
// Arguments:       Orientation.

    void SetOrientation(int Orientation);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetOrientation
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the orientation of the slider.
// Arguments:       None.

    int GetOrientation() const { return m_Orientation; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTickDirection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the direction of the ticks.
// Arguments:       TickDir.

    void SetTickDirection(int TickDir);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTickDirection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the direction of the ticks.
// Arguments:       None.

    int GetTickDirection() const { return m_TickDirection; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMinimum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the minimum value.
// Arguments:       Minimum.

    void SetMinimum(int Minimum);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMinimum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the minimum value.
// Arguments:       None.

    int GetMinimum() const { return m_Minimum; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMaximum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the maximum value.
// Arguments:       Maximum.

    void SetMaximum(int Maximum);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMaximum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the maximum value.
// Arguments:       None.

    int GetMaximum() const { return m_Maximum; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the value.
// Arguments:       Value.

    void SetValue(int Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the value.
// Arguments:       None.

    int GetValue() const { return m_Value; }


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


	/// <summary>
	/// Sets the value resolution for this slider.
	/// </summary>
	/// <param name="valueRes">The new value resolution</param>
	void SetValueResolution(int valueRes);

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 40;
	static constexpr int m_MinHeight = 10;
	static constexpr int m_DefaultWidth = 100;
	static constexpr int m_DefaultHeight = 20;

	GUIBitmap *m_KnobImage = nullptr;

	// Properties
	int m_Orientation = Orientation::Horizontal;
	int m_TickDirection = TickDirection::BottomRight;


	// Internal variables
	int m_EndThickness = 0;
	int m_OldValue = 0;

	int m_Minimum = 0;
	int m_Maximum = 100;
	int m_Value = 0;
	int m_ValueResolution = 1; //!< How much the value increases/decreases on each mouse wheel change when scrolling.

	int m_KnobPosition = 0;
	int m_KnobLength = 0;
	bool m_GrabbedKnob = false;
	int m_GrabbedPos = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the slider bitmap to draw.
// Arguments:       None.

    void BuildBitmap();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Builds the background line for the slider
// Arguments:       Section, SrcImage.

    void BuildLine(const std::string &Section, GUIBitmap *SrcImage);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateKnob
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculates the knob position and size.
// Arguments:       None.

    void CalculateKnob();
};
};
#endif