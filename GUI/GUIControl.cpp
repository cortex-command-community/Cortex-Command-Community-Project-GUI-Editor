#include "GUI.h"
#include "GUIWriter.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Clear() {
		m_Skin = nullptr;
		m_SkinPreset = 1;
		m_Properties.ClearProperties();
		m_ControlChildren.clear();
		m_ControlParent = nullptr;
		m_IsContainer = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(const std::string &name, int posX, int posY, int width, int height) {
		m_Properties.ClearProperties();
		m_Properties.AddProperty("Name", name);
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(GUIProperties *reference) {
		GUIAssert(reference, "");

		// Add the default variables
		m_Properties.AddProperty("Name", "");
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");

		m_Properties.OverwriteProperties(reference);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::ApplyProperties(GUIProperties *properties) {
		GUIAssert(properties, "");

		m_Properties.OverwriteProperties(properties);

		int X, Y;
		int Width, Height;
		bool Enabled;
		bool Visible;
		properties->GetPropertyValue("X", &X);
		properties->GetPropertyValue("Y", &Y);
		properties->GetPropertyValue("Width", &Width);
		properties->GetPropertyValue("Height", &Height);
		properties->GetPropertyValue("Enabled", &Enabled);
		properties->GetPropertyValue("Visible", &Visible);

		// Adjust position from parent
		GUIPanel *panel = GetPanel();
		if (panel && panel->GetParentPanel()) {
			int px;
			int py;
			int pw;
			int ph;
			panel->GetParentPanel()->GetRect(&px, &py, &pw, &ph);
			X += px;
			Y += py;
		}
		SetEnabled(Enabled);

		Move(X, Y);
		Resize(Width, Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::Save(GUIWriter *writer) {
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
		*writer << OutString;

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::GetEnabled() {
		if (const GUIPanel *panel = GetPanel()) {
			return panel->_GetEnabled();
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetEnabled(bool enabled) {
		// Default method is the grab the main panel and directly set its state. Controls that use multiple panels on the same layer will need to override this function
		if (GUIPanel *panel = GetPanel()) { panel->_SetEnabled(enabled); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::GetVisible() {
		if (const GUIPanel *panel = GetPanel()) {
			return panel->_GetVisible();
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetVisible(bool visible) {
		// Default method is the grab the main panel and directly set its state. Controls that use multiple panels on the same layer will need to override this function
		if (GUIPanel *panel = GetPanel()) { panel->_SetVisible(visible); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIControl::GetName() const {
		std::string name;
		m_Properties.GetPropertyValue("Name", &name);
		return name;
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

	std::string GUIControl::GetToolTip() const {
		std::string tip;
		m_Properties.GetPropertyValue("ToolTip", &tip);
		return tip;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::GetControlRect(int *posX, int *posY, int *width, int *height) const {
		// Zero the values for controls that don't override this
		if (posX) { *posX = 0; }
		if (posY) { *posY = 0; }
		if (width) { *width = 0; }
		if (height) { *height = 0; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddChild(GUIControl *control) {
		GUIAssert(control, "");

		// Remove the control from any previous parent
		if (control->GetParent()) { control->GetParent()->GUIControl::RemoveChild(control->GetName()); }

		control->m_ControlParent = this;
		m_ControlChildren.push_back(control);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveChild(const std::string_view &name) {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (std::vector<GUIControl *>::iterator it = m_ControlChildren.begin(); it != m_ControlChildren.end(); it++) {
			const GUIControl *C = *it;
			if (C && C->GetName().compare(name) == 0) {
				m_ControlChildren.erase(it);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveAllChildren() {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (const GUIControl *control : m_ControlChildren) {
			if (control) { m_ControlManager->RemoveControl(control->GetName(), false); }
		}
		m_ControlChildren.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData) {
		m_ControlManager->AddEvent(new GUIEvent(this, eventType, eventCode, eventData));
	}
}