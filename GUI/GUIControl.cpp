#include "GUI.h"
#include "GUIWriter.h"
#include "GUIControl.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		m_UniqueID = m_OwningManager->RequestUniqueID();

		m_ChildControls.clear();

		m_X = posX;
		m_Y = posY;
		m_Width = width;
		m_Height = height;

		m_Properties.ClearProperties();
		m_Properties.SetName(name);

		m_Properties.AddProperty("Name", std::string(name));
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");
		m_Properties.AddProperty("X", m_ParentControl ? m_X - m_ParentControl->m_X : m_X);
		m_Properties.AddProperty("Y", m_ParentControl ? m_Y - m_ParentControl->m_Y : m_Y);
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

		reference->GetPropertyValue("X", &m_X);
		reference->GetPropertyValue("Y", &m_Y);
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

		writer.NewPropertyWithValue("X", m_ParentControl ? GetRelPosX() : m_X);
		writer.NewPropertyWithValue("Y", m_ParentControl ? GetRelPosY() : m_Y);
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

	void GUIControl::CenterInParent(bool centerX, bool centerY) {
		int newRelX = m_X - m_ParentControl->GetPosX();
		int newRelY = m_Y - m_ParentControl->GetPosY();

		if (centerX) { newRelX = (m_ParentControl->GetWidth() / 2) - (GetWidth() / 2); }
		if (centerY) { newRelY = (m_ParentControl->GetHeight() / 2) - (GetHeight() / 2); }

		SetPositionRel(newRelX, newRelY);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetPositionAbs(int X, int Y, bool moveChildren) {
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;

		if (moveChildren) {
			for (GUIControl *childControl : m_ChildControls) {
				childControl->SetPositionAbs(childControl->m_X + DX, childControl->m_Y + DY);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::SetPositionRel(int X, int Y) {
		X += m_ParentControl->GetPosX();
		Y += m_ParentControl->GetPosY();
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;

		for (GUIControl *childControl : m_ChildControls) {
			childControl->SetPositionAbs(childControl->m_X + DX, childControl->m_Y + DY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::MoveRelative(int dX, int dY) {
		m_X += dX;
		m_Y += dY;

		for (GUIControl *childControl : m_ChildControls) {
			childControl->SetPositionAbs(childControl->m_X + dX, childControl->m_Y + dY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIRect * GUIControl::GetRect() {
		SetRect(&m_Rect, m_X, m_Y, m_X + m_Width, m_Y + m_Height);
		return &m_Rect;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::GetRect(int *posX, int *posY, int *width, int *height) const {
		if (posX) { *posX = m_X; }
		if (posY) { *posY = m_Y; }
		if (width) { *width = m_Width; }
		if (height) { *height = m_Height; }
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

	bool GUIControl::ChangeZPosition(ZPosChangeType changeType) {
		return m_ParentControl ? m_OwningManager->ChangeZPosition(m_UniqueID, changeType) : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControl::PointInside(int X, int Y) {
		// Can't be inside an invisible panel
		if (!m_Visible) {
			return false;
		}
		if (X < m_X || Y < m_Y) {
			return false;
		}
		if (X > m_X + m_Width || Y > m_Y + m_Height) {
			return false;
		}
		// Mouse is inside
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControl::TopPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIControl *CurPanel = nullptr;
		std::vector<GUIControl *>::reverse_iterator it;
		for (it = m_ChildControls.rbegin(); it != m_ChildControls.rend(); it++) {
			GUIControl *P = *it;
			if (P) {
				CurPanel = P->TopPanelUnderPoint(x, y);
				if (CurPanel != nullptr) {
					return CurPanel;
				}
			}
		}

		// Return this panel
		return this;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControl::BottomPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIControl *CurPanel = nullptr;
		;
		for (std::vector<GUIControl *>::iterator it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControl *P = *it;
			if (P) {
				CurPanel = P->BottomPanelUnderPoint(x, y);
				if (CurPanel != nullptr) {
					return CurPanel;
				}
			}
		}
		// Return this panel
		return this;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::AddChild(GUIControl *child, bool convertToAbsolutePos) {
		if (child) {
			// Convert the child's coordinates into absolute coordinates
			if (convertToAbsolutePos) {
				child->m_X += m_X;
				child->m_Y += m_Y;
			}

			// Make sure the rectangle is valid
			child->m_Width = std::max(child->m_Width, 0);
			child->m_Height = std::max(child->m_Height, 0);

			// Remove the child from any previous parent
			if (child->m_ParentControl) { child->m_ParentControl->RemoveChild(child->GetName()); }

			// Setup the inherited values
			child->m_ParentControl = this;

			// Add the child to the list
			m_ChildControls.push_back(child);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveChild(const std::string_view &name) {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (std::vector<GUIControl *>::iterator it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			const GUIControl *C = *it;
			if (C && C->GetName().compare(name) == 0) {
				m_ChildControls.erase(it);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::RemoveAllChildren() {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (const GUIControl *control : m_ChildControls) {
			if (control) { m_OwningManager->RemoveControl(control->GetName(), false); }
		}
		m_ChildControls.clear();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControl::Draw(GUIScreen *targetScreen) {
		GUIRect *clippingRect = GetRect();
		targetScreen->GetBitmap()->AddClipRect(clippingRect);

		for (GUIControl *childControl : m_ChildControls) {
			if (childControl->GetVisible()) {
				// Re-set the clipping rect since the last child has messed with it.
				targetScreen->GetBitmap()->SetClipRect(clippingRect);
				childControl->Draw(targetScreen);
			}
		}
		targetScreen->GetBitmap()->SetClipRect(nullptr);
	}
}