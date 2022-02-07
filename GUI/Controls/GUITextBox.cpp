#include "GUI.h"
#include "GUITextBox.h"

namespace RTE {

	const std::string_view GUITextBox::c_ControlType = "TEXTBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Create(GUIProperties *reference) {
		GUIControl::Create(reference);

		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		GUITextPanel::Create(m_X, m_Y, m_Width, m_Height);

		std::string alignString;
		reference->GetPropertyValue("HAlignment", &alignString);
		if (alignString == "left") {
			m_HAlignment = GUIFont::HAlignment::Left;
		} else if (alignString == "centre" || alignString == "center") {
			m_HAlignment = GUIFont::HAlignment::Centre;
		} else if (alignString == "right") {
			m_HAlignment = GUIFont::HAlignment::Right;
		}

		reference->GetPropertyValue("VAlignment", &alignString);
		if (alignString == "top") {
			m_VAlignment = GUIFont::VAlignment::Top;
		} else if (alignString == "middle") {
			m_VAlignment = GUIFont::VAlignment::Middle;
		} else if (alignString == "bottom") {
			m_VAlignment = GUIFont::VAlignment::Bottom;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUITextBox::~GUITextBox() {
		// Destroy the draw bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Free any old bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}

		// Create a new bitmap
		m_DrawBitmap = m_Skin->CreateBitmap(m_Width, m_Height);

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap, "TextBox", 0, 0, m_Width, m_Height);

		// Setup the skin in the panel too
		GUITextPanel::ChangeSkin(Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Draw(GUIScreen *Screen) {
		// Draw the background
		m_DrawBitmap->Draw(Screen->GetBitmap(), m_X, m_Y, nullptr);

		GUITextPanel::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::StoreProperties() {
		m_Properties.AddProperty("NumericOnly", GetNumericOnly());
		m_Properties.AddProperty("MaxNumericValue", GetMaxNumericValue());
		m_Properties.AddProperty("MaxTextLength", GetMaxTextLength());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUITextPanel::SetSize(Width, Height);

		// Force a rebuild
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) {
		// Clicked
		if (Code == GUIEventCode::Clicked) { AddEvent(GUIEventType::Notification, GUIEventCode::Clicked, Data); }

		// Changed
		if (Code == GUIEventCode::Changed) { AddEvent(GUIEventType::Notification, GUIEventCode::Changed); }

		// Enter
		if (Code == GUIEventCode::Enter) { AddEvent(GUIEventType::Notification, GUIEventCode::Enter); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		bool numericOnly = GetNumericOnly();
		m_Properties.GetPropertyValue("NumericOnly", &numericOnly);
		SetNumericOnly(numericOnly);

		int maxNumericValue = GetMaxNumericValue();
		m_Properties.GetPropertyValue("MaxNumericValue", &maxNumericValue);
		SetMaxNumericValue(maxNumericValue);

		int maxTextLength = GetMaxTextLength();
		m_Properties.GetPropertyValue("MaxTextLength", &maxTextLength);
		SetMaxTextLength(maxTextLength);

		// Force a rebuild of the bitmap
		ChangeSkin(m_Skin);
	}
}