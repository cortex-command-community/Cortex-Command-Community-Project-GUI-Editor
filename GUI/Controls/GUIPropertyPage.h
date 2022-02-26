#ifndef _GUIPROPERTYPAGE_
#define _GUIPROPERTYPAGE_

#include "GUIScrollbar.h"

namespace RTE {

	class GUITextBox;

/// <summary>
/// A property page control class.
/// </summary>
class GUIPropertyPage : public GUIControl {

public:

	GUIControlOverrideMethods;

#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUIPropertyPage object in system memory.
	/// </summary>
	/// <param name="owningManager">GUIControlManager.</param>
	explicit GUIPropertyPage(GUIControlManager *owningManager) : m_VertScroll(owningManager) { m_OwningManager = owningManager; }

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
	~GUIPropertyPage() override = default;
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

    void OnMouseUp(int X, int Y, int Buttons, int Modifier) override {}


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

    void OnMouseEnter(int X, int Y, int Buttons, int Modifier) override {}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseLeave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse leaves the panel.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseLeave(int X, int Y, int Buttons, int Modifier) override {}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties. // Note: This is for saving the control, not related directly to our control type
// Arguments:       None.

	void StoreProperties() override {}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPropertyValues
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Refreshes the page with new variables & values.
// Arguments:       GUIProperties.

    void SetPropertyValues(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPropertyValues
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the properties in the page.
// Arguments:       None.

	GUIProperties * GetPropertyValues() { return &m_PageValues; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReceiveSignal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when receiving a signal.
// Arguments:       Signal source, Signal code, Signal data.

    void ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearValues
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the property page values.
// Arguments:       None.

    void ClearValues();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          InvokeUpdate
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Invokes an explicit update on text panels to property page.
// Arguments:       None.
// Returns:         Boolean whether or not any values have changed.

    bool InvokeUpdate();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HasTextFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if any of the visible text panels have focus.
// Arguments:       None.

    bool HasTextFocus();

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 50;
	static constexpr int m_MinHeight = 50;
	static constexpr int m_DefaultWidth = 80;
	static constexpr int m_DefaultHeight = 100;

	int m_LineColor = 0;

    GUIProperties m_PageValues;
    std::vector<GUITextBox *> m_TextPanelList;
	GUIScrollbar m_VertScroll;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Method:          BuildBitmap
	//////////////////////////////////////////////////////////////////////////////////////////
	// Description:     Create the property page bitmap to draw.
	// Arguments:       None.

	void BuildBitmap();
};
};
#endif