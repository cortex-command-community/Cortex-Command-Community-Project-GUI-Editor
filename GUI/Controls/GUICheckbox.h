#ifndef _GUICHECKBOX_
#define _GUICHECKBOX_

namespace RTE {

/// <summary>
/// A checkbox control class.
/// </summary>
class GUICheckbox : public GUIControl, public GUIPanel {

public:

    // Check types
    enum State {
        Unchecked = 0,
        Checked,
        Greycheck
    };

	GUIControlTypeGetter;
	GUIPanelOverrideMethods;

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUICheckbox
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUICheckbox object in
//                  system memory.
// Arguments:       GUIManager, GUIControlManager.

    GUICheckbox(GUIManager *Manager, GUIControlManager *ControlManager);


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
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been destroyed.
// Arguments:       None.

    void Destroy() override {}


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

	void Move(int X, int Y) override { SetPositionAbs(X, Y); }


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

	GUIBitmap *m_Image;
	GUIRect m_ImageRects[4];

	int m_Check;
	std::string m_Text;
	int m_Mouseover;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the checkbox bitmap to draw.
// Arguments:       None.

    void BuildBitmap();
};
};
#endif