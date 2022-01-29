#include "GUI.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::Clear() {
		m_X = 0;
		m_Y = 0;
		m_Width = 0;
		m_Height = 0;
		m_ID = -1;
		m_GotFocus = false;
		m_Captured = false;
		m_Visible = true;
		m_Enabled = true;

		m_Parent = nullptr;
		m_Children.clear();

		m_Manager = nullptr;
		m_SignalTarget = this;
		m_ZPos = 0;

		m_Font = nullptr;
		m_FontColor = 0;
		m_FontShadow = 0;
		m_FontKerning = 1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::Create(GUIManager *manager) {
		m_Manager = manager;
		m_Font = nullptr;
		m_FontColor = 0;
		m_FontShadow = 0;
		m_FontKerning = 1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::LoadProperties(const GUIProperties *properties) {
		GUIAssert(properties, "");

		properties->GetPropertyValue("X", &m_X);
		properties->GetPropertyValue("Y", &m_Y);
		properties->GetPropertyValue("Width", &m_Width);
		properties->GetPropertyValue("Height", &m_Height);

		properties->GetPropertyValue("Visible", &m_Visible);
		properties->GetPropertyValue("Enabled", &m_Enabled);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::BuildProperties(GUIProperties *properties) const {
		GUIAssert(properties, "");

		// Subtract the position from the parent
		int X = m_X;
		int Y = m_Y;

		if (m_Parent) {
			X -= m_Parent->m_X;
			Y -= m_Parent->m_Y;
		}

		properties->AddProperty("X", X);
		properties->AddProperty("Y", Y);
		properties->AddProperty("Width", m_Width);
		properties->AddProperty("Height", m_Height);
		properties->AddProperty("Visible", m_Visible);
		properties->AddProperty("Enabled", m_Enabled);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::_ApplyProperties(const GUIProperties *properties) {
		GUIAssert(properties, "");

		properties->GetPropertyValue("Visible", &m_Visible);
		properties->GetPropertyValue("Enabled", &m_Enabled);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIPanel::ToString() const {
		std::string OutString = "";

		// Subtract the position from the parent
		int X = m_X;
		int Y = m_Y;

		if (m_Parent) {
			X -= m_Parent->m_X;
			Y -= m_Parent->m_Y;
		}

		OutString += WriteValue("X", X);
		OutString += WriteValue("Y", Y);
		OutString += WriteValue("Width", m_Width);
		OutString += WriteValue("Height", m_Height);
		OutString += WriteValue("Visible", m_Visible);
		OutString += WriteValue("Enabled", m_Enabled);

		return OutString;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::SetFocus() {
		m_Manager->SetFocus(this);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::SetPositionAbs(int X, int Y, bool moveChildren) {
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;
		// Move children
		if (moveChildren) {
			std::vector<GUIPanel *>::iterator it;
			for (it = m_Children.begin(); it != m_Children.end(); it++) {
				GUIPanel *P = *it;
				P->SetPositionAbs(P->m_X + DX, P->m_Y + DY);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::SetPositionRel(int X, int Y) {
		X += m_Parent->GetXPos();
		Y += m_Parent->GetYPos();
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;

		// Move children
		std::vector<GUIPanel *>::iterator it;
		for (it = m_Children.begin(); it != m_Children.end(); it++) {
			GUIPanel *P = *it;
			P->SetPositionAbs(P->m_X + DX, P->m_Y + DY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::MoveRelative(int dX, int dY) {
		m_X += dX;
		m_Y += dY;

		// Move children
		std::vector<GUIPanel *>::iterator it;
		for (it = m_Children.begin(); it != m_Children.end(); it++) {
			GUIPanel *P = *it;
			P->SetPositionAbs(P->m_X + dX, P->m_Y + dY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::CenterInParent(bool centerX, bool centerY) {
		int newRelX = m_X - m_Parent->GetXPos();
		int newRelY = m_Y - m_Parent->GetYPos();

		if (centerX) { newRelX = (m_Parent->GetWidth() / 2) - (GetWidth() / 2); }
		if (centerY) { newRelY = (m_Parent->GetHeight() / 2) - (GetHeight() / 2); }

		SetPositionRel(newRelX, newRelY);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIRect * GUIPanel::GetRect() {
		SetRect(&m_Rect, m_X, m_Y, m_X + m_Width, m_Y + m_Height);
		return &m_Rect;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::GetRect(int *posX, int *posY, int *width, int *height) const {
		if (posX) { *posX = m_X; }
		if (posY) { *posY = m_Y; }
		if (width) { *width = m_Width; }
		if (height) { *height = m_Height; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::Setup(GUIManager *manager, int ZPos) {
		m_Manager = manager;
		m_ZPos = ZPos;

		// Request a new ID
		m_ID = m_Manager->GetPanelID();

		// Set the manager for all the children
		int Z = 0;
		std::vector<GUIPanel *>::iterator it;
		for (it = m_Children.begin(); it != m_Children.end(); it++) {
			GUIPanel *P = *it;
			if (P) {
				Z++;
				P->Setup(manager, Z);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIPanel * GUIPanel::TopPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIPanel *CurPanel = nullptr;
		std::vector<GUIPanel *>::reverse_iterator it;
		for (it = m_Children.rbegin(); it != m_Children.rend(); it++) {
			GUIPanel *P = *it;
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

	GUIPanel * GUIPanel::BottomPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIPanel *CurPanel = nullptr;
		std::vector<GUIPanel *>::iterator it;
		for (it = m_Children.begin(); it != m_Children.end(); it++) {
			GUIPanel *P = *it;
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

	void GUIPanel::ChangeZPosition(int Type) {
		// If we don't have a parent, get the manager to alter the Z Position
		if (!m_Parent) {
			//m_Manager->ChangeZPosition(this, Type);
			return;
		}
		// Get the parent to change the position
		m_Parent->_ChangeZ(this, Type);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::AddChild(GUIPanel *child, bool convertToAbsolutePos) {
		if (child) {
			// Convert the child's coordinates into absolute coordinates
			if (convertToAbsolutePos) {
				child->m_X += m_X;
				child->m_Y += m_Y;
			}

			// Make sure the rectangle is valid
			child->m_Width = std::max(child->m_Width, 0);
			child->m_Height = std::max(child->m_Height, 0);

			int zPos = 0;
			if (!m_Children.empty()) {
				const GUIPanel *lastChild = m_Children.back();
				zPos = lastChild->GetZPos() + 1;
			}

			// Remove the child from any previous parent
			if (child->GetParentPanel()) { child->GetParentPanel()->GUIPanel::RemoveChild(child); }

			// Setup the inherited values
			child->m_Parent = this;
			child->Setup(m_Manager, zPos);

			// Add the child to the list
			m_Children.push_back(child);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::RemoveChild(const GUIPanel *pChild) {
		// Note: We do NOT free the children because they are still linked in through their controls. This merely removes the panel from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack
		for (std::vector<GUIPanel *>::iterator itr = m_Children.begin(); itr != m_Children.end(); itr++) {
			const GUIPanel *pPanel = *itr;
			if (pPanel && pPanel == pChild) {
				m_Children.erase(itr);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::CaptureMouse() {
		m_Manager->CaptureMouse(this);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::ReleaseMouse() {
		m_Manager->ReleaseMouse();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::TrackMouseHover(bool Enabled, int Delay) {
		m_Manager->TrackMouseHover(this, Enabled, Delay);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIPanel::PointInside(int X, int Y) {
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

	void GUIPanel::_ChangeZ(GUIPanel *Child, int Type) {
		GUIAssert(Child, "");

		int Index = -1;

		// Find the child in our children list
		std::vector<GUIPanel *>::iterator it;
		int Count = 0;
		for (it = m_Children.begin(); it != m_Children.end(); it++, Count++) {
			const GUIPanel *P = *it;
			if (P && P->GetPanelID() == Child->GetPanelID()) {
				Index = Count;
				break;
			}
		}
		// Didn't find the child
		if (Index == -1) {
			return;
		}
		switch (Type) {
			// Put the child at the end of the list
			case GUIPanel::ZChange::TopMost:
				m_Children.erase(m_Children.begin() + Index);
				m_Children.push_back(Child);
				break;

				// Put the child at the start of the list
			case GUIPanel::ZChange::BottomMost:
				m_Children.erase(m_Children.begin() + Index);
				m_Children.insert(m_Children.begin(), Child);
				break;
			default:
				break;
		}
		// Go through and re-order the Z positions
		Count = 0;
		for (it = m_Children.begin(); it != m_Children.end(); it++, Count++) {
			GUIPanel *P = *it;
			if (P) { P->SetZPos(Count); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPanel::Draw(GUIScreen *Screen) {
		// Calculate this panel's clipping region - set the clipping rect to be the intersection of what
		// was already set by the parent, and the dimensions of this panel.
		Screen->GetBitmap()->AddClipRect(GetRect());
		// Now save this intersection clipping rect so we can re-set it before each new child is drawn
		GUIRect thisClip;
		Screen->GetBitmap()->GetClipRect(&thisClip);

		// Draw children
		std::vector<GUIPanel *>::iterator it;
		for (it = m_Children.begin(); it != m_Children.end(); it++) {
			GUIPanel *P = *it;
			if (P->_GetVisible()) {
				// Re-set the clipping rect of this panel since the last child has messed with it
				Screen->GetBitmap()->SetClipRect(&thisClip);
				P->Draw(Screen);
			}
		}
		// Get rid of the clipping rect since the parent will re-set it if necessary
		Screen->GetBitmap()->SetClipRect(nullptr);
	}
}