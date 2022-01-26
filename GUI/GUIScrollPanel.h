#ifndef _GUISCROLLPANEL_
#define _GUISCROLLPANEL_

namespace RTE {

/// <summary>
/// A scrollbar panel class used for controls requiring a scrollbar.
/// </summary>
class GUIScrollPanel : public GUIPanel {

public:

    // Scroll panel orientation
    enum {
        Horizontal,
        Vertical
    };

    // Pre-built draw bitmaps
    enum {
        ButtonStates=0,
        KnobStates,
        Back
    };

    // Signals
    enum {
        ChangeValue=0,
        Grab,
        Release
    };


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIScrollPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIScrollPanel object in
//                  system memory.
// Arguments:       GUIManager.

    explicit GUIScrollPanel(GUIManager *Manager);


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIScrollPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIScrollPanel object in
//                  system memory.
// Arguments:       None.

    GUIScrollPanel();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the scrollpanel
// Arguments:       Position, Size.

    void Create(int X, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel has been destroyed.
// Arguments:       None.

    void Destroy(); 


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
// Method:          OnMouseUp
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
// Method:          OnMouseHover
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse is hovering over the panel (has to be enabled)
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseHover(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the size of the panel.
// Arguments:       Width, Height.

    void SetSize(int Width, int Height);


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

protected:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadProps
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Load values from a property class.
// Arguments:       Properties.

    void LoadProps(GUIProperties *Props);


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

private:

	GUISkin *m_Skin;
	GUIBitmap *m_DrawBitmap[3];

	// User attributes
	int m_Orientation;
	int m_Minimum;
	int m_Maximum;
	int m_Value;
	int m_PageSize;
	int m_SmallChange;

	// Internal attributes
	bool m_RebuildSize;
	bool m_RebuildKnob;
	int m_ButtonSize;
	int m_MinimumKnobSize;
	int m_KnobPosition;
	int m_KnobLength;
	bool m_ButtonPushed[2];
	bool m_GrabbedKnob;
	bool m_GrabbedBackg;
	int m_GrabbedPos;
	int m_GrabbedSide;
	int m_ValueResolution; //!< How much the value increases/decreases on each mouse wheel change when scrolling.

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