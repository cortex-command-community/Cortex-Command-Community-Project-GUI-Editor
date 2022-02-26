#ifndef _GUICHECKBOX_
#define _GUICHECKBOX_

namespace RTE {

/// <summary>
/// A checkbox control class.
/// </summary>
class GUICheckbox : public GUIControl {

public:

    // Check types
    enum State {
        Unchecked = 0,
        Checked,
        Greycheck
    };

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUICheckbox object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
    explicit GUICheckbox(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

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
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUICheckbox() override = default;
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
// Method:          OnMouseEnter
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse enters the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseEnter(int X, int Y, int Buttons, int Modifier) override { m_Mouseover = true; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseLeave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse leaves the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseLeave(int X, int Y, int Buttons, int Modifier) override { m_Mouseover = false; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

    void StoreProperties() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the text.
// Arguments:       Text.

	void SetText(const std::string &Text) { m_Text = Text; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the text.
// Arguments:       None.

	std::string GetText() const { return m_Text; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the check state.
// Arguments:       Check state.

	void SetCheck(int Check) { m_Check = Check; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the check state.
// Arguments:       None.

	int GetCheck() const { return m_Check; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 40;
	static constexpr int m_MinHeight = 10;
	static constexpr int m_DefaultWidth = 60;
	static constexpr int m_DefaultHeight = 16;

	GUIBitmap *m_Image = nullptr;

	std::array<GUIRect, 4> m_ImageRects;
	std::string m_Text;

	int m_Check = State::Unchecked;
	bool m_Mouseover = false;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the checkbox bitmap to draw.
// Arguments:       None.

    void BuildBitmap();
};
};
#endif