#ifndef _GUIEVENT_
#define _GUIEVENT_

namespace RTE {

	/// <summary>
	/// A class to hold event information.
	/// </summary>
	class GUIEvent {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIEvent object in system memory.
		/// </summary>
		GUIEvent() = default;

		/// <summary>
		/// Constructor method used to instantiate a GUIEvent object in system memory.
		/// </summary>
		/// <param name="eventOrigin">Control.</param>
		/// <param name="eventType">Event type.</param>
		/// <param name="eventCode">Msg.</param>
		/// <param name="eventData">Data.</param>
		GUIEvent(GUIControlBase *eventOrigin, GUIEventType eventType, GUIEventCode eventCode, int eventData = 0) : m_Origin(eventOrigin), m_Type(eventType), m_Code(eventCode), m_Data(eventData) {}
#pragma endregion

#pragma region Getters
		/// <summary>
		/// Gets the event control.
		/// </summary>
		/// <returns></returns>
		GUIControlBase * GetOrigin() const { return m_Origin; }

		/// <summary>
		/// Gets the event type.
		/// </summary>
		/// <returns></returns>
		GUIEventType GetType() const { return m_Type; }

		/// <summary>
		/// Gets the msg.
		/// </summary>
		/// <returns></returns>
		GUIEventCode GetMsg() const { return m_Code; }

		/// <summary>
		/// Gets the data.
		/// </summary>
		/// <returns></returns>
		int GetData() const { return m_Data; }
#pragma endregion

	private:

		GUIControlBase *m_Origin = nullptr;
		GUIEventType m_Type = GUIEventType::NoEvent;
		GUIEventCode m_Code = GUIEventCode::NoNotification;
		int m_Data = 0;
	};
};
#endif