#include "GUI.h"
#include "GUICollectionBox.h"

namespace RTE {

	const std::string_view GUICollectionBox::c_ControlType = "COLLECTIONBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the box isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Get the values
		Props->GetPropertyValue("DrawBackground", &m_DrawBackground);
		std::string value;
		Props->GetPropertyValue("DrawType", &value);
		if (value == "Color") {
			m_DrawType = DrawType::Color;
		} else if (value == "Image") {
			m_DrawType = DrawType::Image;
		} else if (value == "Panel") {
			m_DrawType = DrawType::Panel;
		}
		Props->GetPropertyValue("DrawColor", &m_DrawColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUICollectionBox::~GUICollectionBox() {
		if (m_Background) {
			m_Background->Destroy();
			delete m_Background;
			m_Background = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the panel bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::BuildBitmap() {
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));

		// Create the button image
		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "CollectionBox_Panel", 0, 0, m_Width, m_Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Draw(GUIScreen *Screen) {
		if (m_DrawBackground) {
			if (m_DrawType == DrawType::Color) {
				Screen->GetBitmap()->DrawRectangle(m_X, m_Y, m_Width, m_Height, m_Skin->ConvertColor(m_DrawColor, Screen->GetBitmap()->GetColorDepth()), true);
			} else if (m_DrawType == DrawType::Image) {
				if (m_DrawBitmap && m_DrawBackground) {
					// Setup the clipping
					Screen->GetBitmap()->SetClipRect(GetRect());

					// Draw the image
					m_DrawBitmap->DrawTrans(Screen->GetBitmap(), m_X, m_Y, nullptr);

					// Get rid of clipping
					Screen->GetBitmap()->SetClipRect(nullptr);
				}
			} else if (m_DrawType == DrawType::Panel && m_DrawBackground) {
				if (m_DrawBitmap) {
					GUIRect Rect;
					SetRect(&Rect, 0, 0, m_Width, m_Height);
					Screen->DrawBitmapTrans(m_DrawBitmap.get(), m_X, m_Y, &Rect);
				}
			}
		}
		GUIControl::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();

		AddEvent(GUIEventType::Notification, GUIEventCode::Clicked, Buttons);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Move(int X, int Y) {
		int DX = X - m_X;
		int DY = Y - m_Y;

		m_X = X;
		m_Y = Y;

		// Go through all my children moving them
		std::vector<GUIControl *>::iterator it;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControl *C = *it;
			int CX;
			int CY;
			int CW;
			int CH;
			C->GetControlRect(&CX, &CY, &CW, &CH);

			C->Move(CX + DX, CY + DY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Resize(int Width, int Height) {
		int OldWidth = m_Width;
		int OldHeight = m_Height;

		m_Width = Width;
		m_Height = Height;

		// Go through all my children moving them
		std::vector<GUIControl *>::iterator it;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControl *C = *it;
			int CX, CY, CW, CH;
			int Anchor = C->GetAnchor();

			C->GetControlRect(&CX, &CY, &CW, &CH);

			int DX = CX;
			int DY = CY;
			int W = CW;
			int H = CH;

			// Attached to Right and/or Bottom edges
			if ((Anchor & GUIControl::Anchor::AnchorRight) && !(Anchor & GUIControl::Anchor::AnchorLeft)) { DX = m_Width - (OldWidth - (CX - m_X)) + m_X; }
			if ((Anchor & GUIControl::Anchor::AnchorBottom) && !(Anchor & GUIControl::Anchor::AnchorTop)) { DY = m_Height - (OldHeight - (CY - m_Y)) + m_Y; }

			if (DX != CX || DY != CY) { C->Move(DX, DY); }

			CX -= m_X;
			CY -= m_Y;

			// Attached to opposing edges
			if (Anchor & GUIControl::Anchor::AnchorLeft && Anchor & GUIControl::Anchor::AnchorRight) { W = (m_Width - (OldWidth - (CX + CW))) - CX; }
			if (Anchor & GUIControl::Anchor::AnchorTop && Anchor & GUIControl::Anchor::AnchorBottom) { H = (m_Height - (OldHeight - (CY + CH))) - CY; }

			if (W != CW || H != CH) { C->Resize(W, H); }
		}
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::SetDrawImage(GUIBitmap *Bitmap) {
		m_DrawBitmap.reset(Bitmap);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::StoreProperties() {
		m_Properties.AddProperty("DrawBackground", m_DrawBackground);
		m_Properties.AddProperty("DrawType", m_DrawType == DrawType::Color ? "Color" : "Image");
		m_Properties.AddProperty("DrawColor", (int)m_DrawColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		// Get the values
		m_Properties.GetPropertyValue("DrawBackground", &m_DrawBackground);
		std::string value;
		m_Properties.GetPropertyValue("DrawType", &value);
		if (value == "Color") {
			m_DrawType = DrawType::Color;
		} else if (value == "Image") {
			m_DrawType = DrawType::Image;
		}
		m_Properties.GetPropertyValue("DrawColor", &m_DrawColor);
	}
}