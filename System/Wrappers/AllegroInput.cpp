#include "GUI.h"
#include "AllegroInput.h"
#include "Timer.h"

#ifndef GUI_STANDALONE
#include "FrameMan.h"
#include "UInputMan.h"
#endif

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	AllegroInput::AllegroInput(bool keyJoyMouseCursor) : GUIInput(keyJoyMouseCursor) {
		install_keyboard();
		install_mouse();

#ifndef GUI_STANDALONE
		set_mouse_range(0, 0, (g_FrameMan.GetResX() * g_FrameMan.GetResMultiplier()) - 3, (g_FrameMan.GetResY() * g_FrameMan.GetResMultiplier()) - 3);
		AdjustMouseMovementSpeedToGraphicsDriver(g_FrameMan.GetGraphicsDriver());
#endif

		m_KeyTimer = std::make_unique<Timer>();
		m_CursorAccelTimer = std::make_unique<Timer>();

		m_KeyHoldDuration.fill(-1);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::AdjustMouseMovementSpeedToGraphicsDriver(int graphicsDriver) const {
#ifndef GUI_STANDALONE
		if (graphicsDriver == GFX_AUTODETECT_WINDOWED || graphicsDriver == GFX_DIRECTX_WIN_BORDERLESS) {
			set_mouse_speed(2, 2);
		} else {
			set_mouse_speed(1, 1);
		}
#else
		set_mouse_speed(2, 2);
#endif
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::ConvertKeyEvent(int allegroKey, int guiKey, float elapsedS) {
		if (key[allegroKey]) {
			if (m_KeyHoldDuration.at(guiKey) < 0) {
				m_KeyboardBuffer[guiKey] = GUIInput::InputEvents::Pushed;
				m_KeyHoldDuration.at(guiKey) = 0;
			} else if (m_KeyHoldDuration.at(guiKey) < m_KeyRepeatDelay) {
				m_KeyboardBuffer[guiKey] = GUIInput::InputEvents::None;
			} else {
				m_KeyboardBuffer[guiKey] = GUIInput::InputEvents::Repeat;
				m_KeyHoldDuration.at(guiKey) = 0;
			}
			m_KeyHoldDuration.at(guiKey) += elapsedS;
		} else {
			if (m_KeyHoldDuration.at(guiKey) >= 0) {
				m_KeyboardBuffer[guiKey] = GUIInput::InputEvents::Released;
			} else {
				m_KeyboardBuffer[guiKey] = GUIInput::InputEvents::None;
			}
			m_KeyHoldDuration.at(guiKey) = -1;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::Update() {
		float keyElapsedTime = static_cast<float>(m_KeyTimer->GetElapsedRealTimeS());
		m_KeyTimer->Reset();

		UpdateKeyboardInput(keyElapsedTime);
		UpdateMouseInput();

		// If joysticks and keyboard can control the mouse cursor too
		if (m_KeyJoyMouseCursor) { UpdateKeyJoyMouseInput(keyElapsedTime); }

		// Update the mouse position of this GUIInput, based on the Allegro mouse vars (which may have been altered by joystick or keyboard input)
#ifndef GUI_STANDALONE
		m_MouseX = static_cast<int>(static_cast<float>(mouse_x) / static_cast<float>(g_FrameMan.GetResMultiplier()));
		m_MouseY = static_cast<int>(static_cast<float>(mouse_y) / static_cast<float>(g_FrameMan.GetResMultiplier()));
#else
		m_MouseX = mouse_x;
		m_MouseY = mouse_y;
#endif
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::UpdateKeyboardInput(float keyElapsedTime) {
		// Clear the keyboard buffer, we need it to check for changes
		m_KeyboardBuffer.fill(0);
		m_ScanCodeState.fill(0);

		while (keypressed()) {
			int rawkey = readkey();
			uint8_t ascii = (rawkey & 0xFF);
			uint8_t scancode = static_cast<uint8_t>(rawkey >> 8);

			m_ScanCodeState.at(scancode) = GUIInput::InputEvents::Pushed;
			m_KeyboardBuffer.at(ascii) = GUIInput::InputEvents::Pushed;
		}

		ConvertKeyEvent(KEY_SPACE, GUIInput::KeyboardKeys::KeySpace, keyElapsedTime);
		ConvertKeyEvent(KEY_BACKSPACE, GUIInput::KeyboardKeys::KeyBackspace, keyElapsedTime);
		ConvertKeyEvent(KEY_TAB, GUIInput::KeyboardKeys::KeyTab, keyElapsedTime);
		ConvertKeyEvent(KEY_ENTER, GUIInput::KeyboardKeys::KeyEnter, keyElapsedTime);
		ConvertKeyEvent(KEY_ESC, GUIInput::KeyboardKeys::KeyEscape, keyElapsedTime);
		ConvertKeyEvent(KEY_LEFT, GUIInput::KeyboardKeys::KeyLeftArrow, keyElapsedTime);
		ConvertKeyEvent(KEY_RIGHT, GUIInput::KeyboardKeys::KeyRightArrow, keyElapsedTime);
		ConvertKeyEvent(KEY_UP, GUIInput::KeyboardKeys::KeyUpArrow, keyElapsedTime);
		ConvertKeyEvent(KEY_DOWN, GUIInput::KeyboardKeys::KeyDownArrow, keyElapsedTime);
		ConvertKeyEvent(KEY_INSERT, GUIInput::KeyboardKeys::KeyInsert, keyElapsedTime);
		ConvertKeyEvent(KEY_DEL, GUIInput::KeyboardKeys::KeyDelete, keyElapsedTime);
		ConvertKeyEvent(KEY_HOME, GUIInput::KeyboardKeys::KeyHome, keyElapsedTime);
		ConvertKeyEvent(KEY_END, GUIInput::KeyboardKeys::KeyEnd, keyElapsedTime);
		ConvertKeyEvent(KEY_PGUP, GUIInput::KeyboardKeys::KeyPageUp, keyElapsedTime);
		ConvertKeyEvent(KEY_PGDN, GUIInput::KeyboardKeys::KeyPageDown, keyElapsedTime);

		m_KeyModifier = GUIInput::KeyModifiers::ModNone;

#ifndef GUI_STANDALONE
		if (key_shifts & KB_CTRL_FLAG) {
			m_KeyModifier = GUIInput::KeyModifiers::ModCtrl;
		} else if (key_shifts & KB_SHIFT_FLAG) {
			m_KeyModifier = GUIInput::KeyModifiers::ModShift;
		} else if (key_shifts & KB_ALT_FLAG) {
			m_KeyModifier = GUIInput::KeyModifiers::ModAlt;
		}
		if (m_KeyModifier != GUIInput::KeyModifiers::ModNone) {
			if ((key_shifts & KB_CTRL_FLAG) && (key_shifts & KB_SHIFT_FLAG) && (key_shifts & KB_ALT_FLAG)) {
				m_KeyModifier = GUIInput::KeyModifiers::ModCtrlShiftAlt;
			} else {
				if ((key_shifts & KB_CTRL_FLAG) && (key_shifts & KB_SHIFT_FLAG)) {
					m_KeyModifier = GUIInput::KeyModifiers::ModCtrlShift;
				} else if ((key_shifts & KB_CTRL_FLAG) && (key_shifts & KB_ALT_FLAG)) {
					m_KeyModifier = GUIInput::KeyModifiers::ModCtrlAlt;
				}
			}
		}
#else
		if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) {
			m_KeyModifier = GUIInput::KeyModifiers::ModCtrl;
		} else if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
			m_KeyModifier = GUIInput::KeyModifiers::ModShift;
		} else if (key[KEY_ALT]) {
			m_KeyModifier = GUIInput::KeyModifiers::ModAlt;
		}
		if (m_KeyModifier != GUIInput::KeyModifiers::ModNone) {
			if ((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && (key[KEY_LSHIFT] || key[KEY_RSHIFT]) && key[KEY_ALT]) {
				m_KeyModifier = GUIInput::KeyModifiers::ModCtrlShiftAlt;
			} else {
				if ((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && (key[KEY_LSHIFT] || key[KEY_RSHIFT])) {
					m_KeyModifier = GUIInput::KeyModifiers::ModCtrlShift;
				} else if ((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_ALT]) {
					m_KeyModifier = GUIInput::KeyModifiers::ModCtrlAlt;
				}
			}
		}
#endif
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::UpdateMouseInput() {
		if (mouse_needs_poll()) { poll_mouse(); }

#ifndef GUI_STANDALONE
		m_MouseWheelChange = g_UInputMan.MouseWheelMoved();
#else
		m_MouseWheelChange = mouse_z;
		position_mouse_z(0);
#endif

		if (!m_KeyJoyMouseCursor) {
			if (mouse_b & AllegroMouseButtons::ButtonLeft) {
				m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] == GUIInput::InputStates::Up) ? GUIInput::InputEvents::Pushed : GUIInput::InputEvents::Repeat;
				m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Down;
			} else {
				m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] == GUIInput::InputStates::Down) ? GUIInput::InputEvents::Released : GUIInput::InputEvents::None;
				m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Up;
			}
		}
		if (mouse_b & AllegroMouseButtons::ButtonMiddle) {
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonMiddle] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonMiddle] == GUIInput::InputStates::Up) ? GUIInput::InputEvents::Pushed : GUIInput::InputEvents::Repeat;
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonMiddle] = GUIInput::InputStates::Down;
		} else {
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonMiddle] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonMiddle] == GUIInput::InputStates::Down) ? GUIInput::InputEvents::Released : GUIInput::InputEvents::None;
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonMiddle] = GUIInput::InputStates::Up;
		}
		if (mouse_b & AllegroMouseButtons::ButtonRight) {
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonRight] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonRight] == GUIInput::InputStates::Up) ? GUIInput::InputEvents::Pushed : GUIInput::InputEvents::Repeat;
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonRight] = GUIInput::InputStates::Down;
		} else {
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonRight] = (m_MouseButtonStates[GUIInput::MouseButtons::ButtonRight] == GUIInput::InputStates::Down) ? GUIInput::InputEvents::Released : GUIInput::InputEvents::None;
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonRight] = GUIInput::InputStates::Up;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllegroInput::UpdateKeyJoyMouseInput(float keyElapsedTime) {
#ifndef GUI_STANDALONE
		int mouseDenominator = g_FrameMan.GetResMultiplier();
		Vector joyKeyDirectional = g_UInputMan.GetMenuDirectional() * 5;

		// See how much to accelerate the joystick input based on how long the stick has been pushed around
		if (joyKeyDirectional.GetMagnitude() < 0.95F) { m_CursorAccelTimer->Reset(); }

		float acceleration = 0.25F + static_cast<float>(std::min(m_CursorAccelTimer->GetElapsedRealTimeS(), 0.5)) * 20.0F;

		// Manipulate the mouse position with the joysticks or keys
		mouse_x = mouse_x + static_cast<volatile int>(joyKeyDirectional.GetX() * static_cast<float>(mouseDenominator) * keyElapsedTime * 15.0F * acceleration);
		mouse_y = mouse_y + static_cast<volatile int>(joyKeyDirectional.GetY() * static_cast<float>(mouseDenominator) * keyElapsedTime * 15.0F * acceleration);
		// Prevent mouse from flying out of the screen
		mouse_x = std::max(0, static_cast<int>(mouse_x));
		mouse_y = std::max(0, static_cast<int>(mouse_y));
		// Pull in a bit so cursor doesn't completely disappear
		mouse_x = std::min((g_FrameMan.GetResX() * mouseDenominator) - 3, static_cast<int>(mouse_x));
		mouse_y = std::min((g_FrameMan.GetResY() * mouseDenominator) - 3, static_cast<int>(mouse_y));

		// Button states/presses, Primary - ACTUALLY make either button work, we don't have use for secondary in menus
		if (g_UInputMan.MenuButtonHeld(UInputMan::MenuCursorButtons::MENU_EITHER)) {
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Down;
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputEvents::Repeat;
		}
		if (g_UInputMan.MenuButtonPressed(UInputMan::MenuCursorButtons::MENU_EITHER)) {
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Down;
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputEvents::Pushed;
		} else if (g_UInputMan.MenuButtonReleased(UInputMan::MenuCursorButtons::MENU_EITHER)) {
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Up;
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputEvents::Released;
		} else if (m_MouseButtonEvents[0] == Released) {
			m_MouseButtonStates[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputStates::Up;
			m_MouseButtonEvents[GUIInput::MouseButtons::ButtonLeft] = GUIInput::InputEvents::None;
		}
#endif
	}
}