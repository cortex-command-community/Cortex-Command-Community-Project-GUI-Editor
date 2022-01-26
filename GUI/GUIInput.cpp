#include "GUI.h"

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIInput::GUIInput(int whichPlayer, bool keyJoyMouseCursor) {
	// Clear all the states
	memset(m_KeyboardBuffer, 0, sizeof(unsigned char) * KEYBOARD_BUFFER_SIZE);
	memset(m_ScanCodeState, 0, sizeof(unsigned char) * KEYBOARD_BUFFER_SIZE);
	memset(m_MouseButtonsEvents, 0, sizeof(int) * 3);
	memset(m_MouseButtonsStates, 0, sizeof(int) * 3);

	m_MouseX = 0;
	m_MouseY = 0;
	m_LastFrameMouseX = 0;
	m_LastFrameMouseY = 0;

	m_MouseOffsetX = 0;
	m_MouseOffsetY = 0;
	m_Modifier = ModNone;

	m_KeyJoyMouseCursor = keyJoyMouseCursor;

	m_Player = whichPlayer;

	m_MouseWheelChange = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIInput::GetKeyboard(unsigned char *Buffer) const {
	if (Buffer) { memcpy(Buffer, m_KeyboardBuffer, sizeof(unsigned char) * KEYBOARD_BUFFER_SIZE); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIInput::GetMouseButtons(int *Buttons, int *States) const {
	if (Buttons) { memcpy(Buttons, m_MouseButtonsEvents, sizeof(int) * 3); }
	if (States) { memcpy(States, m_MouseButtonsStates, sizeof(int) * 3); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIInput::GetMousePosition(int *X, int *Y) const {
	if (X) { *X = (m_MouseX + m_MouseOffsetX); }
	if (Y) { *Y = (m_MouseY + m_MouseOffsetY); }
}