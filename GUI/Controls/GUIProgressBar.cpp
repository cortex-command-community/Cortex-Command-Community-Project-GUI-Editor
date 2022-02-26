#include "GUI.h"
#include "GUIProgressBar.h"

namespace RTE {

	const std::string_view GUIProgressBar::c_ControlType = "PROGRESSBAR";

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

	void GUIProgressBar::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the progressbar bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::BuildBitmap() {
		// Create a new bitmap.
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "ProgressBar_Base", 0, 0, m_Width, m_Height);

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

		m_IndicatorImage.reset(m_Skin->CreateBitmap(Values[2], m_Height - 4));
		if (!m_IndicatorImage) {
			return;
		}
		// Tile the centre piece
		m_Skin->GetValue("ProgressBar_Indicator", "Centre", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		for (int y = 0; y < m_IndicatorImage->GetHeight(); y += Values[3]) {
			for (int x = 0; x < m_IndicatorImage->GetWidth(); x += Values[2]) {
				Src->Draw(m_IndicatorImage.get(), x, y, &Rect);
			}
		}

		// Draw the top & bottom pieces
		m_Skin->GetValue("ProgressBar_Indicator", "Top", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		Src->Draw(m_IndicatorImage.get(), 0, 0, &Rect);

		m_Skin->GetValue("ProgressBar_Indicator", "Bottom", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		Src->Draw(m_IndicatorImage.get(), 0, m_IndicatorImage->GetHeight() - Values[3], &Rect);

		m_Skin->GetValue("ProgressBar_Indicator", "Spacing", &m_Spacing);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Draw(GUIScreen *Screen) {
		// Draw the base
		Screen->DrawBitmap(m_DrawBitmap.get(), m_PosX, m_PosY, nullptr);

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

		int x = m_PosX + 2;
		int Limit = (int)ceil(Count);
		for (int i = 0; i < Limit; i++) {
			m_IndicatorImage->Draw(Screen->GetBitmap(), x, m_PosY + 2, nullptr);
			x += m_IndicatorImage->GetWidth() + m_Spacing;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();
		if (PointInside(X, Y)) { AddEvent(GUIEventType::Notification, GUIEventCode::Clicked, Buttons); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIProgressBar::Resize(int newWidth, int newHeight) {
		if (m_Width != newWidth || m_Height != newHeight) {
			GUIControl::Resize(std::max(newWidth, m_MinWidth), std::max(newHeight, m_MinHeight));
			BuildBitmap();
		}
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