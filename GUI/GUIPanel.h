#ifndef _GUIPANEL_
#define _GUIPANEL_

#include "GUIConstants.h"

namespace RTE {

class GUIPanel;
class GUIManager;

/// <summary>
/// A rectangle 'window' in the GUI that recieves mouse and keyboard events.
/// </summary>
class GUIPanel {

public:

    // Mouse buttons
    enum MouseButtons {
        MOUSE_NONE = 0x00,
        MOUSE_LEFT = 0x01,
        MOUSE_MIDDLE = 0x02,
        MOUSE_RIGHT = 0x04,
    };

    // Mouse Modifiers
    enum MouseModifiers {
        MODI_NONE = 0x00,
        MODI_SHIFT = 0x01,
        MODI_CTRL = 0x02,
        MODI_ALT = 0x04,
		MODI_COMMAND = 0x08
    };

    // Z Change
    enum ZChange {
        TopMost=0,
        BottomMost,
    };


#pragma region Global Macro Definitions
	#define GUIPanelOverrideMethods \
		GUIPanel * GetPanel() override { return this; }
#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIPanel object in system
//                  memory.
// Arguments:       Manager.

    explicit GUIPanel(GUIManager *Manager);


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIPanel object in system
//                  memory.
// Arguments:       None.

	GUIPanel() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the settings.
// Arguments:       None.

    void Clear();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BottomPanelUnderPoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Recursively goes down the tree to check the last panel under a point
// Arguments:       X, Y Coordinates of point
// Return value:    A pointer to the panel. 0 if no panel is under the point

    GUIPanel *BottomPanelUnderPoint(int x, int y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TopPanelUnderPoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Recursively goes up the tree from to check the first panel under a point
// Arguments:       X, Y Coordinates of point
// Return value:    A pointer to the panel. 0 if no panel is under the point

    GUIPanel *TopPanelUnderPoint(int x, int y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddChild
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a child to this panel
// Arguments:       Pointer to the panel to add

    void AddChild(GUIPanel *child, bool convertToAbsolutePos = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RemoveChild
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes a child based on name.
// Arguments:       Child Name.

    void RemoveChild(const GUIPanel *pChild);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Setup
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets up the panel for use with the manager.
// Arguments:       Pointer to the manager to use, ZPosition.

    void Setup(GUIManager *manager, int ZPos);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads the base data from a properties page
// Arguments:       Pointer to the properties class

    void LoadProperties(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the panel
// Arguments:       Screen class

    virtual void Draw(GUIScreen *Screen);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes down on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseDown(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes up on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseUp(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnDoubleClick
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse has double-clicked on the pane.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnDoubleClick(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseMove
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse moves (over the panel, or when captured).
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseMove(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseEnter
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse enters the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseEnter(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseLeave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse leaves the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseLeave(int X, int Y, int Buttons, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnMouseHover
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse is hovering over the panel (has to be enabled)
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	virtual void OnMouseHover(int X, int Y, int Buttons, int Modifier) {}


	/// <summary>
	/// Called when the mouse scroll wheel is moved.
	/// </summary>
	/// <param name="x">Mouse X position.</param>
	/// <param name="y">Mouse Y position.</param>
	/// <param name="modifier">Activated modifier buttons.</param>
	/// <param name="mouseWheelChange">The amount of wheel movement. Positive is scroll up, negative is scroll down.</param>
	virtual void OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) {};


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnKeyDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key goes down.
// Arguments:       KeyCode, Modifier.

	virtual void OnKeyDown(int KeyCode, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnKeyUp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key goes up.
// Arguments:       KeyCode, Modifier.

	virtual void OnKeyUp(int KeyCode, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnKeyPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key is pressed (OnDown & repeating).
// Arguments:       KeyCode, Modifier.

	virtual void OnKeyPress(int KeyCode, int Modifier) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnGainFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel gains focus.
// Arguments:       None.

    virtual void OnGainFocus() { m_GotFocus = true; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnLoseFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel looses focus.
// Arguments:       None.

    virtual void OnLoseFocus() { m_GotFocus = false; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SetSize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the size of the panel.
// Arguments:       Width, Height.

    virtual void SetSize(int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SetPositionAbs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the absolute position of the panel.
// Arguments:       X, Y, and whether to move the children too

    virtual void SetPositionAbs(int X, int Y, bool moveChildren = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SetPositionRel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the position of the panel, relative to its parent.
// Arguments:       X, Y.

    virtual void SetPositionRel(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  MoveRelative
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Moves the position of the panel by a relative amount.
// Arguments:       X, Y, relative.

    virtual void MoveRelative(int dX, int dY);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  CenterInParent
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Centers this in its parent, taking this' dimensions into consideration.
// Arguments:       Which axes to center.

    virtual void CenterInParent(bool centerX, bool centerY);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  ReceiveSignal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when receiving a signal.
// Arguments:       Signal source, Signal code, Signal data.

    virtual void ReceiveSignal(GUIPanel *Source, GUIEventCode Code, int Data) {}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CaptureMouse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Captures the mouse.
// Arguments:       None.

    void CaptureMouse();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReleaseMouse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Releases the mouse.
// Arguments:       None.

    void ReleaseMouse();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SetFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the font this panel will be using
// Arguments:       The new font, ownership is NOT transferred!

    virtual void SetFont(GUIFont *pFont) { m_Font = pFont; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _SetVisible
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the visibility of the panel.
// Arguments:       Visible.

    void _SetVisible(bool Visible) { m_Visible = Visible; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _GetVisible
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the visibility of the panel.
// Arguments:       None.

    bool _GetVisible() const { return m_Visible; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _SetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the enabled state of the panel.
// Arguments:       Enabled.

    void _SetEnabled(bool Enabled) { m_Enabled = Enabled; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _GetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the enabled state of the panel.
// Arguments:       None.

    bool _GetEnabled() const { return m_Enabled; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetWidth
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the width of the panel.
// Arguments:       None.

    int GetWidth() const { return m_Width; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the height of the panel.
// Arguments:       None.

    int    GetHeight() const { return m_Height; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetXPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the x position of the panel
// Arguments:       None.

    int    GetXPos() const { return m_X; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetYPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the y position of the panel
// Arguments:       None.

    int    GetYPos() const { return m_Y; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRelXPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the x position of the panel, relative to its parent
// Arguments:       None.

    int    GetRelXPos() const { return m_X - m_Parent->GetXPos(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRelYPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the y position of the panel, relative to its parent
// Arguments:       None.

    int    GetRelYPos() const { return m_Y - m_Parent->GetYPos(); }



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the rectangle of the panel.
// Arguments:       None.

    GUIRect * GetRect();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the rectangle of the panel.
// Arguments:       X, Y, Width, Height

    void GetRect(int *X, int *Y, int *Width, int *Height) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetParentPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the parent of this panel.
// Arguments:       None.

    GUIPanel * GetParentPanel() { return m_Parent; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPanelID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the panel's ID.
// Arguments:       None.

    int GetPanelID() const { return m_ID; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCaptureState
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the panel's captured state.
// Arguments:       Captured.

    void SetCaptureState(bool Captured) { m_Captured = Captured; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsCaptured
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the panel's captured state.
// Arguments:       None.

    bool IsCaptured() const { return m_Captured; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the panel's enabled state.
// Arguments:       None.

    bool IsEnabled() const { return m_Enabled; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetSignalTarget
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the target panel to receive signals.
// Arguments:       Target panel.

    void SetSignalTarget(GUIPanel *Target);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          PointInside
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if a point is inside the panel
// Arguments:       X, Y Coordinates of point
// Return value:    A boolean of the check result

    virtual bool PointInside(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the focus of this panel.
// Arguments:       None.

    void SetFocus();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HasFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the focus value of the panel.
// Arguments:       None.

    bool HasFocus() const { return m_GotFocus; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetZPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the Z index of the panel.
// Arguments:       ZPos.

    void SetZPos(int Z) { m_ZPos = Z; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetZPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the Z index of the panel.
// Arguments:       None.

    int GetZPos() const { return m_ZPos; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ChangeZPosition
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the Z Position of the panel.
// Arguments:       Change type.

    void ChangeZPosition(int Type);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Convert the properties in the panel to a string.
// Arguments:       None.

    std::string ToString();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds this panels properties to a properties class.
// Arguments:       GUIProperties.

    void BuildProperties(GUIProperties *Prop);

protected:

	int m_X; // absolute coordinates
	int m_Y;
	int m_Width;
	int m_Height;

	bool m_Visible;
	bool m_Enabled;
	bool m_GotFocus;
	bool m_Captured;
	GUIManager *m_Manager;
	GUIPanel *m_Parent;

	GUIFont *m_Font;
	int m_FontColor;
	int m_FontShadow;
	int m_FontKerning;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SendSignal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sends a signal to the target.
// Arguments:       Signal code, Data.

    void SendSignal(GUIEventCode Code, int Data);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TrackMouseHover
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets up the manager to enable/disable hover tracking of this panel
// Arguments:       Enabled, Delay (milliseconds)

    void TrackMouseHover(bool Enabled, int Delay);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _ChangeZ
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the Z position of a child panel.
// Arguments:       Child panel, Change type.

    void _ChangeZ(GUIPanel *Child, int Type);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WriteValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Writes a single value to string.
// Arguments:       Value name, Value.

    std::string WriteValue(const std::string &Name, int Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WriteValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Writes a single value to string.
// Arguments:       Value name, Value.

    std::string WriteValue(const std::string &Name, bool Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          _ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the panel.
// Arguments:       GUIProperties.

    void _ApplyProperties(GUIProperties *Props);

	private:

		std::vector<GUIPanel *> m_Children;
		GUIRect m_Rect;

		int m_ID;
		int m_ZPos;

		GUIPanel *m_SignalTarget;
};
};
#endif