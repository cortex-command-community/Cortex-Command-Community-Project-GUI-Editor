#include "GUI.h"
#include "GUICollectionBox.h"

namespace RTE {

	const std::string_view GUICollectionBox::c_ControlType = "COLLECTIONBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUICollectionBox::GUICollectionBox(GUIControlManager *ControlManager) {
		m_OwningManager = ControlManager;

		m_IsContainer = true; // We are a container
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControlBase::Create(Name, X, Y, Width, Height);

		// Minimum size of the control
		m_MinWidth = 10;
		m_MinHeight = 10;

		// Default size of the control
		m_DefWidth = 100;
		m_DefHeight = 100;

		// Setup the panel
		m_X = X;
		m_Y = Y;
		m_Width = m_DefWidth;
		m_Height = m_DefHeight;

		if (Width != -1) { m_Width = Width; }
		if (Height != -1) { m_Height = Height; }

		// Make sure the box isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Create(GUIProperties *Props) {
		GUIControlBase::Create(Props);

		// Minimum size of the control
		m_MinWidth = 10;
		m_MinHeight = 10;

		// Default size of the control
		m_DefWidth = 100;
		m_DefHeight = 100;

		// Setup the panel
		//GUIControlBase::LoadProperties(Props);

		// Make sure the box isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		// Get the values
		Props->GetPropertyValue("DrawBackground", &m_DrawBackground);
		std::string v;
		Props->GetPropertyValue("DrawType", &v);
		if (stricmp(v.c_str(), "Color") == 0) {
			m_DrawType = DrawType::Color;
		} else if (stricmp(v.c_str(), "Image") == 0) {
			m_DrawType = DrawType::Image;
		} else if (stricmp(v.c_str(), "Panel") == 0) {
			m_DrawType = DrawType::Panel;
		}
		Props->GetPropertyValue("DrawColor", &m_DrawColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Destroy() {
		delete m_Background;
		delete m_DrawBitmap;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::ChangeSkin(GUISkin *Skin) {
		GUIControlBase::ChangeSkin(Skin);

		// Build the panel bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::BuildBitmap() {
		// Free any old bitmap
		delete m_DrawBitmap;

		// Create a new bitmap.
		m_DrawBitmap = m_Skin->CreateBitmap(m_Width, m_Height);

		// Create the button image
		m_Skin->BuildStandardRect(m_DrawBitmap, "CollectionBox_Panel", 0, 0, m_Width, m_Height);
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
					Screen->DrawBitmapTrans(m_DrawBitmap, m_X, m_Y, &Rect);
				}
			}
		}
		GUIControlBase::Draw(Screen);
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
		std::vector<GUIControlBase *>::iterator it;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControlBase *C = *it;
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
		std::vector<GUIControlBase *>::iterator it;
		for (it = m_ChildControls.begin(); it != m_ChildControls.end(); it++) {
			GUIControlBase *C = *it;
			int CX, CY, CW, CH;
			int Anchor = C->GetAnchor();

			C->GetControlRect(&CX, &CY, &CW, &CH);

			int DX = CX;
			int DY = CY;
			int W = CW;
			int H = CH;

			// Attached to Right and/or Bottom edges
			if ((Anchor & GUIControlBase::Anchor::AnchorRight) && !(Anchor & GUIControlBase::Anchor::AnchorLeft)) { DX = m_Width - (OldWidth - (CX - m_X)) + m_X; }
			if ((Anchor & GUIControlBase::Anchor::AnchorBottom) && !(Anchor & GUIControlBase::Anchor::AnchorTop)) { DY = m_Height - (OldHeight - (CY - m_Y)) + m_Y; }

			if (DX != CX || DY != CY) { C->Move(DX, DY); }

			CX -= m_X;
			CY -= m_Y;

			// Attached to opposing edges
			if (Anchor & GUIControlBase::Anchor::AnchorLeft && Anchor & GUIControlBase::Anchor::AnchorRight) { W = (m_Width - (OldWidth - (CX + CW))) - CX; }
			if (Anchor & GUIControlBase::Anchor::AnchorTop && Anchor & GUIControlBase::Anchor::AnchorBottom) { H = (m_Height - (OldHeight - (CY + CH))) - CY; }

			if (W != CW || H != CH) { C->Resize(W, H); }
		}
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::SetDrawImage(GUIBitmap *Bitmap) {
		// Free any old bitmap
		delete m_DrawBitmap;

		m_DrawBitmap = Bitmap;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::StoreProperties() {
		m_Properties.AddProperty("DrawBackground", m_DrawBackground);
		m_Properties.AddProperty("DrawType", m_DrawType == DrawType::Color ? "Color" : "Image");
		m_Properties.AddProperty("DrawColor", (int)m_DrawColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::ApplyProperties(GUIProperties *Props) {
		GUIControlBase::ApplyProperties(Props);

		// Get the values
		m_Properties.GetPropertyValue("DrawBackground", &m_DrawBackground);
		std::string v;
		m_Properties.GetPropertyValue("DrawType", &v);
		if (stricmp(v.c_str(), "Color") == 0) {
			m_DrawType = DrawType::Color;
		} else if (stricmp(v.c_str(), "Image") == 0) {
			m_DrawType = DrawType::Image;
		}
		m_Properties.GetPropertyValue("DrawColor", &m_DrawColor);
	}
}