#include "GUI.h"
#include "GUIScrollbar.h"

namespace RTE {

	const std::string_view GUIScrollbar::c_ControlType = "SCROLLBAR";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the scrollpanel isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		GUIScrollPanel::Create(m_X, m_Y, m_Width, m_Height);
		GUIScrollPanel::LoadProps(Props);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) {
		GUIAssert(Source, "");

		// Should be our scroll panel
		if (Source->GetUniqueID() == GetUniqueID() && Code == GUIEventCode::ChangeValue) {
			AddEvent(GUIEventType::Notification, GUIEventCode::ChangeValue);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		// Capture the mouse
		CaptureMouse();

		GUIScrollPanel::OnMouseDown(X, Y, Buttons, Modifier);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		// Release the mouse
		ReleaseMouse();

		GUIScrollPanel::OnMouseUp(X, Y, Buttons, Modifier);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) {
		int oldValue = GetValue();
		int newValue = std::clamp(oldValue - (GetValueResolution() * ((mouseWheelChange > 0) ? 1 : -1)), GetMinimum(), GetMaximum() - GetPageSize());
		if (newValue != oldValue) {
			SetValue(newValue);
			AddEvent(GUIEventType::Notification, GUIEventCode::ChangeValue);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUIScrollPanel::SetSize(Width, Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIScrollbar::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		GUIScrollPanel::LoadProps(&m_Properties);

		// Rebuild the bitmap
		BuildBitmap(true, true);
	}
}