#include "GUI.h"
#include "GUILabel.h"

namespace RTE {

	const std::string_view GUILabel::c_ControlType = "LABEL";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);

		m_Properties.AddProperty("Text", "");
		m_Properties.AddProperty("HAlignment", "Left");
		m_Properties.AddProperty("VAlignment", "Middle");
		m_Properties.AddProperty("HorizontalOverflowScroll", false);
		m_Properties.AddProperty("VerticalOverflowScroll", false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::Create(GUIProperties *reference) {
		GUIControl::Create(reference);

		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		m_Properties.OverwriteProperties(reference, true);

		m_Properties.AddProperty("Text", reference->GetPropertyValue<std::string>("Text"));
		m_Properties.AddProperty("HAlignment", GUIFont::HAlignment::Left);
		m_Properties.AddProperty("VAlignment", GUIFont::VAlignment::Middle);
		m_Properties.AddProperty("HorizontalOverflowScroll", reference->GetPropertyValue<bool>("HorizontalOverflowScroll"));
		m_Properties.AddProperty("VerticalOverflowScroll", reference->GetPropertyValue<bool>("VerticalOverflowScroll"));

		std::string alignString;
		reference->GetPropertyValue("HAlignment", &alignString);
		if (alignString == "left") { m_HAlignment = GUIFont::HAlignment::Left; }
		if (alignString == "centre" || alignString == "center") { m_HAlignment = GUIFont::HAlignment::Centre; }
		if (alignString == "right") { m_HAlignment = GUIFont::HAlignment::Right; }

		reference->GetPropertyValue("VAlignment", &alignString);
		if (alignString == "top") { m_VAlignment = GUIFont::VAlignment::Top; }
		if (alignString == "middle") { m_VAlignment = GUIFont::VAlignment::Middle; }
		if (alignString == "bottom") { m_VAlignment = GUIFont::VAlignment::Bottom; }

	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUILabel::GetTextHeight() {
		return m_Font->CalculateHeight(GetText(), m_Width);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::Resize(int newWidth, int newHeight) {
		if (m_Width != newWidth || m_Height != newHeight) { GUIControl::Resize(std::max(newWidth, m_MinWidth), std::max(newHeight, m_MinHeight)); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::SetHorizontalOverflowScroll(bool newOverflowScroll) {
		m_HorizontalOverflowScroll = newOverflowScroll;
		if (m_HorizontalOverflowScroll) {
			m_VerticalOverflowScroll = false;
		} else if (!m_VerticalOverflowScroll) {
			m_OverflowScrollState = OverflowScrollState::Deactivated;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::SetVerticalOverflowScroll(bool newOverflowScroll) {
		m_VerticalOverflowScroll = newOverflowScroll;
		if (m_VerticalOverflowScroll) {
			m_HorizontalOverflowScroll = false;
		} else if (!m_HorizontalOverflowScroll) {
			m_OverflowScrollState = OverflowScrollState::Deactivated;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::ActivateDeactivateOverflowScroll(bool activateScroll) {
		if (OverflowScrollIsEnabled() && activateScroll != OverflowScrollIsActivated()) {
			m_OverflowScrollState = activateScroll ? OverflowScrollState::WaitAtStart : OverflowScrollState::Deactivated;
			m_OverflowScrollTimer.SetRealTimeLimitMS(-1);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::StoreProperties() {
		//m_Properties.AddProperty("Text", m_Text);

		if (m_HAlignment == GUIFont::HAlignment::Left) {
			m_Properties.AddProperty("HAlignment", "left");
		} else if (m_HAlignment == GUIFont::HAlignment::Centre) {
			m_Properties.AddProperty("HAlignment", "centre");
		} else if (m_HAlignment == GUIFont::HAlignment::Right) {
			m_Properties.AddProperty("HAlignment", "right");
		}
		if (m_VAlignment == GUIFont::VAlignment::Top) {
			m_Properties.AddProperty("VAlignment", "top");
		} else if (m_VAlignment == GUIFont::VAlignment::Middle) {
			m_Properties.AddProperty("VAlignment", "middle");
		} else if (m_VAlignment == GUIFont::VAlignment::Bottom) {
			m_Properties.AddProperty("VAlignment", "bottom");
		}
		m_Properties.AddProperty("HorizontalOverflowScroll", m_HorizontalOverflowScroll);
		m_Properties.AddProperty("VerticalOverflowScroll", m_VerticalOverflowScroll);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		//m_Properties.GetPropertyValue("Text", &m_Text);

		std::string alignString;
		m_Properties.GetPropertyValue("HAlignment", &alignString);
		if (alignString == "left") { m_HAlignment = GUIFont::HAlignment::Left; }
		if (alignString == "centre") { m_HAlignment = GUIFont::HAlignment::Centre; }
		if (alignString == "right") { m_HAlignment = GUIFont::HAlignment::Right; }

		m_Properties.GetPropertyValue("VAlignment", &alignString);
		if (alignString == "top") { m_VAlignment = GUIFont::VAlignment::Top; }
		if (alignString == "middle") { m_VAlignment = GUIFont::VAlignment::Middle; }
		if (alignString == "bottom") { m_VAlignment = GUIFont::VAlignment::Bottom; }

		m_Properties.GetPropertyValue("HorizontalOverflowScroll", &m_HorizontalOverflowScroll);
		m_Properties.GetPropertyValue("VerticalOverflowScroll", &m_VerticalOverflowScroll);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUILabel::ResizeHeightToFit() {
		int newHeight = m_Font->CalculateHeight(GetText(), m_Width);
		GUIControl::Resize(m_Width, newHeight);

		return newHeight;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) {
		if (buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			CaptureMouse();
			SetFocus();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) {
		if (m_Captured && (buttons & GUIControl::MouseButtons::MOUSE_LEFT) && PointInside(mousePosX, mousePosY)) { AddEvent(GUIEventType::Notification, GUIEventCode::Clicked); }
		ReleaseMouse();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::ChangeSkin(GUISkin *newSkin) {
		GUIControl::ChangeSkin(newSkin);

		std::string fontFileName;
		m_Skin->GetValue("Label", "Font", &fontFileName);
		m_Font = m_Skin->GetFont(fontFileName);

		m_Skin->GetValue("Label", "FontColor", &m_FontColor);
		m_Skin->GetValue("Label", "FontShadow", &m_FontShadow);
		m_Skin->GetValue("Label", "FontKerning", &m_FontKerning);
		m_FontColor = m_Skin->ConvertColor(m_FontColor);
		m_Font->CacheColor(m_FontColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::Draw(GUIScreen *targetScreen) {
		Draw(targetScreen->GetBitmap());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUILabel::Draw(GUIBitmap *targetBitmap, bool overwiteFontColorAndKerning) {
		if (m_Font) {
			if (overwiteFontColorAndKerning) {
				m_Font->SetColor(m_FontColor);
				m_Font->SetKerning(m_FontKerning);
			}

			// Adjust for horizontal alignment
			int xPos = m_PosX;
			if (m_HAlignment == GUIFont::HAlignment::Centre) {
				xPos += m_Width / 2;
			} else if (m_HAlignment == GUIFont::HAlignment::Right) {
				xPos += m_Width;
			}

			// Adjust for vertical alignment
			int yPos = m_PosY;
			if (m_VAlignment == GUIFont::VAlignment::Middle) {
				yPos += (m_Height / 2) - 1;
			} else if (m_VAlignment == GUIFont::VAlignment::Bottom) {
				yPos += (m_Height)-1;
			}

			int textFullWidth = m_HorizontalOverflowScroll ? m_Font->CalculateWidth(GetText()) : 0;
			int textFullHeight = m_VerticalOverflowScroll ? m_Font->CalculateHeight(GetText()) : 0;
			bool modifyXPos = textFullWidth > m_Width;
			bool modifyYPos = textFullHeight > m_Height;
			xPos = modifyXPos ? m_PosX : xPos;
			yPos = modifyYPos ? m_PosY : yPos;
			if (OverflowScrollIsActivated()) {
				switch (m_OverflowScrollState) {
					case OverflowScrollState::WaitAtStart:
						if (m_OverflowScrollTimer.GetRealTimeLimitMS() == -1) {
							m_OverflowScrollTimer.SetRealTimeLimitMS(1000);
							m_OverflowScrollTimer.Reset();
						} else if (m_OverflowScrollTimer.IsPastRealTimeLimit()) {
							m_OverflowScrollState = OverflowScrollState::Scrolling;
							m_OverflowScrollTimer.SetRealTimeLimitMS(-1);
							break;
						}
						break;
					case OverflowScrollState::Scrolling:
						if (m_OverflowScrollTimer.GetRealTimeLimitMS() == -1) {
							//TODO Maybe time limits should account for extra size vs width, so it scrolls slower on small labels, since it can be harder to read fast text on smaller areas. I think it's fine as-is though.
							// Note - time limits set so 5 characters of fatfont horizontal overflow or one line of fatfont vertical overflow will take 1 second.
							if (modifyXPos) {
								m_OverflowScrollTimer.SetRealTimeLimitMS((1000.0 / 30.0) * static_cast<double>(textFullWidth - m_Width));
							} else if (modifyYPos) {
								m_OverflowScrollTimer.SetRealTimeLimitMS((1000.0 / 8.0) * static_cast<double>(textFullHeight - m_Height));
							}
							m_OverflowScrollTimer.Reset();
						} else if (m_OverflowScrollTimer.IsPastRealTimeLimit()) {
							m_OverflowScrollTimer.SetRealTimeLimitMS(-1);
							m_OverflowScrollState = OverflowScrollState::WaitAtEnd;
							break;
						}
						xPos -= modifyXPos ? static_cast<int>(static_cast<double>(textFullWidth - m_Width) * m_OverflowScrollTimer.RealTimeLimitProgress()) : 0;
						yPos -= modifyYPos ? static_cast<int>(static_cast<double>(textFullHeight - m_Height) * m_OverflowScrollTimer.RealTimeLimitProgress()) : 0;
						break;
					case OverflowScrollState::WaitAtEnd:
						if (m_OverflowScrollTimer.GetRealTimeLimitMS() == -1) {
							m_OverflowScrollTimer.SetRealTimeLimitMS(1000);
							m_OverflowScrollTimer.Reset();
						} else if (m_OverflowScrollTimer.IsPastRealTimeLimit()) {
							m_OverflowScrollTimer.SetRealTimeLimitMS(-1);
							m_OverflowScrollState = OverflowScrollState::WaitAtStart;
							break;
						}
						xPos -= modifyXPos ? (textFullWidth - m_Width) : 0;
						yPos -= modifyYPos ? (textFullHeight - m_Height) : 0;
						break;
					default:
						GUIAbort("Invalid GUILabel overflow scroll state " + static_cast<int>(m_OverflowScrollState));
						break;
				}
			}
			m_Font->DrawAligned(targetBitmap, xPos, yPos, GetText(), m_HorizontalOverflowScroll && textFullWidth > m_Width ? GUIFont::HAlignment::Left : m_HAlignment, m_VerticalOverflowScroll && textFullHeight > m_Height ? GUIFont::VAlignment::Top : m_VAlignment, m_HorizontalOverflowScroll ? textFullWidth : m_Width, m_FontShadow);
		}
	}
}