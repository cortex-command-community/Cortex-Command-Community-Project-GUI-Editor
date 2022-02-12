#ifndef _RTEGUICONSTANTS_
#define _RTEGUICONSTANTS_

namespace RTE {

#pragma region
	enum MouseCursorType {
		Pointer,
		Text,
		HorizSize,
		MouseCursorTypeCount
	};
#pragma endregion

#pragma region Input Constants
	static constexpr int c_KeyboardBufferSize = 256;
#pragma endregion

#pragma region Event Constants
	/// <summary>
	/// Event Types
	/// </summary>
	enum class GUIEventType {
		NoEvent,
		Command,
		Notification
	};

	enum class GUIEventCode {
		NoNotification,
		Clicked,
		Pushed,
		UnPushed,
		Focused,
		Enter, // A text panel has lost focus or the enter key was hit
		Changed, // Any text panel has changed. Property values are NOT updated
		Dropped, // When listpanel has dropped
		Closed, // When listpanel has closed
		Hovered,
		Click, // Mouse click. Not just inside the panel
		MouseDown, // Mouse down inside the panel
		MouseUp, // Mouse up inside the panel
		Select, // Selected (on mouse down) an item
		MouseMove, // Mouse moved over the panel
		MouseEnter, // Mouse left the panel
		MouseLeave, // Mouse left the panel
		DoubleClick, // Double click
		KeyDown, // Key Down
		EdgeHit, // Tried scrolling the selection past the first or last item. data = 0 for top edge, data = 1 for bottom edge.
		ChangeValue,
		Grab,
		Release
	};
#pragma endregion
}
#endif