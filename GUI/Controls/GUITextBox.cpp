#include "GUI.h"
#include "GUITextBox.h"

namespace RTE {

	const std::string_view GUITextBox::c_ControlType = "TEXTBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControl::Create(Name, X, Y, Width, Height);

		// Setup the panel
		m_X = X;
		m_Y = Y;
		m_Width = m_DefaultWidth;
		m_Height = m_DefaultHeight;

		// Create the ListPanel
		int w = m_DefaultWidth;
		int h = m_DefaultHeight;
		if (Width != -1) { w = Width; }
		if (Height != -1) { h = Height; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);
		GUITextPanel::Create(m_X, m_Y, m_Width, m_Height);

		// Make sure the textbox isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Alignment values - these don't affect anything as of yet
		std::string alignString;
		Props->GetPropertyValue("HAlignment", &alignString);
		if (stricmp(alignString.c_str(), "left") == 0) { m_HAlignment = GUIFont::HAlignment::Left; }
		if (stricmp(alignString.c_str(), "centre") == 0 || stricmp(alignString.c_str(), "center") == 0) { m_HAlignment = GUIFont::HAlignment::Centre; }
		if (stricmp(alignString.c_str(), "right") == 0) { m_HAlignment = GUIFont::HAlignment::Right; }

		Props->GetPropertyValue("VAlignment", &alignString);
		if (stricmp(alignString.c_str(), "top") == 0) { m_VAlignment = GUIFont::VAlignment::Top; }
		if (stricmp(alignString.c_str(), "middle") == 0) { m_VAlignment = GUIFont::VAlignment::Middle; }
		if (stricmp(alignString.c_str(), "bottom") == 0) { m_VAlignment = GUIFont::VAlignment::Bottom; }
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