#ifndef _GUIRADIOBUTTON_
#define _GUIRADIOBUTTON_

namespace RTE {

/// <summary>
/// A radiobutton control class.
/// </summary>
class GUIRadioButton : public GUIControl, public GUIPanel {

public:

	GUIControlTypeGetter;
	GUIPanelOverrideMethods;


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIRadioButton
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIRadioButton object in
//                  system memory.
// Arguments:       GUIManager, GUIControlManager.

    GUIRadioButton(GUIManager *Manager, GUIControlManager *ControlManager);


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
// Method:          Move
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be moved.
// Arguments:       New position.

    void Move(int X, int Y) override { GUIPanel::SetPositionAbs(X, Y); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be resized.
// Arguments:       New size.

    void Resize(int Width, int Height) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetControlRect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the rectangle of the control.
// Arguments:       Position, Size.

    void GetControlRect(int *X, int *Y, int *Width, int *Height) override { GUIPanel::GetRect(X, Y, Width, Height); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

    void StoreProperties() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the check state.
// Arguments:       State.

    void SetCheck(bool Check);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the check state.
// Arguments:       None.

    bool GetCheck() const { return m_Checked; }


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
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;

private:

	static const std::string_view c_ControlType;

    GUIBitmap *m_Image;
    GUIRect m_ImageRects[4];

    bool m_Checked;
    int m_Mouseover;
    std::string m_Text;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildBitmap
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Create the checkbox bitmap to draw.
	// Arguments:       None.

	void BuildBitmap();
};
};
#endif