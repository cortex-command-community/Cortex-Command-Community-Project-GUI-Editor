#ifndef _GUIINPUT_
#define _GUIINPUT_

#include "GUIConstants.h"

namespace RTE {

	/// <summary>
	/// An interface class inherited by the different types of input methods.
	/// </summary>
	class GUIInput {

	public:

		/// <summary>
		/// Mouse & Key events.
		/// </summary>
		enum InputEvents { None, Released, Pushed, Repeat };

		/// <summary>
		/// Mouse & Key states
		/// </summary>
		enum InputStates { Up, Down };

		/// <summary>
		/// 
		/// </summary>
		enum MouseButtons {
			ButtonNone = -1,
			ButtonLeft,
			ButtonMiddle,
			ButtonRight,
			MouseButtonCount
		};

		/// <summary>
		/// Extra keys
		/// </summary>
		enum KeyboardKeys {
			KeyNone = 0,
			KeyBackspace = 0x08,
			KeyTab = 0x09,
			KeyEnter = 0x0D,
			KeyEscape = 0x1B,
			KeySpace = 0x20,
			KeyLeftArrow = 0x86,
			KeyRightArrow = 0x87,
			KeyUpArrow = 0x88,
			KeyDownArrow = 0x89,
			KeyInsert = 0x95,
			KeyDelete = 0x96,
			KeyHome = 0x97,
			KeyEnd = 0x98,
			KeyPageUp = 0x99,
			KeyPageDown = 0x9A
		};

		/// <summary>
		/// Modifiers
		/// </summary>
		enum KeyModifiers { ModNone, ModCtrl, ModShift, ModAlt, ModCtrlShift, ModCtrlAlt, ModAltShift, ModCtrlShiftAlt };

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIInput object in system memory.
		/// </summary>
		/// <param name="keyJoyMouseCursor">Whether the keyboard and joysticks also can control the mouse cursor.</param>
		explicit GUIInput(bool keyJoyMouseCursor = false) : m_KeyJoyMouseCursor(keyJoyMouseCursor) {}
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		virtual ~GUIInput() = default;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Sets whether the keyboard and joysticks also control the mouse.
		/// </summary>
		/// <param name="enableKeyJoyMouseCursor">Whether the keyboard and joysticks also control the mouse or not.</param>
		void SetKeyJoyMouseCursor(bool enableKeyJoyMouseCursor) { m_KeyJoyMouseCursor = enableKeyJoyMouseCursor; }
#pragma endregion

#pragma region Keyboard Handling
		/// <summary>
		/// Copies the keyboard buffer into an array. The keyboard buffer is ordered by ASCII code and each entry contains a GUInput::Event enum state.
		/// </summary>
		const std::array<uint8_t, c_KeyboardBufferSize> & GetKeyboardBuffer() const { return m_KeyboardBuffer; }

		/// <summary>
		/// Gets the key modifiers.
		/// </summary>
		/// <returns></returns>
		int GetKeyModifier() const { return m_KeyModifier; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="ascii"></param>
		/// <returns></returns>
		uint8_t GetAsciiState(uint8_t ascii) const { return m_KeyboardBuffer.at(ascii); }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="scanCode"></param>
		/// <returns></returns>
		uint8_t GetScanCodeState(uint8_t scanCode) const { return m_ScanCodeState.at(scanCode); }
#pragma endregion

#pragma region Mouse Handling
		/// <summary>
		/// Gets the mouse position.
		/// </summary>
		/// <param name="mousePosX">Pointers to store the X and Y coordinates in.</param>
		/// <param name="mousePosY"></param>
		void GetMousePosition(int *mousePosX, int *mousePosY) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetMousePosX() const { return m_MouseX + m_MouseOffsetX; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetMousePosY() const { return m_MouseY + m_MouseOffsetY; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetPrevMousePosX() const { return m_PreviousMouseX + m_MouseOffsetX; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetPrevMousePosY() const { return m_PreviousMouseY + m_MouseOffsetY; }

		/// <summary>
		/// 
		/// </summary>
		void SetCurrentMousePosAsPrev() { m_PreviousMouseX = m_MouseX; m_PreviousMouseY = m_MouseY; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetMouseMovementX() const { return m_MouseX - m_PreviousMouseX; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int GetMouseMovementY() const { return m_MouseY - m_PreviousMouseY; }

		/// <summary>
		/// Copies the mouse button states into an array.
		/// </summary>
		/// <param name="eventsArray"></param>
		/// <param name="statesArray">State array.</param>
		void GetMouseButtons(int *eventsArray, int *statesArray) const;

		/// <summary>
		/// This function returns how much the mouse scroll wheel has moved. Positive integer is scroll up, negative is scroll down.
		/// </summary>
		/// <returns>Mouse scroll wheel movement in integer value.</returns>
		int GetMouseWheelChange() const { return m_MouseWheelChange; }

		/// <summary>
		/// Sets the offset for the mouse input to be adjusted by.
		/// </summary>
		/// <param name="mouseOffsetX">The offset from the GUI upper left corner to the screen upper left corner.</param>
		/// <param name="mouseOffsetY"></param>
		void GetMouseOffset(int &mouseOffsetX, int &mouseOffsetY) const { mouseOffsetX = m_MouseOffsetX; mouseOffsetY = m_MouseOffsetY; }

		/// <summary>
		/// Sets the offset for the mouse input to be adjusted by. This should be used when the GUI is being drawn somewhere else on the screen than the upper left corner.
		/// </summary>
		/// <param name="mouseOffsetX">The new offset from the GUI upper left corner to the screen upper left corner.</param>
		/// <param name="mouseOffsetY"></param>
		void SetMouseOffset(int mouseOffsetX, int mouseOffsetY) { m_MouseOffsetX = mouseOffsetX; m_MouseOffsetY = mouseOffsetY; }

		/// <summary>
		/// Checks if the mouse point is inside a rectangle.
		/// </summary>
		/// <param name="rect">Rectangle</param>
		/// <param name="mousePosX">Mouse position.</param>
		/// <param name="mousePosY"></param>
		/// <returns></returns>
		bool MouseIsInsideRect(const GUIRect *rect) const;
#pragma endregion

#pragma region Virtual Methods
		/// <summary>
		/// Updates the Input.
		/// </summary>
		virtual void Update() {}
#pragma endregion

	protected:

		// Keyboard buffer holding the key states
		std::array<uint8_t, c_KeyboardBufferSize> m_KeyboardBuffer = {};
		std::array<uint8_t, c_KeyboardBufferSize> m_ScanCodeState = {};

		// Mouse button states. Order: Left, Middle, Right
		std::array<int, MouseButtons::MouseButtonCount> m_MouseButtonEvents = {};
		std::array<int, MouseButtons::MouseButtonCount> m_MouseButtonStates = {};

		int m_MouseX = 0;
		int m_MouseY = 0;
		int m_PreviousMouseX = 0;
		int m_PreviousMouseY = 0;
		// These offset the mouse positions so that the cursor is shifted for all events
		int m_MouseOffsetX = 0;
		int m_MouseOffsetY = 0;

		int	m_MouseWheelChange = 0; //!< the amount and direction that the mouse wheel has moved.

		int m_KeyModifier = KeyModifiers::ModNone;

		bool m_KeyJoyMouseCursor = false; //!< Whether the keyboard and joysticks also control the mouse.

	private:

		// Disallow the use of some implicit methods.
		GUIInput(const GUIInput &reference) = delete;
		GUIInput & operator=(const GUIInput &rhs) = delete;
	};
};
#endif