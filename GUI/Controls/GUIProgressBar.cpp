#include "GUI.h"
#include "GUIProgressBar.h"

namespace RTE {

	const std::string_view GUIProgressBar::c_ControlType = "PROGRESSBAR";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControl::Create(Name, X, Y, Width, Height);

		// Setup the panel
		m_X = X;
		m_Y = Y;
		m_Width = m_DefaultWidth;
		m_Height = m_DefaultHeight;

		if (Width != -1) { m_Width = Width; }
		if (Height != -1) { m_Height = Height; }

		// Make sure the control isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the control isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		Props->GetPropertyValue("Minimum", &m_Minimum);
		Props->GetPropertyValue("Maximum", &m_Maximum);
		Props->GetPropertyValue("Value", &m_Value);

		// Clamp the value
		m_Value = std::max(m_Value, m_Minimum);
		m_Value = std::min(m_Value, m_Maximum);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIProgressBar::~GUIProgressBar() {
		// Destroy the drawing bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}

		// Destroy the indicator bitmap
		if (m_IndicatorImage) {
			m_IndicatorImage->Destroy();
			delete m_IndicatorImage;
			m_IndicatorImage = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the progressbar bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::BuildBitmap() {
		// Free any old bitmaps
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}
		if (m_IndicatorImage) {
			m_IndicatorImage->Destroy();
			delete m_IndicatorImage;
			m_IndicatorImage = nullptr;
		}

		// Create a new bitmap.
		m_DrawBitmap = m_Skin->CreateBitmap(m_Width, m_Height);

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap, "ProgressBar_Base", 0, 0, m_Width, m_Height);

		// Build the indicator
		std::string Filename;
		m_Skin->GetValue("ProgressBar_Indicator", "Filename", &Filename);
		GUIBitmap *Src = m_Skin->CreateBitmap(Filename);
		if (!Src) {
			return;
		}
		int Values[4];
		GUIRect Rect;
		m_Skin->GetValue("ProgressBar_Indicator", "Top", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		m_IndicatorImage = m_Skin->CreateBitmap(Values[2], m_Height - 4);
		if (!m_IndicatorImage) {
			return;
		}
		// Tile the centre piece
		m_Skin->GetValue("ProgressBar_Indicator", "Centre", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		for (int y = 0; y < m_IndicatorImage->GetHeight(); y += Values[3]) {
			for (int x = 0; x < m_IndicatorImage->GetWidth(); x += Values[2]) {
				Src->Draw(m_IndicatorImage, x, y, &Rect);
			}
		}

		// Draw the top & bottom pieces
		m_Skin->GetValue("ProgressBar_Indicator", "Top", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		Src->Draw(m_IndicatorImage, 0, 0, &Rect);

		m_Skin->GetValue("ProgressBar_Indicator", "Bottom", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		Src->Draw(m_IndicatorImage, 0, m_IndicatorImage->GetHeight() - Values[3], &Rect);

		m_Skin->GetValue("ProgressBar_Indicator", "Spacing", &m_Spacing);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Draw(GUIScreen *Screen) {
		// Draw the base
		Screen->DrawBitmap(m_DrawBitmap, m_X, m_Y, nullptr);

		// Draw the indicators
		if (!m_IndicatorImage) {
			return;
		}

		float Count = 0;
		if (m_Maximum - m_Minimum > 0) {
			float V = (float)(m_Value - m_Minimum) / (float)(m_Maximum - m_Minimum);
			Count = (float)m_Width*V;
		}
		if (m_IndicatorImage->GetWidth() + m_Spacing > 0) { Count = Count / (float)(m_IndicatorImage->GetWidth() + m_Spacing); }

		// Setup the clipping
		GUIRect Rect = *GetRect();
		Rect.left++;
		Rect.right -= 2;
		Screen->GetBitmap()->SetClipRect(&Rect);

		int x = m_X + 2;
		int Limit = (int)ceil(Count);
		for (int i = 0; i < Limit; i++) {
			m_IndicatorImage->Draw(Screen->GetBitmap(), x, m_Y + 2, nullptr);
			x += m_IndicatorImage->GetWidth() + m_Spacing;
		}

		Screen->GetBitmap()->SetClipRect(nullptr);

		GUIControl::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();
		if (PointInside(X, Y)) { AddEvent(GUIEventType::Notification, GUIEventCode::Clicked, Buttons); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUIControl::SetSize(Width, Height);

		// Rebuild the bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::StoreProperties() {
		m_Properties.AddProperty("Minimum", m_Minimum);
		m_Properties.AddProperty("Maximum", m_Maximum);
		m_Properties.AddProperty("Value", m_Value);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::SetValue(int Value) {
		int OldValue = m_Value;
		m_Value = Value;

		// Clamp the value
		m_Value = std::min(m_Value, m_Maximum);
		m_Value = std::max(m_Value, m_Minimum);

		// Changed?
		if (m_Value != OldValue) { AddEvent(GUIEventType::Notification, GUIEventCode::Changed); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		m_Properties.GetPropertyValue("Minimum", &m_Minimum);
		m_Properties.GetPropertyValue("Maximum", &m_Maximum);
		m_Properties.GetPropertyValue("Value", &m_Value);

		// Clamp the value
		m_Value = std::max(m_Value, m_Minimum);
		m_Value = std::min(m_Value, m_Maximum);
	}
}