#include "GUI.h"
#include "GUISlider.h"

namespace RTE {

	const std::string_view GUISlider::c_ControlType = "SLIDER";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);
		CalculateKnob();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the button isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);


		// Get the values
		std::string orientation;
		Props->GetPropertyValue("Orientation", &orientation);
		if (orientation == "horizontal") {
			m_Orientation = Orientation::Horizontal;
		} else if (orientation == "vertical") {
			m_Orientation = Orientation::Vertical;
		}

		std::string tick;
		Props->GetPropertyValue("TickDirection", &tick);
		if (tick == "TopLeft") {
			m_TickDirection = TickDirection::TopLeft;
		} else if (tick == "BottomRight") {
			m_TickDirection = TickDirection::BottomRight;
		}
		Props->GetPropertyValue("Minimum", &m_Minimum);
		Props->GetPropertyValue("Maximum", &m_Maximum);
		Props->GetPropertyValue("Value", &m_Value);
		if (!Props->GetPropertyValue("ValueResolution", &m_ValueResolution)) { m_ValueResolution = std::max((m_Maximum - m_Minimum) / 100, 1); }

		m_Value = std::clamp(m_Value, m_Minimum, m_Maximum);

		// Re-Calculate the knob info
		CalculateKnob();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUISlider::~GUISlider() {
		// Destroy the knob image bitmap
		if (m_KnobImage) {
			m_KnobImage->Destroy();
			delete m_KnobImage;
			m_KnobImage = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Build the progressbar bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::BuildBitmap() {
		// Free any old bitmaps
		if (m_KnobImage) {
			m_KnobImage->Destroy();
			delete m_KnobImage;
			m_KnobImage = nullptr;
		}

		std::string Section;
		if (m_Orientation == Orientation::Horizontal) {
			Section = "Slider_Horz";
		} else {
			Section = "Slider_Vert";
		}

		// Load the source image
		std::string Filename;
		m_Skin->GetValue(Section, "Filename", &Filename);
		GUIBitmap *SrcImage = m_Skin->CreateBitmap(Filename);
		if (!SrcImage) {
			return;
		}

		// Build the line & ticks
		BuildLine(Section, SrcImage);

		// Load the indicator image
		std::string Side;
		if (m_TickDirection == TickDirection::TopLeft) {
			Side = "TopLeftSlider";
		} else {
			Side = "BottomRightSlider";
		}

		int Values[4];
		GUIRect Rect;
		m_Skin->GetValue(Section, Side, Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		// Create the indicator bitmap
		m_KnobImage = m_Skin->CreateBitmap(Values[2], Values[3]);
		SrcImage->Draw(m_KnobImage, 0, 0, &Rect);

		int ColorKey;
		m_Skin->GetValue(Section, "ColorKeyIndex", &ColorKey);
		ColorKey = m_Skin->ConvertColor(ColorKey, m_KnobImage->GetColorDepth());
		m_KnobImage->SetColorKey(ColorKey);
		m_DrawBitmap->SetColorKey(ColorKey);

		// Re-Calculate the knob info
		CalculateKnob();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::BuildLine(const std::string &Section, GUIBitmap *SrcImage) {
		int Values[4];
		GUIRect Rect;

		// Get the size of the middle bit for determining part of the bitmap size
		m_Skin->GetValue(Section, "Middle", Values, 4);
		if (m_Orientation == Orientation::Horizontal) {
			m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, Values[3]));
		} else {
			m_DrawBitmap.reset(m_Skin->CreateBitmap(Values[2], m_Height));
		}

		// Tile the line
		m_Skin->GetValue(Section, "Middle", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		if (m_Orientation == Orientation::Horizontal) {
			for (int i = 0; i < m_Width; i += Values[2]) {
				SrcImage->Draw(m_DrawBitmap.get(), i, 0, &Rect);
			}
		} else {
			for (int i = 0; i < m_Height; i += Values[3]) {
				SrcImage->Draw(m_DrawBitmap.get(), 0, i, &Rect);
			}
		}

		// Draw the start
		m_Skin->GetValue(Section, "Start", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		SrcImage->Draw(m_DrawBitmap.get(), 0, 0, &Rect);

		m_EndThickness = m_Orientation == Orientation::Horizontal ? Values[2] : Values[3];

		// Draw the end
		int X = 0;
		int Y = 0;
		m_Skin->GetValue(Section, "End", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);
		if (m_Orientation == Orientation::Horizontal) {
			X = m_Width - Values[2];
		} else {
			Y = m_Height - Values[3];
		}
		SrcImage->Draw(m_DrawBitmap.get(), X, Y, &Rect);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::Draw(GUIScreen *Screen) {
		int X = 0;
		int Y = 0;

		if (m_Orientation == Orientation::Horizontal) {
			Y = m_Height / 2 - m_DrawBitmap->GetHeight() / 2;
		} else {
			X = m_Width / 2 - m_DrawBitmap->GetWidth() / 2;
		}

		// Draw the base
		Screen->DrawBitmap(m_DrawBitmap.get(), m_X + X, m_Y + Y, nullptr);

		// Draw the indicator
		if (!m_KnobImage) {
			return;
		}

		int Half = 0;
		if (m_Orientation == Orientation::Horizontal) {
			Half = m_DrawBitmap->GetHeight() / 2;
		} else {
			Half = m_DrawBitmap->GetWidth() / 2;
		}
		if (m_Orientation == Orientation::Horizontal) {
			m_KnobImage->DrawTrans(Screen->GetBitmap(), m_X + m_KnobPosition, m_Y + Y + Half - m_KnobImage->GetHeight() / 2, nullptr);
		} else {
			m_KnobImage->DrawTrans(Screen->GetBitmap(), m_X + X + Half - m_KnobImage->GetWidth() / 2, m_Y + m_KnobPosition, nullptr);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		CaptureMouse();
		SetFocus();

		m_OldValue = m_Value;
		int Size = 0;

		// Only do stuff here on a left mouse click
		if (!(Buttons & GUIControl::MouseButtons::MOUSE_LEFT)) {
			return;
		}

		if (m_Orientation == Orientation::Horizontal) {
			if (X > m_X + m_EndThickness && X < m_X + m_Width - m_EndThickness) {
				m_GrabbedKnob = true;
				m_KnobPosition = X - m_X - (m_KnobLength / 2);
				m_GrabbedPos = X - (m_X + m_KnobPosition);
			}
			Size = m_Width;
		} else {
			if (Y > m_Y + m_EndThickness && Y < m_Y + m_Height - m_EndThickness) {
				m_GrabbedKnob = true;
				m_KnobPosition = Y - m_Y - (m_KnobLength / 2);
				m_GrabbedPos = Y - (m_Y + m_KnobPosition);
			}
			Size = m_Height;
		}

		m_KnobPosition = std::clamp(m_KnobPosition, m_EndThickness, Size - m_KnobLength - m_EndThickness);

		// Calculate the new value
		int Area = Size - m_KnobLength;
		if (Area > 0) {
			float p = (float)m_KnobPosition / (float)(Area);
			int MaxRange = (m_Maximum - m_Minimum);
			m_Value = (float)MaxRange * p + m_Minimum;
		}

		m_Value = std::clamp(m_Value, m_Minimum, m_Maximum);

		// If the value has changed, add the "Changed" notification
		if (m_Value != m_OldValue)
			AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();

		m_GrabbedKnob = false;

		// If the value has changed, add the "Changed" notification
		if (m_Value != m_OldValue) { AddEvent(GUIEventType::Notification, GUIEventCode::Changed); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::OnMouseMove(int X, int Y, int Buttons, int Modifier) {
		int MousePos = X;
		int KnobTop = 0;
		int Size = 1;

		// Horizontal
		if (m_Orientation == Orientation::Horizontal) {
			MousePos = X;
			KnobTop = m_X + m_KnobPosition;
			Size = m_Width;
		}

		// Vertical
		if (m_Orientation == Orientation::Vertical) {
			MousePos = Y;
			KnobTop = m_Y + m_KnobPosition;
			Size = m_Height;
		}

		// Move the knob if it is grabbed
		if (m_GrabbedKnob) {
			int Delta = m_GrabbedPos - (MousePos - KnobTop);
			m_KnobPosition -= Delta;

			// Clamp the knob position
			m_KnobPosition = std::max(m_KnobPosition, 0);
			m_KnobPosition = std::min(m_KnobPosition, Size - m_KnobLength);

			// Calculate the new value
			int Area = Size - m_KnobLength;
			if (Area > 0) {
				float p = (float)m_KnobPosition / (float)(Area);
				int MaxRange = (m_Maximum - m_Minimum);
				m_Value = (float)MaxRange * p + m_Minimum;
			}

			// Clamp the value
			m_Value = std::max(m_Value, m_Minimum);
			m_Value = std::min(m_Value, m_Maximum);

			// Clamp the knob position again for the graphics
			m_KnobPosition = std::max(m_KnobPosition, m_EndThickness);
			m_KnobPosition = std::min(m_KnobPosition, Size - m_KnobLength - m_EndThickness);

			// If the value has changed, add the "Changed" notification
			if (m_Value != m_OldValue) { AddEvent(GUIEventType::Notification, GUIEventCode::Changed); }

			m_OldValue = m_Value;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) {
		m_OldValue = m_Value;

		if (mouseWheelChange < 0) {
			m_Value = std::max(m_Value - m_ValueResolution, m_Minimum);
		} else {
			m_Value = std::min(m_Value + m_ValueResolution, m_Maximum);
		}

		if (m_Value != m_OldValue) {
			CalculateKnob();
			AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::CalculateKnob() {
		if (!m_KnobImage) {
			return;
		}

		if (m_Maximum > m_Minimum) {
			const bool horizontalOrientation = (m_Orientation == Orientation::Horizontal);
			m_KnobLength = horizontalOrientation ? m_KnobImage->GetWidth() : m_KnobImage->GetHeight();
			const int size = horizontalOrientation ? m_Width : m_Height;
			const float valueRatio = static_cast<float>(m_Value - m_Minimum) / static_cast<float>(m_Maximum - m_Minimum);
			m_KnobPosition = m_EndThickness + static_cast<int>(static_cast<float>(size - m_KnobLength - (m_EndThickness * 2)) * valueRatio);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::Resize(int Width, int Height) {
		// Make sure the control isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);

		GUIControl::SetSize(Width, Height);

		// Rebuild the bitmap
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetOrientation(int Orientation) {
		m_Orientation = Orientation;

		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetTickDirection(int TickDir) {
		m_TickDirection = TickDir;

		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetMinimum(int Minimum) {
		if (Minimum != m_Minimum) {
			m_Minimum = Minimum;
			m_Value = std::max(m_Value, m_Minimum);

			// Re-Calculate the knob info
			CalculateKnob();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetMaximum(int Maximum) {
		if (Maximum != m_Maximum) {
			m_Maximum = Maximum;
			m_Value = std::min(m_Value, m_Maximum);

			// Re-Calculate the knob info
			CalculateKnob();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetValue(int Value) {
		int OldValue = m_Value;

		m_Value = std::clamp(Value, m_Minimum, m_Maximum);

		if (m_Value != OldValue) {
			CalculateKnob();
			AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::StoreProperties() {
		m_Properties.AddProperty("Value", m_Value);
		m_Properties.AddProperty("Minimum", m_Minimum);
		m_Properties.AddProperty("Maximum", m_Maximum);
		m_Properties.AddProperty("ValueResolution", m_ValueResolution);
		m_Properties.AddProperty("Orientation", m_Orientation == Orientation::Horizontal ? "Horizontal" : "Vertical");
		m_Properties.AddProperty("TickDirection", m_TickDirection == TickDirection::TopLeft ? "TopLeft" : "BottomRight");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		// Get the values
		std::string orientation;
		m_Properties.GetPropertyValue("Orientation", &orientation);
		if (orientation == "horizontal") {
			m_Orientation = Orientation::Horizontal;
		} else if (orientation == "vertical") {
			m_Orientation = Orientation::Vertical;
		}

		std::string tick;
		m_Properties.GetPropertyValue("TickDirection", &tick);
		if (tick == "TopLeft") {
			m_TickDirection = TickDirection::TopLeft;
		} else if (tick == "BottomRight") {
			m_TickDirection = TickDirection::BottomRight;
		}
		m_Properties.GetPropertyValue("Minimum", &m_Minimum);
		m_Properties.GetPropertyValue("Maximum", &m_Maximum);
		m_Properties.GetPropertyValue("Value", &m_Value);
		m_Properties.GetPropertyValue("ValueResolution", &m_ValueResolution);

		// Clamp the value
		m_Value = std::max(m_Value, m_Minimum);
		m_Value = std::min(m_Value, m_Maximum);

		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISlider::SetValueResolution(int valueRes) {
		if (valueRes >= 1 && valueRes <= m_Maximum - m_Minimum) { m_ValueResolution = valueRes; }
	}
}