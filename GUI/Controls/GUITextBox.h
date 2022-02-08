#ifndef _GUITEXTBOX_
#define _GUITEXTBOX_

#include "GUITextPanel.h"

namespace RTE {

/// <summary>
/// A TextBox control class.
/// </summary>
class GUITextBox : public GUITextPanel {

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
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUITextBox() override = default;
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
// Method:          StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

	void StoreProperties() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Move
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be moved.
// Arguments:       New position.

    void Move(int X, int Y) override { GUITextPanel::SetPositionAbs(X, Y); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be resized.
// Arguments:       New size.

    void Resize(int Width, int Height) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReceiveSignal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when receiving a signal.
// Arguments:       Signal source, Signal code, Signal data.

    void ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    void ApplyProperties(GUIProperties *Props) override;

private:

	static const std::string_view c_ControlType;

	static constexpr int m_MinWidth = 30;
	static constexpr int m_MinHeight = 10;
	static constexpr int m_DefaultWidth = 60;
	static constexpr int m_DefaultHeight = 16;

    int m_HAlignment = GUIFont::HAlignment::Left;
    int m_VAlignment = GUIFont::VAlignment::Top;
};
};
#endif