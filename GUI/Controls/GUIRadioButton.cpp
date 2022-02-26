#include "GUI.h"
#include "GUIRadioButton.h"

namespace RTE {

	const std::string_view GUIRadioButton::c_ControlType = "RADIOBUTTON";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the button isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Get the values
		Props->GetPropertyValue("Text", &m_Text);
		Props->GetPropertyValue("Checked", &m_Checked);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the checkbox bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::BuildBitmap() {
		std::string Filename;
		int ColorIndex = 0;
		int Values[4];

		// Load the image
		m_Skin->GetValue("RadioButton", "Filename", &Filename);
		m_Image = m_Skin->CreateBitmap(Filename);
		if (!m_Image) {
			return;
		}
		// Load the font
		m_Skin->GetValue("RadioButton", "Font", &Filename);
		m_Font = m_Skin->GetFont(Filename);

		m_Skin->GetValue("RadioButton", "FontColor", &m_FontColor);
		m_Skin->GetValue("RadioButton", "FontShadow", &m_FontShadow);
		m_Skin->GetValue("RadioButton", "FontKerning", &m_FontKerning);
		m_FontColor = m_Skin->ConvertColor(m_FontColor, m_Image->GetColorDepth());
		m_Font->CacheColor(m_FontColor);

		// Set the color key
		m_Skin->GetValue("RadioButton", "ColorKeyIndex", &ColorIndex);
		ColorIndex = m_Skin->ConvertColor(ColorIndex, m_Image->GetColorDepth());
		m_Image->SetColorKey(ColorIndex);

		// Load the source image rectangles

		// Base checkbox
		m_Skin->GetValue("RadioButton", "Base", Values, 4);
		SetRect(&m_ImageRects[0], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Mouse over checkbox
		m_Skin->GetValue("RadioButton", "MouseOver", Values, 4);
		SetRect(&m_ImageRects[1], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Normal check
		m_Skin->GetValue("RadioButton", "Check", Values, 4);
		SetRect(&m_ImageRects[2], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Grayed check (for disabled mode)
		m_Skin->GetValue("RadioButton", "GreyCheck", Values, 4);
		SetRect(&m_ImageRects[3], Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Draw(GUIScreen *Screen) {
		if (!m_Image) {
			return;
		}

		// Calculate the y position of the base
		// Make it centered vertically
		int YPos = m_Height / 2 - (m_ImageRects[0].bottom - m_ImageRects[0].top) / 2 + m_PosY;

		// Draw the base
		if (m_Mouseover || m_GotFocus) {
			m_Image->DrawTrans(Screen->GetBitmap(), m_PosX, YPos, &m_ImageRects[1]);
		} else {
			m_Image->DrawTrans(Screen->GetBitmap(), m_PosX, YPos, &m_ImageRects[0]);
		}
		// Draw the check
		if (m_Checked) {
			if (m_Enabled) {
				m_Image->DrawTrans(Screen->GetBitmap(), m_PosX, YPos, &m_ImageRects[2]);
			} //else {
				//m_Image->DrawTrans(Screen->GetBitmap(), m_PosX, YPos, &m_ImageRects[3]);
			//}
		}

		// Should show as grayed out and disabled when it is, regardless of checked or not
		if (!m_Enabled) { m_Image->DrawTrans(Screen->GetBitmap(), m_PosX, YPos, &m_ImageRects[3]); }

		// Draw the text

		// Add a space to make a gap between checkbox & text
		std::string Text;
		std::string space = " ";
		Text = space.append(m_Text);

		if (m_Font) {
			m_Font->SetColor(m_FontColor);
			m_Font->SetKerning(m_FontKerning);
			m_Font->Draw(Screen->GetBitmap(), m_PosX + (m_ImageRects[0].right - m_ImageRects[0].left), m_PosY + (m_Height / 2) - (m_Font->GetFontHeight() / 2) - 1, Text, m_FontShadow);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		if (Buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			// Push the checkbox down
			CaptureMouse();
			SetFocus();

			AddEvent(GUIEventType::Notification, GUIEventCode::Pushed);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();

		// If the mouse is over the button, add the command to the event queue
		if (PointInside(X, Y) && Buttons & GUIControl::MouseButtons::MOUSE_LEFT) { SetCheck(true); }

		AddEvent(GUIEventType::Notification, GUIEventCode::UnPushed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Resize(int newWidth, int newHeight) {
		if (m_Width != newWidth || m_Height != newHeight) {
			GUIControl::Resize(std::max(newWidth, m_MinWidth), std::max(newHeight, m_MinHeight));
			BuildBitmap();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::StoreProperties() {
		m_Properties.AddProperty("Text", m_Text);
		m_Properties.AddProperty("Checked", m_Checked);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::SetCheck(bool Check) {
		// Nothing to do if already in the same state
		if (m_Checked == Check) {
			return;
		}

		m_Checked = Check;

		AddEvent(GUIEventType::Notification, GUIEventCode::Changed, Check);

		// Don't worry if we are not checked
		if (!m_Checked) {
			return;
		}

		// Go through all my RadioButton siblings and un-check them
		if (m_ParentControl) {
			std::deque<GUIControl *>::iterator it;
			std::deque<GUIControl *> *Children = m_ParentControl->GetChildren();

			for (it = Children->begin(); it != Children->end(); it++) {
				GUIControl *C = *it;
				if (C) {
					// Make sure this is not me
					//if (C->GetPanel() && GetPanel() && C->GetPanel()->GetUniqueID() == GetPanel()->GetUniqueID()) {
					//	continue;
					//}

					// Make sure the control is a radio button
					if (C->GetControlType().compare(GetControlType()) == 0) {
						GUIRadioButton *R = (GUIRadioButton *)C;
						R->SetCheck(false);
					}
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		m_Properties.GetPropertyValue("Text", &m_Text);
		m_Properties.GetPropertyValue("Checked", &m_Checked);
	}
}