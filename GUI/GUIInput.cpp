#include "GUI.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::GetMouseButtons(int *eventsArray, int *statesArray) const {
		if (eventsArray) { memcpy(eventsArray, m_MouseButtonEvents.data(), sizeof(int) * MouseButtons::MouseButtonCount); }
		if (statesArray) { memcpy(statesArray, m_MouseButtonStates.data(), sizeof(int) * MouseButtons::MouseButtonCount); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIInput::GetMousePosition(int *mousePosX, int *mousePosY) const {
		if (mousePosX) { *mousePosX = (m_MouseX + m_MouseOffsetX); }
		if (mousePosY) { *mousePosY = (m_MouseY + m_MouseOffsetY); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIInput::MouseIsInsideRect(const GUIRect *rect) const {
		if (rect) {
			int mousePosX = GetMousePosX();
			int mousePosY = GetMousePosY();
			if ((mousePosX >= rect->left) && (mousePosX <= rect->right) && (mousePosY >= rect->top) && (mousePosY <= rect->bottom)) {
				return true;
			}
		}
		return false;
	}
}