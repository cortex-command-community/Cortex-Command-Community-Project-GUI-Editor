#include "GUI.h"
#include "GUIWriter.h"
#include "GUIControlBase.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::Create(const std::string &name, int posX, int posY, int width, int height) {
		m_ChildControls.clear();
		m_Properties.ClearProperties();

		m_Properties.AddProperty("Name", name);
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");

		m_X = posX;
		m_Y = posY;
		m_Width = width;
		m_Height = height;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::Create(GUIProperties *reference) {
		m_Properties.AddProperty("Name", "");
		m_Properties.AddProperty("Anchor", "Left, Top");
		m_Properties.AddProperty("ToolTip", "");

		reference->GetPropertyValue("Visible", &m_Visible);
		reference->GetPropertyValue("Enabled", &m_Enabled);

		reference->GetPropertyValue("X", &m_X);
		reference->GetPropertyValue("Y", &m_Y);
		reference->GetPropertyValue("Width", &m_Width);
		reference->GetPropertyValue("Height", &m_Height);

		reference->GetPropertyValue("Visible", &m_Visible);
		reference->GetPropertyValue("Enabled", &m_Enabled);

		m_Properties.OverwriteProperties(reference);
	}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::CenterInParent(bool centerX, bool centerY) {
		int newRelX = m_X - m_ParentControl->GetPosX();
		int newRelY = m_Y - m_ParentControl->GetPosY();

		if (centerX) { newRelX = (m_ParentControl->GetWidth() / 2) - (GetWidth() / 2); }
		if (centerY) { newRelY = (m_ParentControl->GetHeight() / 2) - (GetHeight() / 2); }

		SetPositionRel(newRelX, newRelY);
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::SetPositionAbs(int X, int Y, bool moveChildren) {
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;

		if (moveChildren) {
			for (GUIControlBase *childControl : m_ChildControls) {
				childControl->SetPositionAbs(childControl->m_X + DX, childControl->m_Y + DY);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::SetPositionRel(int X, int Y) {
		X += m_ParentControl->GetPosX();
		Y += m_ParentControl->GetPosY();
		int DX = X - m_X;
		int DY = Y - m_Y;
		m_X = X;
		m_Y = Y;

		for (GUIControlBase *childControl : m_ChildControls) {
			childControl->SetPositionAbs(childControl->m_X + DX, childControl->m_Y + DY);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::MoveRelative(int dX, int dY) {
		m_X += dX;
		m_Y += dY;

		for (GUIControlBase *childControl : m_ChildControls) {
			childControl->SetPositionAbs(childControl->m_X + dX, childControl->m_Y + dY);
		}
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIControlBase::GetName() const {
		std::string name;
		m_Properties.GetPropertyValue("Name", &name);
		return name;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIControlBase::GetAnchor() {
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

	std::string GUIControlBase::GetToolTip() const {
		std::string tip;
		m_Properties.GetPropertyValue("ToolTip", &tip);
		return tip;
	}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::SetFocus() {
		m_OwningManager->SetFocus(this);
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData) {
		m_OwningManager->AddEvent(new GUIEvent(this, eventType, eventCode, eventData));
	}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIRect * GUIControlBase::GetRect() {
		SetRect(&m_Rect, m_X, m_Y, m_X + m_Width, m_Y + m_Height);
		return &m_Rect;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::GetRect(int *posX, int *posY, int *width, int *height) const {
		if (posX) { *posX = m_X; }
		if (posY) { *posY = m_Y; }
		if (width) { *width = m_Width; }
		if (height) { *height = m_Height; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::GetControlRect(int *posX, int *posY, int *width, int *height) const {
		// Zero the values for controls that don't override this
		if (posX) { *posX = 0; }
		if (posY) { *posY = 0; }
		if (width) { *width = 0; }
		if (height) { *height = 0; }
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::CaptureMouse() {
		m_OwningManager->CaptureMouse(this);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::ReleaseMouse() {
		m_OwningManager->ReleaseMouse();
	}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::AddChild(GUIControlBase *child, bool convertToAbsolutePos) {
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
			if (!m_ChildControls.empty()) {
				const GUIControlBase *lastChild = m_ChildControls.back();
				zPos = lastChild->GetZPos() + 1;
			}

			// Remove the child from any previous parent
			if (child->m_ParentControl) { child->m_ParentControl->RemoveChild(child->GetName()); }

			// Setup the inherited values
			child->m_ParentControl = this;
			child->Setup(m_OwningManager, zPos);

			// Add the child to the list
			m_ChildControls.push_back(child);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::RemoveChild(const std::string_view &name) {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (std::vector<GUIControlBase *>::iterator it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			const GUIControlBase *C = *it;
			if (C && C->GetName().compare(name) == 0) {
				m_ChildControls.erase(it);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::RemoveAllChildren() {
		// Note: We do NOT free the children because they are still linked in through their panels. This merely removes the control from the list.
		// This will cause a small memory leak, but this is only designed for the GUI Editor and is a bit of a hack.
		for (const GUIControlBase *control : m_ChildControls) {
			if (control) { m_OwningManager->RemoveControl(control->GetName(), false); }
		}
		m_ChildControls.clear();
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::ChangeZPosition(int changeType) {
		if (!m_ParentControl) {
			return;
		}
		int Index = -1;

		// Find the child in our children list
		std::vector<GUIControlBase *>::iterator it;
		int Count = 0;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++, Count++) {
			if (GetUniqueID() == this->GetUniqueID()) {
				Index = Count;
				break;
			}
		}
		// Didn't find the child
		if (Index == -1) {
			return;
		}
		switch (changeType) {
			// Put the child at the end of the list
			case GUIControlBase::ZChange::TopMost:
				m_ChildControls.erase(m_ChildControls.begin() + Index);
				m_ChildControls.push_back(this);
				break;

				// Put the child at the start of the list
			case GUIControlBase::ZChange::BottomMost:
				m_ChildControls.erase(m_ChildControls.begin() + Index);
				m_ChildControls.insert(m_ChildControls.begin(), this);
				break;
			default:
				break;
		}
		// Go through and re-order the Z positions
		Count = 0;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++, Count++) {
			GUIControlBase *P = *it;
			if (P) { P->SetZPos(Count); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::TrackMouseHover(bool Enabled, int Delay) {
		m_OwningManager->TrackMouseHover(this, Enabled, Delay);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlBase::PointInside(int X, int Y) {
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

	GUIControlBase * GUIControlBase::TopPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIControlBase *CurPanel = nullptr;
		std::vector<GUIControlBase *>::reverse_iterator it;
		for (it = m_ChildControls.rbegin(); it != m_ChildControls.rend(); it++) {
			GUIControlBase *P = *it;
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

	GUIControlBase * GUIControlBase::BottomPanelUnderPoint(int x, int y) {
		if (!PointInside(x, y)) {
			return nullptr;
		}
		// If this panel is invisible or disabled, this panel is ignored
		if (!m_Visible || !m_Enabled) {
			return nullptr;
		}

		// Go through the children
		GUIControlBase *CurPanel = nullptr;
		;
		for (std::vector<GUIControlBase *>::iterator it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControlBase *P = *it;
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

	void GUIControlBase::BuildProperties(GUIProperties *properties) const {
		GUIAssert(properties, "");

		// Subtract the position from the parent
		int X = m_X;
		int Y = m_Y;

		if (m_ParentControl) {
			X -= m_ParentControl->m_X;
			Y -= m_ParentControl->m_Y;
		}

		properties->AddProperty("X", X);
		properties->AddProperty("Y", Y);
		properties->AddProperty("Width", m_Width);
		properties->AddProperty("Height", m_Height);
		properties->AddProperty("Visible", m_Visible);
		properties->AddProperty("Enabled", m_Enabled);
	}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIControlBase::ToString() const {
		std::string OutString = "";

		// Subtract the position from the parent
		int X = m_X;
		int Y = m_Y;

		if (m_ParentControl) {
			X -= m_ParentControl->m_X;
			Y -= m_ParentControl->m_Y;
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

	void GUIControlBase::Setup(GUIControlManager *manager, int ZPos) {
		m_OwningManager = manager;
		m_ZPos = ZPos;

		// Request a new ID
		m_UniqueID = m_OwningManager->GetUniqueID();

		// Set the manager for all the children
		int Z = 0;
		std::vector<GUIControlBase *>::iterator it;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControlBase *P = *it;
			if (P) {
				Z++;
				P->Setup(manager, Z);
			}
		}
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::ApplyProperties(GUIProperties *properties) {
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
		//GUIControlBase *panel = GetPanel();
		//if (panel && panel->GetParentPanel()) {
		//	int px;
		//	int py;
		//	int pw;
		//	int ph;
		//	panel->GetParentPanel()->GetRect(&px, &py, &pw, &ph);
		//	X += px;
		//	Y += py;
		//}
		SetEnabled(Enabled);

		Move(X, Y);
		Resize(Width, Height);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlBase::Save(GUIWriter *writer) {
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
		if (m_ParentControl) {
			OutString += m_ParentControl->GetName();
		} else {
			OutString += "None";
		}
		OutString += "\n";

		// Get the main panel and write its location
		//GUIControlBase *Pan = GetPanel();
		//if (Pan) { OutString.append(Pan->ToString()); }

		// Write out the properties
		OutString.append(m_Properties.ToString());

		// Write to the writer class
		*writer << OutString;

		return true;
	}











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlBase::Draw(GUIScreen *targetScreen) {
		GUIRect *clippingRect = GetRect();
		targetScreen->GetBitmap()->AddClipRect(clippingRect);

		for (GUIControlBase *childControl : m_ChildControls) {
			if (childControl->GetVisible()) {
				// Re-set the clipping rect since the last child has messed with it.
				targetScreen->GetBitmap()->SetClipRect(clippingRect);
				childControl->Draw(targetScreen);
			}
		}
		targetScreen->GetBitmap()->SetClipRect(nullptr);
	}
}