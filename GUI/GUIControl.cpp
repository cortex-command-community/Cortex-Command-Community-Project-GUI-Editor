#include "GUI.h"
#include "GUIWriter.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl::GUIControl() {
		m_Skin = nullptr;
		m_SkinPreset = 1;
		m_Properties.ClearProperties();
		m_ControlChildren.clear();
		m_ControlParent = nullptr;
		m_IsContainer = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		m_Properties.ClearProperties();
		m_Properties.AddProperty("Name", Name);
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(GUIProperties *Props) {
		GUIAssert(Props, "");

		// Add the default variables
		m_Properties.AddProperty("Name", "");
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");

		m_Properties.OverwriteProperties(Props);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData) {
		m_ControlManager->AddEvent(new GUIEvent(this, eventType, eventCode, eventData));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIControl::GetName() {
		std::string Name;
		m_Properties.GetPropertyValue("Name", &Name);

		return Name;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIControl::GetToolTip() {
		std::string tip;
		m_Properties.GetPropertyValue("ToolTip", &tip);

		return tip;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddChild(GUIControl *Control) {
		GUIAssert(Control, "");

		// Remove the control from any previous parent
		if (Control->GetParent()) { Control->GetParent()->GUIControl::RemoveChild(Control->GetName()); }

		Control->m_ControlParent = this;
		m_ControlChildren.push_back(Control);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::Save(GUIWriter *W) {
		std::string OutString = "";
		std::string Name;

		// Get the control to store its properties
		StoreProperties();

		// Section Header
		m_Properties.GetPropertyValue("Name", &Name);

		OutString.append("[");
		OutString.append(Name);
		OutString.append("]\n");

		// General control values
		OutString.append("ControlType = ");
		OutString += GetControlType();
		OutString.append("\n");

		// Parent
		OutString += "Parent = ";
		if (m_ControlParent) {
			OutString += m_ControlParent->GetName();
		} else {
			OutString += "None";
		}
		OutString += "\n";

		// Get the main panel and write its location
		GUIPanel *Pan = GetPanel();
		if (Pan) { OutString.append(Pan->ToString()); }

		// Write out the properties
		OutString.append(m_Properties.ToString());

		// Write to the writer class
		*W << OutString;

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::GetControlRect(int *posX, int *posY, int *width, int *height) {
		// Zero the values for controls that don't override this
		if (posX) { *posX = 0; }
		if (posY) { *posY = 0; }
		if (width) { *width = 0; }
		if (height) { *height = 0; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIControl::GetAnchor() {
		int Anchor = 0;
		std::string Value[4];

		int Count = m_Properties.GetPropertyValue("Anchor", Value, 4);

		for (int i = 0; i < Count; i++) {
			if (stricmp(Value[i].c_str(), "left") == 0) { Anchor |= Anchor::AnchorLeft; }
			if (stricmp(Value[i].c_str(), "top") == 0) { Anchor |= Anchor::AnchorTop; }
			if (stricmp(Value[i].c_str(), "right") == 0) { Anchor |= Anchor::AnchorRight; }
			if (stricmp(Value[i].c_str(), "bottom") == 0) { Anchor |= Anchor::AnchorBottom; }
		}

		// The anchor cannot have both sides missing, so we default to Left, Top is that is the case
		if (!(Anchor & Anchor::AnchorLeft) && !(Anchor & Anchor::AnchorRight)) { Anchor |= Anchor::AnchorLeft; }
		if (!(Anchor & Anchor::AnchorTop) && !(Anchor & Anchor::AnchorBottom)) { Anchor |= Anchor::AnchorTop; }

		return Anchor;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetVisible(bool Visible) {
		// Default method is the grab the main panel and directly set its state. Controls that use multiple panels on the same layer will need to override this function
		GUIPanel *Panel = GetPanel();
		if (Panel) { Panel->_SetVisible(Visible); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::GetVisible() {
		// See SetVisible() comment
		GUIPanel *Panel = GetPanel();
		if (Panel) {
			return Panel->_GetVisible();
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetEnabled(bool Enabled) {
		// See SetVisible() comment
		GUIPanel *Panel = GetPanel();
		if (Panel) { Panel->_SetEnabled(Enabled); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::GetEnabled() {
		// See SetVisible() comment
		GUIPanel *Panel = GetPanel();
		if (Panel) { return Panel->_GetEnabled(); }

		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::ApplyProperties(GUIProperties *Props) {
		GUIAssert(Props, "");

		m_Properties.OverwriteProperties(Props);

		int X, Y;
		int Width, Height;
		bool Enabled;
		bool Visible;
		Props->GetPropertyValue("X", &X);
		Props->GetPropertyValue("Y", &Y);
		Props->GetPropertyValue("Width", &Width);
		Props->GetPropertyValue("Height", &Height);
		Props->GetPropertyValue("Enabled", &Enabled);
		Props->GetPropertyValue("Visible", &Visible);

		// Adjust position from parent
		GUIPanel *P = GetPanel();
		if (P && P->GetParentPanel()) {
			int px;
			int py;
			int pw;
			int ph;
			P->GetParentPanel()->GetRect(&px, &py, &pw, &ph);
			X += px;
			Y += py;
		}

		SetEnabled(Enabled);

		Move(X, Y);
		Resize(Width, Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveChild(const std::string Name) {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		std::vector<GUIControl *>::iterator it;

		for (it = m_ControlChildren.begin(); it != m_ControlChildren.end(); it++) {
			GUIControl *C = *it;
			if (C && C->GetName().compare(Name) == 0) {
				m_ControlChildren.erase(it);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveChildren() {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		std::vector<GUIControl *>::iterator it;

		for (it = m_ControlChildren.begin(); it != m_ControlChildren.end(); it++) {
			GUIControl *C = *it;
			if (C) { m_ControlManager->RemoveControl(C->GetName(), false); }
		}

		m_ControlChildren.clear();
	}
}