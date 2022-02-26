#include "GUI.h"
#include "GUITextBox.h"

namespace RTE {

	const std::string_view GUITextBox::c_ControlType = "TEXTBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Create(GUIProperties *reference) {
		GUIControl::Create(reference);

		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		std::string alignString;
		reference->GetPropertyValue("HAlignment", &alignString);
		if (alignString == "left") {
			m_HAlignment = GUIFont::HAlignment::Left;
		} else if (alignString == "centre" || alignString == "center") {
			m_HAlignment = GUIFont::HAlignment::Centre;
		} else if (alignString == "right") {
			m_HAlignment = GUIFont::HAlignment::Right;
		}

		reference->GetPropertyValue("VAlignment", &alignString);
		if (alignString == "top") {
			m_VAlignment = GUIFont::VAlignment::Top;
		} else if (alignString == "middle") {
			m_VAlignment = GUIFont::VAlignment::Middle;
		} else if (alignString == "bottom") {
			m_VAlignment = GUIFont::VAlignment::Bottom;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::StoreProperties() {
		m_Properties.AddProperty("NumericOnly", GetNumericOnly());
		m_Properties.AddProperty("MaxNumericValue", GetMaxNumericValue());
		m_Properties.AddProperty("MaxTextLength", GetMaxTextLength());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		bool numericOnly = GetNumericOnly();
		m_Properties.GetPropertyValue("NumericOnly", &numericOnly);
		SetNumericOnly(numericOnly);

		int maxNumericValue = GetMaxNumericValue();
		m_Properties.GetPropertyValue("MaxNumericValue", &maxNumericValue);
		SetMaxNumericValue(maxNumericValue);

		int maxTextLength = GetMaxTextLength();
		m_Properties.GetPropertyValue("MaxTextLength", &maxTextLength);
		SetMaxTextLength(maxTextLength);

		// Force a rebuild of the bitmap
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Resize(int newWidth, int newHeight) {
		if (m_Width != newWidth || m_Height != newHeight) {
			GUIControl::Resize(std::max(newWidth, m_MinWidth), std::max(newHeight, m_MinHeight));
			ChangeSkin(m_Skin);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::UpdateText(bool Typing, bool DoIncrement) {
		if (!m_Font) {
			return;
		}

		// Using increments of four characters to show a little extra of the text when moving and typing. Only do this when NOT typing
		int Increment = 4;
		int Spacer = 2;

		if (Typing) { Increment = 1; }

		// Make sure the cursor is greater or equal to the start index
		if (m_CursorIndex <= m_StartIndex && DoIncrement) { m_StartIndex = m_CursorIndex - Increment; }

		// Clamp it
		m_StartIndex = std::max(m_StartIndex, 0);

		// If the cursor is greater than the length of text panel, adjust the start index
		std::string Sub = m_Text.substr(m_StartIndex, m_CursorIndex - m_StartIndex);
		while (m_Font->CalculateWidth(Sub) > m_Width - Spacer * 2 && DoIncrement) {
			m_StartIndex += Increment;
			Sub = m_Text.substr(m_StartIndex, m_CursorIndex - m_StartIndex);
		}

		// Clamp it
		m_StartIndex = std::max(0, std::min(m_StartIndex, static_cast<int>(m_Text.size() - 1)));

		// Adjust the cursor position
		m_CursorX = m_Font->CalculateWidth(m_Text.substr(m_StartIndex, m_CursorIndex - m_StartIndex));

		// Update the selection
		if (m_GotSelection) { SetSelection(m_StartSelection, m_EndSelection); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int RTE::GUITextBox::GetStartOfNextCharacterGroup(const std::string_view &stringToCheck, int currentIndex) const {
		auto isNormalCharacter = [](char charToCheck) {
			return (std::isalnum(charToCheck) || charToCheck == '_');
		};
		auto isNormalCharacterOrSpace = [](char charToCheck) {
			return (std::isalnum(charToCheck) || charToCheck == '_' || std::isspace(charToCheck));
		};
		auto isSpecialCharacterOrSpace = [](char charToCheck) {
			return !(std::isalnum(charToCheck) || charToCheck == '_');
		};

		std::string_view::const_iterator currentIterator = stringToCheck.cbegin() + currentIndex;
		currentIterator = isNormalCharacter(*currentIterator) ?
			std::find_if(currentIterator, stringToCheck.cend(), isSpecialCharacterOrSpace) :
			std::find_if(currentIterator, stringToCheck.cend(), isNormalCharacterOrSpace);

		if (currentIterator != stringToCheck.cend() && std::isspace(*currentIterator)) { currentIterator = std::find_if_not(currentIterator, stringToCheck.cend(), isspace); }
		return std::distance(stringToCheck.cbegin(), currentIterator);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int RTE::GUITextBox::GetStartOfPreviousCharacterGroup(const std::string_view &stringToCheck, int currentIndex) const {
		auto isNormalCharacter = [](char charToCheck) {
			return (std::isalnum(charToCheck) || charToCheck == '_');
		};
		auto isNormalCharacterOrSpace = [](char charToCheck) {
			return (std::isalnum(charToCheck) || charToCheck == '_' || std::isspace(charToCheck));
		};
		auto isSpecialCharacterOrSpace = [](char charToCheck) {
			return !(std::isalnum(charToCheck) || charToCheck == '_');
		};

		std::string_view::reverse_iterator currentIterator = stringToCheck.crbegin() + (m_Text.size() - currentIndex);
		if (std::isspace(*currentIterator)) { currentIterator = std::find_if_not(currentIterator, stringToCheck.crend(), isspace); }

		if (currentIterator != stringToCheck.crend()) {
			currentIterator = isNormalCharacter(*currentIterator) ?
				std::find_if(currentIterator, stringToCheck.crend(), isSpecialCharacterOrSpace) :
				std::find_if(currentIterator, stringToCheck.crend(), isNormalCharacterOrSpace);
		}
		return std::distance(stringToCheck.cbegin(), currentIterator.base());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::RemoveSelectionText() {
		if (!m_GotSelection) {
			return;
		}

		int Start = std::min(m_StartSelection, m_EndSelection);
		int End = std::max(m_StartSelection, m_EndSelection);

		if (Start == End) {
			return;
		}

		m_Text.erase(Start, End - Start);

		m_CursorIndex = Start;
		UpdateText(false);

		m_GotSelection = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::SetCursorPos(int cursorPos) {
		m_GotSelection = false;

		if (cursorPos <= 0) { cursorPos = 0; }
		if (cursorPos > m_Text.size()) { cursorPos = m_Text.size(); }

		m_CursorIndex = m_Text.size();

		UpdateText();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUITextBox::GetSelectionText() const {
		if (!m_GotSelection) {
			return "";
		}
		int Start = std::min(m_StartSelection, m_EndSelection);
		int End = std::max(m_StartSelection, m_EndSelection);

		if (Start == End) {
			return "";
		}
		return m_Text.substr(Start, End - Start);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::SetText(const std::string_view &Text) {
		m_Text = Text;

		// Clear the selection
		ClearSelection();

		// Clear the cursor position
		m_CursorIndex = 0;
		m_StartIndex = 0;
		m_CursorX = 0;

		UpdateText(false, false);

		AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::SetRightText(const std::string_view &rightText) {
		m_RightText = rightText;
		AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::SetSelection(int Start, int End) {
		if (!m_ReadOnly) {
			m_GotSelection = false;

			// Start a selection
			if (!m_GotSelection) {
				m_GotSelection = true;
				m_StartSelection = Start;
				m_EndSelection = End;
			} else {
				// Update the selection
				m_EndSelection = End;
			}

			// Avoid zero char selections
			if (m_GotSelection && m_StartSelection == m_EndSelection) {
				m_GotSelection = false;
				return;
			}

			// Update the selection coordinates
			int StartSel = std::min(m_StartSelection, m_EndSelection);
			int EndSel = std::max(m_StartSelection, m_EndSelection);

			m_SelectionX = StartSel - m_StartIndex;
			m_SelectionX = std::max(m_SelectionX, 0);
			int temp = m_SelectionX;

			m_SelectionWidth = (EndSel - m_StartIndex) - m_SelectionX;

			m_SelectionX = m_Font->CalculateWidth(m_Text.substr(m_StartIndex, m_SelectionX));
			m_SelectionWidth = m_Font->CalculateWidth(m_Text.substr(m_StartIndex + temp, m_SelectionWidth));

			m_SelectionX = std::max(m_SelectionX, 0);
			m_SelectionWidth = std::min(m_SelectionWidth, m_Width);

			UpdateText(false, false);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		AddEvent(GUIEventType::Notification, GUIEventCode::MouseDown);

		if (m_ReadOnly) {
			return;
		}

		// Give me focus
		SetFocus();
		CaptureMouse();

		if (!(Buttons & GUIControl::MouseButtons::MOUSE_LEFT)) {
			return;
		}

		int OldIndex = m_CursorIndex;

		// Set the cursor
		std::string Text = m_Text.substr(m_StartIndex, m_Text.size() - m_StartIndex);
		m_CursorIndex = m_Text.size();

		if (Modifier != GUIInput::KeyModifiers::ModShift) { m_GotSelection = false; }

		// Go through each character until we to the mouse point
		int TX = m_PosX;
		for (int i = 0; i < Text.size(); i++) {
			TX += m_Font->CalculateWidth(Text.at(i));
			if (TX > X) {
				m_CursorIndex = i + m_StartIndex;
				break;
			}
		}

		// Do a selection if holding the shift button
		if (Modifier == GUIInput::KeyModifiers::ModShift)
			SetSelection(OldIndex, m_CursorIndex);

		// Update the text
		UpdateText(false, false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		ReleaseMouse();
		AddEvent(GUIEventType::Notification, GUIEventCode::Clicked);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::OnMouseMove(int X, int Y, int Buttons, int Modifier) {
		if (!(Buttons & GUIControl::MouseButtons::MOUSE_LEFT) || !m_Captured) {
			return;
		}

		// Select from the mouse down point to where the mouse is currently
		std::string Text = m_Text.substr(m_StartIndex, m_Text.size() - m_StartIndex);
		int TX = m_PosX;
		for (int i = 0; i < Text.size(); i++) {
			TX += m_Font->CalculateWidth(Text.at(i));
			if (TX >= X) {
				SetSelection(m_CursorIndex, i + m_StartIndex);
				m_CursorIndex = i + m_StartIndex;
				UpdateText(false, true);
				break;
			}
		}

		// Double check for the mouse at the end of the text
		if (X > TX) {
			SetSelection(m_CursorIndex, m_Text.size());
			m_CursorIndex = m_Text.size();
			UpdateText(false, true);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::OnKeyPress(int KeyCode, int Modifier) {
		if (m_ReadOnly) {
			return;
		}

		bool modShift = Modifier == GUIInput::KeyModifiers::ModShift;
		bool modCtrl = Modifier == GUIInput::KeyModifiers::ModCtrl;

		// To convert to allegro's crazy scheme with their keyboard function returning the order of the letter when ctrl is pressed
		//int asciiChar = modCtrl ? KeyCode + 96 : KeyCode;

		switch (KeyCode) {
			case GUIInput::KeyboardKeys::KeyBackspace:
				if (m_GotSelection) {
					RemoveSelectionText();
				} else {
					if (m_CursorIndex > 0) {
						int newCursorIndex = modCtrl ? GetStartOfPreviousCharacterGroup(m_Text, m_CursorIndex) : m_CursorIndex - 1;
						m_Text.erase(newCursorIndex, m_CursorIndex - newCursorIndex);
						m_CursorIndex = newCursorIndex;
					}
				}
				UpdateText();
				AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
				return;
			case GUIInput::KeyboardKeys::KeyDelete:
				if (m_GotSelection) {
					RemoveSelectionText();
				} else {
					if (m_CursorIndex < m_Text.size()) {
						int nextCursorIndex = modCtrl ? GetStartOfNextCharacterGroup(m_Text, m_CursorIndex) : m_CursorIndex + 1;
						m_Text.erase(m_CursorIndex, nextCursorIndex - m_CursorIndex);
					}
				}
				UpdateText();
				AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
				return;
			case GUIInput::KeyboardKeys::KeyLeftArrow:
				if (m_CursorIndex > 0) {
					int newCursorIndex = modCtrl ? GetStartOfPreviousCharacterGroup(m_Text, m_CursorIndex) : m_CursorIndex - 1;
					if (modShift) {
						SetSelection(m_CursorIndex, newCursorIndex);
					} else {
						m_GotSelection = false;
					}
					m_CursorIndex = newCursorIndex;
					UpdateText();
				}
				return;
			case GUIInput::KeyboardKeys::KeyRightArrow:
				if (m_CursorIndex < m_Text.size()) {
					int newCursorIndex = modCtrl ? GetStartOfNextCharacterGroup(m_Text, m_CursorIndex) : m_CursorIndex + 1;
					if (modShift) {
						SetSelection(m_CursorIndex, newCursorIndex);
					} else {
						m_GotSelection = false;
					}
					m_CursorIndex = newCursorIndex;
					UpdateText();
				}
				return;
			case GUIInput::KeyboardKeys::KeyHome:
				if (modShift) {
					SetSelection(m_CursorIndex, 0);
				} else {
					m_GotSelection = false;
				}
				m_CursorIndex = 0;
				UpdateText();
				return;
			case GUIInput::KeyboardKeys::KeyEnd:
				if (modShift) {
					SetSelection(m_CursorIndex, m_Text.size());
				} else {
					m_GotSelection = false;
				}
				m_CursorIndex = m_Text.size();
				UpdateText();
				return;
			default:
				if (modCtrl) {
					switch (KeyCode) {
						case 'x':
							if (m_GotSelection) {
								GUIUtil::SetClipboardText(GetSelectionText());
								RemoveSelectionText();
								AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
							}
							return;
						case 'c':
							if (m_GotSelection) { GUIUtil::SetClipboardText(GetSelectionText()); }
							return;
						case 'v':
						{
							RemoveSelectionText();
							std::string Text = GUIUtil::GetClipboardText();
							m_Text.insert(m_CursorIndex, Text);
							m_CursorIndex += Text.size();
							UpdateText(true, true);
							AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
							return;
						}
						case 'a':
							SetSelection(0, m_Text.size());
							UpdateText();
							return;
						default:
							break;
					}
				}

				if (KeyCode == '\n' || KeyCode == '\r') {
					AddEvent(GUIEventType::Notification, GUIEventCode::Enter);
					return;
				}

				int minValidKeyCode = 32;
				int maxValidKeyCode = 126;
				if (m_NumericOnly) {
					minValidKeyCode = 48;
					maxValidKeyCode = 57;
				}
				// Add valid ASCII characters
				if (KeyCode >= minValidKeyCode && KeyCode <= maxValidKeyCode) {
					RemoveSelectionText();
					std::string buf = { static_cast<char>(KeyCode), '\0' };
					if (m_MaxTextLength > 0 && m_Text.length() >= m_MaxTextLength) {
						return;
					}
					m_Text.insert(m_CursorIndex, buf);
					m_CursorIndex++;

					if (m_NumericOnly && m_MaxNumericValue > 0 && std::stoi(m_Text) > m_MaxNumericValue) { m_Text = std::to_string(m_MaxNumericValue); }

					AddEvent(GUIEventType::Notification, GUIEventCode::Changed);
					UpdateText(true);
					return;
				}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Create a new bitmap
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "TextBox", 0, 0, m_Width, m_Height);

		// Setup the skin in the panel too
		std::string Filename;
		Skin->GetValue("TextBox", "Font", &Filename);
		m_Font = Skin->GetFont(Filename);
		Skin->GetValue("TextBox", "FontColor", &m_FontColor);
		Skin->GetValue("TextBox", "FontShadow", &m_FontShadow);
		Skin->GetValue("TextBox", "FontKerning", &m_FontKerning);
		Skin->GetValue("TextBox", "FontSelectColor", &m_FontSelectColor);
		Skin->GetValue("TextBox", "SelectedColorIndex", &m_SelectedColorIndex);

		Skin->GetValue("TextBox", "WidthMargin", &m_WidthMargin);
		Skin->GetValue("TextBox", "HeightMargin", &m_HeightMargin);

		// Convert
		m_FontColor = Skin->ConvertColor(m_FontColor);
		m_FontSelectColor = Skin->ConvertColor(m_FontSelectColor);
		m_SelectedColorIndex = Skin->ConvertColor(m_SelectedColorIndex);

		// Pre-cache the font colors
		m_Font->CacheColor(m_FontColor);
		m_Font->CacheColor(m_FontSelectColor);

		// Get the cursor color
		Skin->GetValue("TextBox", "CursorColorIndex", &m_CursorColor);
		m_CursorColor = Skin->ConvertColor(m_CursorColor);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUITextBox::Draw(GUIScreen *Screen) {
		// Draw the background
		m_DrawBitmap->Draw(Screen->GetBitmap(), m_PosX, m_PosY, nullptr);

		if (!m_Font) {
			return;
		}

		int FontHeight = m_Font->GetFontHeight();


		// Clamp the cursor
		m_CursorX = std::max(m_CursorX, 0);

		std::string Text = m_Text.substr(m_StartIndex);

		// Draw the text
		m_Font->SetColor(m_FontColor);
		m_Font->SetKerning(m_FontKerning);
		m_Font->Draw(Screen->GetBitmap(), m_PosX + m_WidthMargin, m_PosY + m_HeightMargin, Text, m_FontShadow);

		// Draw the right-justified extra text in the background
		m_Font->DrawAligned(Screen->GetBitmap(), m_PosX + m_Width - m_WidthMargin, m_PosY + m_HeightMargin, m_RightText, GUIFont::HAlignment::Right, GUIFont::VAlignment::Top, m_Width, m_FontShadow);

		// Draw the selected text
		if (m_GotSelection && m_GotFocus && !m_Text.empty()) {
			// Draw selection mark
			Screen->GetBitmap()->DrawRectangle(m_PosX + m_WidthMargin + m_SelectionX, m_PosY + m_HeightMargin + 2, m_SelectionWidth, FontHeight - 3, m_SelectedColorIndex, true);
			// Draw text with selection regions in different color
			m_Font->SetColor(m_FontSelectColor);
			int Start = std::min(m_StartSelection, m_EndSelection);
			int End = std::max(m_StartSelection, m_EndSelection);

			// Selection
			if (m_StartIndex > Start) { Start = m_StartIndex; }
			m_Font->Draw(Screen->GetBitmap(), m_PosX + m_WidthMargin + m_SelectionX, m_PosY + m_HeightMargin, Text.substr(Start - m_StartIndex, End - Start));
		}


		// If we have focus, draw the cursor with hacky blink
		if (m_GotFocus && (m_CursorBlinkCount++ % 30 > 15)) { Screen->GetBitmap()->DrawRectangle(m_PosX + m_CursorX + 2, m_PosY + m_HeightMargin + m_CursorY + 2, 1, FontHeight - 3, m_CursorColor, true); }
	}
}