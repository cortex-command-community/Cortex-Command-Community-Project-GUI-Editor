#include "GUI.h"

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIEvent::GUIEvent() {
	m_Control = 0;
	m_Type = 0;
	m_Msg = 0;
	m_Data = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIEvent::GUIEvent(GUIControl *Control, int Type, int Msg, int Data) {
	assert(Control);
	m_Control = Control;
	m_Type = Type;
	m_Msg = Msg;
	m_Data = Data;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetType() {
	return m_Type;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIControl *GUIEvent::GetControl() {
	return m_Control;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetMsg() {
	return m_Msg;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetData() {
	return m_Data;
}
