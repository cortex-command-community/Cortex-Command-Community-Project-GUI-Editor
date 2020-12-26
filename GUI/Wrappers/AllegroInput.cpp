#include "GUI.h"
#include "AllegroInput.h"
#include "Timer.h"

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AllegroInput::AllegroInput(int whichPlayer, bool keyJoyMouseCursor) : GUIInput(whichPlayer, keyJoyMouseCursor) {
    install_keyboard();
    install_mouse();

    memset(m_ModStates, 0, sizeof(int) * 6);
    memset(m_KeyboardBuffer, 0, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE);
	memset(m_ScanCodeState, 0, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE);

    m_KeyDelay = 0.300F;        // 0.3 second delay
    m_KeyRepeat = 1.0F / 30.0F;   // 30 repetitions per second

    // Setup the keyboard timing
	for (int i = 0; i < 256; i++) {
		m_KeyboardTimes[i] = -1;
	}

    // Create the timers
    m_pTimer = new Timer();
    m_pCursorAccelTimer = new Timer();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AllegroInput::~AllegroInput() {
    delete m_pTimer;
	m_pTimer = nullptr;
    delete m_pCursorAccelTimer;
	m_pCursorAccelTimer = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllegroInput::Update() {
	// Clear the keyboard buffer, we need it to check for changes
	memset(m_KeyboardBuffer, 0, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE);
	memset(m_ScanCodeState, 0, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE);
	while (keypressed()) {
		uint32_t rawkey = readkey();
		uint8_t ascii = rawkey & 0xff;
		uint8_t scancode = (rawkey >> 8);

		m_ScanCodeState[scancode] = Pushed;
		m_KeyboardBuffer[ascii] = Pushed;
	}

	float elapsedS = m_pTimer->GetElapsedRealTimeS();
	m_pTimer->Reset();

	// Special keys
	ConvertKeyEvent(KEY_SPACE, ' ', elapsedS);
	ConvertKeyEvent(KEY_BACKSPACE, Key_Backspace, elapsedS);
	ConvertKeyEvent(KEY_TAB, Key_Tab, elapsedS);
	ConvertKeyEvent(KEY_ENTER, Key_Enter, elapsedS);
	ConvertKeyEvent(KEY_ESC, Key_Escape, elapsedS);
	ConvertKeyEvent(KEY_LEFT, Key_LeftArrow, elapsedS);
	ConvertKeyEvent(KEY_RIGHT, Key_RightArrow, elapsedS);
	ConvertKeyEvent(KEY_UP, Key_UpArrow, elapsedS);
	ConvertKeyEvent(KEY_DOWN, Key_DownArrow, elapsedS);
	ConvertKeyEvent(KEY_INSERT, Key_Insert, elapsedS);
	ConvertKeyEvent(KEY_DEL, Key_Delete, elapsedS);
	ConvertKeyEvent(KEY_HOME, Key_Home, elapsedS);
	ConvertKeyEvent(KEY_END, Key_End, elapsedS);
	ConvertKeyEvent(KEY_PGUP, Key_PageUp, elapsedS);
	ConvertKeyEvent(KEY_PGDN, Key_PageDown, elapsedS);

	// Modifiers
	m_Modifier = ModNone;
	if (key_shifts & KB_SHIFT_FLAG) { m_Modifier |= ModShift; }
	if (key_shifts & KB_ALT_FLAG) { m_Modifier |= ModAlt; }
	if (key_shifts & KB_CTRL_FLAG) { m_Modifier |= ModCtrl; }
	if (key_shifts & KB_COMMAND_FLAG) { m_Modifier |= ModCommand; }

	if (mouse_needs_poll()) { poll_mouse(); }

	m_LastFrameMouseX = mouse_x;
	m_LastFrameMouseY = mouse_y;

	// Update the mouse button events
	m_MouseButtonsEvents[0] = mouse_b & AMBLEFT ? (m_MouseButtonsStates[0] == Up ? Pushed : Repeat) : (m_MouseButtonsStates[0] == Down ? Released : None);
	m_MouseButtonsEvents[1] = mouse_b & AMBMIDDLE ? (m_MouseButtonsStates[1] == Up ? Pushed : Repeat) : (m_MouseButtonsStates[1] == Down ? Released : None);
	m_MouseButtonsEvents[2] = mouse_b & AMBRIGHT ? (m_MouseButtonsStates[2] == Up ? Pushed : Repeat) : (m_MouseButtonsStates[2] == Down ? Released : None);

	m_MouseButtonsStates[0] = mouse_b & AMBLEFT ? Down : Up;
	m_MouseButtonsStates[1] = mouse_b & AMBMIDDLE ? Down : Up;
	m_MouseButtonsStates[2] = mouse_b & AMBRIGHT ? Down : Up;

	m_MouseX = mouse_x;
	m_MouseY = mouse_y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllegroInput::ConvertKeyEvent(int allegroKey, int guilibKey, float elapsedS) {
	if (key[allegroKey]) {
		if (m_KeyboardTimes[guilibKey] < 0) {
			m_KeyboardBuffer[guilibKey] = Pushed;
			m_KeyboardTimes[guilibKey] = 0;
		} else if (m_KeyboardTimes[guilibKey] < m_KeyDelay) {
			m_KeyboardBuffer[guilibKey] = None;
		} else {
			m_KeyboardBuffer[guilibKey] = Repeat;
			m_KeyboardTimes[guilibKey] = 0;
		}
		m_KeyboardTimes[guilibKey] += elapsedS;
	} else {
		if (m_KeyboardTimes[guilibKey] >= 0) {
			m_KeyboardBuffer[guilibKey] = Released;
		} else {
			m_KeyboardBuffer[guilibKey] = None;
		}
		m_KeyboardTimes[guilibKey] = -1;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AllegroInput::CreateTimer() {
	m_pTimer->Reset();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float AllegroInput::GetTime() {
	return (float)m_pTimer->GetElapsedRealTimeS();
}