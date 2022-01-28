#include "GUI.h"
#include "GUIListBox.h"

namespace RTE {

	const std::string_view GUIListBox::c_ControlType = "LISTBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIListBox::GUIListBox(GUIManager *Manager, GUIControlManager *ControlManager) : GUIControl(), GUIListPanel(Manager) {
		m_ControlManager = ControlManager;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControl::Create(Name, X, Y, Width, Height);

		// Minimum size of the control
		m_MinWidth = 50;
		m_MinHeight = 12;

		// Default size of the control
		m_DefWidth = 100;
		m_DefHeight = 100;

		// Create the ListPanel
		int w = m_DefWidth;
		int h = m_DefHeight;
		if (Width != -1) { w = Width; }
		if (Height != -1) { h = Height; }

		// Make sure the control isn't too small
		w = std::max(w, m_MinWidth);
		h = std::max(h, m_MinHeight);

		GUIListPanel::Create(X, Y, w, h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Minimum size of the control
		m_MinWidth = 100;
		m_MinHeight = 12;

		// Default size of the control
		m_DefWidth = 100;
		m_DefHeight = 100;

		// Setup the panel
		GUIPanel::LoadProperties(Props);

		// Make sure the listbox isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		GUIListPanel::Create(m_X, m_Y, m_Width, m_Height);

		// Get the properties
		bool Multi = false;
		Props->GetPropertyValue("MultiSelect", &Multi);
		SetMultiSelect(Multi);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::Resize(int Width, int Height) {
		// Make sure the listbox isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		SetSize(Width, Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::StoreProperties() {
		m_Properties.AddProperty("MultiSelect", GetMultiSelect());
		m_Properties.AddProperty("ScrollBarThickness", GetScrollBarThickness());
		m_Properties.AddProperty("ScrollBarPadding", GetScrollBarPadding());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::ReceiveSignal(GUIPanel *Source, GUIEventCode Code, int Data) {
		if (Source->GetPanelID() == GetPanelID()) {
			if (Code == GUIEventCode::MouseMove) {
				AddEvent(GUIEventType::Notification, GUIEventCode::MouseMove, Data);
			} else if (Code == GUIEventCode::MouseEnter) {
				AddEvent(GUIEventType::Notification, GUIEventCode::MouseEnter, Data);
			} else if (Code == GUIEventCode::MouseLeave) {
				AddEvent(GUIEventType::Notification, GUIEventCode::MouseLeave, Data);
			} else if (Code == GUIEventCode::Select) {
				AddEvent(GUIEventType::Notification, GUIEventCode::Select, 0);
			} else if (Code == GUIEventCode::MouseDown) {
				AddEvent(GUIEventType::Notification, GUIEventCode::MouseDown, 0);
			} else if (Code == GUIEventCode::DoubleClick) {
				AddEvent(GUIEventType::Notification, GUIEventCode::DoubleClick, 0);
			} else if (Code == GUIEventCode::KeyDown) {
				AddEvent(GUIEventType::Notification, GUIEventCode::KeyDown, Data);
			} else if (Code == GUIEventCode::EdgeHit) {
				AddEvent(GUIEventType::Notification, GUIEventCode::EdgeHit, Data);
			}
		}

		GUIListPanel::ReceiveSignal(Source, Code, Data);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListBox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		bool Multi = false;
		m_Properties.GetPropertyValue("MultiSelect", &Multi);
		SetMultiSelect(Multi);

		int scrollBarThickness = GetScrollBarThickness();
		m_Properties.GetPropertyValue("ScrollBarThickness", &scrollBarThickness);
		SetScrollBarThickness(scrollBarThickness);

		int scrollBarPadding = GetScrollBarPadding();
		m_Properties.GetPropertyValue("ScrollBarPadding", &scrollBarPadding);
		SetScrollBarPadding(scrollBarPadding);

		// Rebuild the bitmap
		BuildBitmap(true, true);
	}
}