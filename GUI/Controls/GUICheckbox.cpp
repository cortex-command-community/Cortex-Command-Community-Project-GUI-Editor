#include "GUI.h"
#include "GUICheckbox.h"

namespace RTE {

	const std::string_view GUICheckbox::c_ControlType = "CHECKBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the button isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);


		// Grab the check value
		m_Check = State::Unchecked;
		std::string value;
		Props->GetPropertyValue("Checked", &value);
		if (value == "Checked") {
			m_Check = State::Checked;
		} else if (value == "Greycheck") {
			m_Check = State::Greycheck;
		}
		Props->GetPropertyValue("Text", &m_Text);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the checkbox bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::BuildBitmap() {
		std::string Filename;
		int ColorIndex = 0;
		int Values[4];

		// Load the image
		m_Skin->GetValue("Checkbox", "Filename", &Filename);
		m_Image = m_Skin->CreateBitmap(Filename);
		if (!m_Image) {
			return;
		}
		// Load the font
		m_Skin->GetValue("Checkbox", "Font", &Filename);
		m_Font = m_Skin->GetFont(Filename);
		m_Skin->GetValue("Checkbox", "FontColor", &m_FontColor);
		m_Skin->GetValue("Checkbox", "FontShadow", &m_FontShadow);
		m_Skin->GetValue("Checkbox", "FontKerning", &m_FontKerning);
		m_FontColor = m_Skin->ConvertColor(m_FontColor, m_Image->GetColorDepth());
		m_Font->CacheColor(m_FontColor);

		// Set the color key
		m_Skin->GetValue("Checkbox", "ColorKeyIndex", &ColorIndex);
		ColorIndex = m_Skin->ConvertColor(ColorIndex, m_Image->GetColorDepth());
		m_Image->SetColorKey(ColorIndex);

		// Load the source image rectangles

		// Base checkbox
		m_Skin->GetValue("Checkbox", "Base", Values, 4);
		SetRect(&m_ImageRects[0], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Mouse over checkbox
		m_Skin->GetValue("Checkbox", "MouseOver", Values, 4);
		SetRect(&m_ImageRects[1], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Normal check
		m_Skin->GetValue("Checkbox", "Check", Values, 4);
		SetRect(&m_ImageRects[2], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Grayed check
		m_Skin->GetValue("Checkbox", "GreyCheck", Values, 4);
		SetRect(&m_ImageRects[3], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::Draw(GUIScreen *Screen) {
		if (!m_Image) {
			return;
		}

		// Calculate the y position of the base
		// Make it centered vertically
		int YPos = m_Height / 2 - (m_ImageRects[0].bottom - m_ImageRects[0].top) / 2 + m_Y;

		// Draw the base
		if (m_Mouseover) {
			m_Image->Draw(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[1]);
		} else {
			m_Image->Draw(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[0]);
		}

		// Draw the check
		switch (m_Check) {
			case Checked:
				m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[2]);
				break;
			case Greycheck:
				m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[3]);
				break;
			default:
				break;
		}

		// Draw the text
		std::string Text;
		std::string space = " ";
		Text = space.append(m_Text);

		if (m_Font) {
			m_Font->SetColor(m_FontColor);
			m_Font->SetKerning(m_FontKerning);
			m_Font->Draw(Screen->GetBitmap(), m_X + (m_ImageRects[0].right - m_ImageRects[0].left) + 2, m_Y + (m_Height / 2) - (m_Font->GetFontHeight() / 2) - 1, Text, m_FontShadow);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		if (Buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			// Push the checkbox down
			CaptureMouse();
			SetFocus();

			AddEvent(GUIEventType::Notification, GUIEventCode::Pushed);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();

		// If the mouse is over the button, add the command to the event queue
		if (PointInside(X, Y) && Buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			if (m_Check == State::Unchecked) {
				m_Check = State::Checked;
			} else {
				m_Check = State::Unchecked;
			}
			AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
		}

		AddEvent(GUIEventType::Notification, GUIEventCode::UnPushed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		SetSize(Width, Height);

		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::StoreProperties() {
		if (m_Check == State::Unchecked) {
			m_Properties.AddProperty("Checked", "Unchecked");
		} else if (m_Check == State::Checked) {
			m_Properties.AddProperty("Checked", "Checked");
		} else if (m_Check == State::Greycheck) {
			m_Properties.AddProperty("Checked", "Greycheck");
		}
		m_Properties.AddProperty("Text", m_Text);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICheckbox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		m_Check = Unchecked;
		std::string value;
		m_Properties.GetPropertyValue("Checked", &value);
		if (value == "Checked") {
			m_Check = State::Checked;
		} else if (value == "Greycheck") {
			m_Check = State::Greycheck;
		}
		m_Properties.GetPropertyValue("Text", &m_Text);
	}
}