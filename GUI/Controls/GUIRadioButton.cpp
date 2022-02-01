#include "GUI.h"
#include "GUIRadioButton.h"

namespace RTE {

	const std::string_view GUIRadioButton::c_ControlType = "RADIOBUTTON";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIRadioButton::GUIRadioButton(GUIControlManager *ControlManager) : GUIControlBase() {
		m_Image = nullptr;
		m_OwningManager = ControlManager;
		m_Checked = false;
		m_Font = nullptr;
		m_Mouseover = false;
		m_FontColor = 0;
		m_Text = "";
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControlBase::Create(Name, X, Y, Width, Height);

		// Minimum size of the control
		m_MinWidth = 40;
		m_MinHeight = 10;

		// Default size of the control
		m_DefWidth = 60;
		m_DefHeight = 16;

		// Setup the panel
		m_X = X;
		m_Y = Y;
		m_Width = m_DefWidth;
		m_Height = m_DefHeight;

		if (Width != -1) { m_Width = Width; }
		if (Height != -1) { m_Height = Height; }

		// Make sure the button isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Create(GUIProperties *Props) {
		GUIControlBase::Create(Props);

		// Minimum size of the control
		m_MinWidth = 40;
		m_MinHeight = 10;

		// Default size of the control
		m_DefWidth = 60;
		m_DefHeight = 16;

		// Setup the panel
		//GUIControlBase::LoadProperties(Props);

		// Make sure the button isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Get the values
		Props->GetPropertyValue("Text", &m_Text);
		Props->GetPropertyValue("Checked", &m_Checked);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::ChangeSkin(GUISkin *Skin) {
		GUIControlBase::ChangeSkin(Skin);

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
		// Setup the clipping
		Screen->GetBitmap()->SetClipRect(GetRect());

		// Calculate the y position of the base
		// Make it centered vertically
		int YPos = m_Height / 2 - (m_ImageRects[0].bottom - m_ImageRects[0].top) / 2 + m_Y;

		// Draw the base
		if (m_Mouseover || m_GotFocus) {
			m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[1]);
		} else {
			m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[0]);
		}
		// Draw the check
		if (m_Checked) {
			if (m_Enabled) {
				m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[2]);
			} //else {
				//m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[3]);
			//}
		}

		// Should show as grayed out and disabled when it is, regardless of checked or not
		if (!m_Enabled) { m_Image->DrawTrans(Screen->GetBitmap(), m_X, YPos, &m_ImageRects[3]); }

		// Draw the text

		// Add a space to make a gap between checkbox & text
		std::string Text;
		std::string space = " ";
		Text = space.append(m_Text);

		if (m_Font) {
			m_Font->SetColor(m_FontColor);
			m_Font->SetKerning(m_FontKerning);
			m_Font->Draw(Screen->GetBitmap(), m_X + (m_ImageRects[0].right - m_ImageRects[0].left), m_Y + (m_Height / 2) - (m_Font->GetFontHeight() / 2) - 1, Text, m_FontShadow);
		}
		Screen->GetBitmap()->SetClipRect(nullptr);

		GUIControlBase::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		if (Buttons & GUIControlBase::MouseButtons::MOUSE_LEFT) {
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
		if (PointInside(X, Y) && Buttons & GUIControlBase::MouseButtons::MOUSE_LEFT) { SetCheck(true); }

		AddEvent(GUIEventType::Notification, GUIEventCode::UnPushed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIRadioButton::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUIControlBase::SetSize(Width, Height);

		// Rebuild the bitmap
		BuildBitmap();
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
			std::vector<GUIControlBase *>::iterator it;
			std::vector<GUIControlBase *> *Children = m_ParentControl->GetChildren();

			for (it = Children->begin(); it != Children->end(); it++) {
				GUIControlBase *C = *it;
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
		GUIControlBase::ApplyProperties(Props);

		m_Properties.GetPropertyValue("Text", &m_Text);
		m_Properties.GetPropertyValue("Checked", &m_Checked);
	}
}