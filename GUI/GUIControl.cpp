#include "GUI.h"
#include "GUIWriter.h"
#include "GUIControl.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		m_UniqueID = m_OwningManager->RequestUniqueID();

		m_PosX = posX;
		m_PosY = posY;
		m_Width = width;
		m_Height = height;

		m_Properties.ClearProperties();
		m_Properties.SetName(name);

		m_Properties.AddProperty("Name", std::string(name));
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");
		m_Properties.AddProperty("X", m_ParentControl ? GetRelPosX() : m_PosX);
		m_Properties.AddProperty("Y", m_ParentControl ? GetRelPosY() : m_PosY);
		m_Properties.AddProperty("Width", m_Width);
		m_Properties.AddProperty("Height", m_Height);
		m_Properties.AddProperty("Visible", m_Visible);
		m_Properties.AddProperty("Enabled", m_Enabled);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(GUIProperties *reference) {
		m_UniqueID = m_OwningManager->RequestUniqueID();

		m_Properties.AddProperty("Name", "");
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");

		reference->GetPropertyValue("X", &m_PosX);
		reference->GetPropertyValue("Y", &m_PosY);
		reference->GetPropertyValue("Width", &m_Width);
		reference->GetPropertyValue("Height", &m_Height);

		reference->GetPropertyValue("Visible", &m_Visible);
		reference->GetPropertyValue("Enabled", &m_Enabled);

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
		if (m_ParentControl) {
			int px;
			int py;
			int pw;
			int ph;
			m_ParentControl->GetRect(&px, &py, &pw, &ph);
			X += px;
			Y += py;
		}
		SetEnabled(Enabled);

		Move(X, Y);
		Resize(Width, Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Save(GUIWriter &writer) {
		StoreProperties();

		writer << ("[" + GetName() + "]");

		writer.NewPropertyWithValue("ControlType", GetControlType());
		writer.NewPropertyWithValue("Parent", m_ParentControl ? m_ParentControl->GetName() : "None");

		writer.NewPropertyWithValue("X", m_ParentControl ? GetRelPosX() : m_PosX);
		writer.NewPropertyWithValue("Y", m_ParentControl ? GetRelPosY() : m_PosY);
		writer.NewPropertyWithValue("Width", m_Width);
		writer.NewPropertyWithValue("Height", m_Height);
		writer.NewPropertyWithValue("Visible", m_Visible);
		writer.NewPropertyWithValue("Enabled", m_Enabled);

		m_Properties.Save(writer);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetFocus() {
		m_OwningManager->SetFocusedControl(this);
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
		std::string value[4];

		int Count = m_Properties.GetPropertyValue("Anchor", value, 4);

		for (int i = 0; i < Count; i++) {
			if (value[i] == "left") { Anchor |= Anchor::AnchorLeft; }
			if (value[i] == "top") { Anchor |= Anchor::AnchorTop; }
			if (value[i] == "right") { Anchor |= Anchor::AnchorRight; }
			if (value[i] == "bottom") { Anchor |= Anchor::AnchorBottom; }
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

	GUIRect * GUIControl::GetRect() {
		SetRect(&m_Rect, m_PosX, m_PosY, m_PosX + m_Width, m_PosY + m_Height);
		return &m_Rect;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::GetRect(int *posX, int *posY, int *width, int *height) const {
		if (posX) { *posX = m_PosX; }
		if (posY) { *posY = m_PosY; }
		if (width) { *width = m_Width; }
		if (height) { *height = m_Height; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::ChangeZPosition(ZPosChangeType changeType) {
		return m_ParentControl ? m_OwningManager->ChangeZPosition(m_UniqueID, changeType) : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControl::TopPanelUnderPoint(int x, int y) {
		if ((!m_Visible || !m_Enabled) || !PointInside(x, y)) {
			return nullptr;
		}
		if (m_IsContainer) {
			for (std::deque<GUIControl *>::reverse_iterator controlItr = m_Children.rbegin(); controlItr != m_Children.rend(); controlItr++) {
				if (GUIControl *control = *controlItr) {
					if (control->TopPanelUnderPoint(x, y)) {
						return control;
					}
				}
			}
		}
		return this;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddChild(GUIControl *child, bool convertToAbsolutePos) {
		if (child) {
			// Convert the child's coordinates into absolute coordinates
			if (convertToAbsolutePos) {
				child->m_PosX += m_PosX;
				child->m_PosY += m_PosY;
			}

			// Make sure the rectangle is valid
			child->m_Width = std::max(child->m_Width, 0);
			child->m_Height = std::max(child->m_Height, 0);

			// Remove the child from any previous parent
			if (child->m_ParentControl) { child->m_ParentControl->RemoveChild(child->GetName()); }

			// Setup the inherited values
			child->m_ParentControl = this;

			// Add the child to the list
			m_Children.push_back(child);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveChild(const std::string_view &name) {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (std::deque<GUIControl *>::iterator it = m_Children.begin(); it != m_Children.end(); it++) {
			const GUIControl *C = *it;
			if (C && C->GetName().compare(name) == 0) {
				m_Children.erase(it);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveAllChildren() {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (const GUIControl *control : m_Children) {
			if (control) { m_OwningManager->RemoveControl(control->GetName(), false); }
		}
		m_Children.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData) {
		m_OwningManager->AddEvent(GUIEvent(this, eventType, eventCode, eventData));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::CaptureMouse() {
		m_OwningManager->CaptureMouse(this);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::ReleaseMouse() {
		m_OwningManager->ReleaseMouse();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::TrackMouseHover(bool Enabled, float Delay) {
		m_OwningManager->TrackMouseHover(this, Enabled, Delay);
	}
}