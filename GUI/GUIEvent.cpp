#include "GUI.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIEvent::GUIEvent() {
		m_Control = nullptr;
		m_Type = GUIEventType::NoEvent;
		m_Msg = GUIEventCode::NoNotification;
		m_Data = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIEvent::GUIEvent(GUIControl *Control, GUIEventType Type, GUIEventCode Msg, int Data) {
		GUIAssert(Control, "");
		m_Control = Control;
		m_Type = Type;
		m_Msg = Msg;
		m_Data = Data;
	}
}