#include "GUI.h"

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIEvent::GUIEvent()
{
    m_Control = 0;
    m_Type = 0;
    m_Msg = 0;
    m_Data = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIEvent::GUIEvent(GUIControl *Control, int Type, int Msg, int Data)
{
    assert(Control);
    m_Control = Control;
    m_Type = Type;
    m_Msg = Msg;
    m_Data = Data;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetType(void)
{
    return m_Type;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIControl *GUIEvent::GetControl(void)
{
    return m_Control;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetMsg(void)
{
    return m_Msg;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIEvent::GetData(void)
{
    return m_Data;
}
