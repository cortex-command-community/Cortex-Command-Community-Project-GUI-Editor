#include "GUI.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::Clear() {
		m_MouseX = 0;
		m_MouseY = 0;
		m_PreviousMouseX = 0;
		m_PreviousMouseY = 0;
		m_MouseOffsetX = 0;
		m_MouseOffsetY = 0;
		m_MouseWheelChange = 0;
		m_KeyModifier = KeyModifiers::ModNone;
		m_KeyJoyMouseCursor = false;

		memset(m_KeyboardBuffer, 0, sizeof(unsigned char) * InputConstants::KeyboardBufferSize);
		memset(m_ScanCodeState, 0, sizeof(unsigned char) * InputConstants::KeyboardBufferSize);
		memset(m_MouseButtonsEvents, 0, sizeof(int) * InputConstants::MouseButtonCount);
		memset(m_MouseButtonsStates, 0, sizeof(int) * InputConstants::MouseButtonCount);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::GetKeyboardBuffer(unsigned char *bufferArray) const {
		if (bufferArray) { memcpy(bufferArray, m_KeyboardBuffer, sizeof(unsigned char) * InputConstants::KeyboardBufferSize); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::GetMouseButtons(int *eventsArray, int *statesArray) const {
		if (eventsArray) { memcpy(eventsArray, m_MouseButtonsEvents, sizeof(int) * InputConstants::MouseButtonCount); }
		if (statesArray) { memcpy(statesArray, m_MouseButtonsStates, sizeof(int) * InputConstants::MouseButtonCount); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::GetMousePosition(int *mousePosX, int *mousePosY) const {
		if (mousePosX) { *mousePosX = (m_MouseX + m_MouseOffsetX); }
		if (mousePosY) { *mousePosY = (m_MouseY + m_MouseOffsetY); }
	}
}