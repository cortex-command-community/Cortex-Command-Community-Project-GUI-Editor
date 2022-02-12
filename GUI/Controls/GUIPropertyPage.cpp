#include "GUI.h"
#include "GUIPropertyPage.h"
#include "GUITextBox.h"

namespace RTE {

	const std::string_view GUIPropertyPage::c_ControlType = "PROPERTYPAGE";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);

		m_PageValues.ClearProperties();
		m_TextPanelList.clear();

		m_VertScroll.Create(m_Width - 12, 0, 12, m_Height);
		m_VertScroll.SetOrientation(GUIScrollPanel::Vertical);
		m_VertScroll.SetVisible(false);
		m_VertScroll.SetValue(0);
		m_VertScroll.SetSignalTarget(this);
		AddChild(&m_VertScroll);

		int spacer = 2;
		int panelHeight = 16;
		int panelCount = m_Height / panelHeight;
		for (int i = 0; i < panelCount; ++i) {
			GUITextBox *textPanel = new GUITextBox(m_OwningManager);
			textPanel->Create("", m_Width / 2, i * panelHeight + spacer, m_Width / 2, panelHeight);
			textPanel->SetVisible(false);
			textPanel->SetSignalTarget(this);
			AddChild(textPanel);
			m_TextPanelList.emplace_back(textPanel);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the control isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Create the vertical scrollbar
		m_VertScroll.Create(m_Width - 12, 0, 12, m_Height);
		m_VertScroll.SetOrientation(GUIScrollPanel::Vertical);
		m_VertScroll.SetVisible(false);
		m_VertScroll.SetValue(0);
		m_VertScroll.SetSignalTarget(this);

		AddChild(&m_VertScroll);

		// Create the text panels
		int H = 16;
		int Spacer = 0;
		int Size = m_Height / H;
		for (int i = 0; i < Size; i++) {
			GUITextBox *T = new GUITextBox(m_OwningManager);
			T->Create("", m_Width / 2, i*H + Spacer, m_Width / 2, H);
			T->SetVisible(false);
			T->SetSignalTarget(this);
			AddChild(T);

			m_TextPanelList.push_back(T);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Change the skin of the text panels
		for (GUITextBox *textPanel : m_TextPanelList) {
			textPanel->ChangeSkin(Skin);
		}

		// Build the control bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::BuildBitmap() {
		// Create a new bitmap.
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));

		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "PropertyPage", 0, 0, m_Width, m_Height);

		// Pre-cache the font
		std::string Filename;
		m_Skin->GetValue("PropertyPage", "Font", &Filename);
		m_Skin->GetValue("PropertyPage", "FontShadow", &m_FontShadow);
		m_Skin->GetValue("PropertyPage", "FontColor", &m_FontColor);
		m_Skin->GetValue("PropertyPage", "FontKerning", &m_FontKerning);

		m_FontColor = m_Skin->ConvertColor(m_FontColor, m_DrawBitmap->GetColorDepth());

		m_Font = m_Skin->GetFont(Filename);
		if (m_Font) { m_Font->CacheColor(m_FontColor); }

		m_Skin->GetValue("PropertyPage", "LineColor", &m_LineColor);
		m_LineColor = m_Skin->ConvertColor(m_LineColor, m_DrawBitmap->GetColorDepth());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::Draw(GUIScreen *Screen) {
		if (m_DrawBitmap) { m_DrawBitmap->Draw(Screen->GetBitmap(), m_X, m_Y, nullptr); }

		// Check the font first
		if (!m_Font) {
			return;
		}

		// Draw the properties
		int Count = m_PageValues.GetPropertyCount();
		int Spacer = 2;
		int Y = m_Y + Spacer;
		int Size = 16;
		std::string Name;
		std::string Value;

		for (int i = 0; i < Count; i++) {
			m_PageValues.GetProperty(i, &Name, &Value);
			m_Font->SetColor(m_FontColor);
			m_Font->SetKerning(m_FontKerning);
			m_Font->Draw(Screen->GetBitmap(), m_X + Spacer, Y, Name, m_FontShadow);

			Screen->GetBitmap()->DrawRectangle(m_X + 1, Y + Size + (m_Font->GetFontHeight() / 2 - Size / 2), m_Width - 2, 0, m_LineColor, false);
			Y += Size;
		}
		Screen->GetBitmap()->DrawRectangle(m_X + m_Width / 2, m_Y + 1, 0, Y - m_Y - Spacer * 2, m_LineColor, false);

		GUIControl::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		if (Buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			// Push the button down
			//m_Pushed = true;
			//CaptureMouse();

			//AddEvent(GUIEventType::Notification, Pushed);
		}
		SetFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::OnMouseMove(int X, int Y, int Buttons, int Modifier) {
		if (!(Buttons & GUIControl::MouseButtons::MOUSE_LEFT) || !IsCaptured()) {
			return;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUIControl::SetSize(Width, Height);

		// TODO: Alter text panels

		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::SetPropertyValues(GUIProperties *Props) {
		m_PageValues.ClearProperties();
		m_PageValues.OverwriteProperties(Props, true);

		// Update the text panels
		for (int i = 0; i < m_TextPanelList.size(); i++) {
			GUITextBox *T = m_TextPanelList.at(i);
			T->SetVisible(false);
			T->SetText("");

			if (i < m_PageValues.GetPropertyCount()) {
				T->SetVisible(true);
				std::string Name;
				std::string Value;
				if (m_PageValues.GetProperty(i, &Name, &Value)) { T->SetText(Value); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) {
		GUIAssert(Source, "");

		bool TextSignal = false;

		// Is this a text panel?
		std::vector<GUITextBox *>::iterator it;
		for (it = m_TextPanelList.begin(); it != m_TextPanelList.end(); it++) {
			const GUITextBox *T = *it;

			if (Source->GetUniqueID() == T->GetUniqueID()) {
				TextSignal = true;

				// Change event. Do not update properties
				if (Code == GUIEventCode::Changed) {
					AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
					return;
				}
				break;
			}
		}

		// Update the properties.
		// If any of the values are different, fire a 'changed' notification event
		if (TextSignal && InvokeUpdate()) {
			// Fire the enter event
			AddEvent(GUIEventType::Notification, GUIEventCode::Enter);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIPropertyPage::InvokeUpdate() {
		bool Changed = false;

		for (int i = 0; i < m_TextPanelList.size(); i++) {
			const GUITextBox *T = m_TextPanelList.at(i);

			if (i < m_PageValues.GetPropertyCount()) {
				std::string Name;
				std::string Value;
				if (m_PageValues.GetProperty(i, &Name, &Value)) {
					if (T->GetText().compare(Value) != 0) { Changed = true; }
					// Set the value
					m_PageValues.SetProperty(i, Name, T->GetText());
				}
			}
		}

		return Changed;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIPropertyPage::ClearValues() {
		m_PageValues.ClearProperties();

		// Hide the text panels
		std::vector<GUITextBox *>::iterator it;
		for (it = m_TextPanelList.begin(); it != m_TextPanelList.end(); it++) {
			GUITextBox *T = *it;
			T->SetVisible(false);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIPropertyPage::HasTextFocus() {
		std::vector<GUITextBox *>::iterator it;
		for (it = m_TextPanelList.begin(); it != m_TextPanelList.end(); it++) {
			const GUITextBox *T = *it;

			// Visible & has focus??
			if (T->GetVisible() && T->HasFocus()) {
				return true;
			}
		}

		return false;
	}
}