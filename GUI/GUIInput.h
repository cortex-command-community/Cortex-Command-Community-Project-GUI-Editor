#ifndef _GUIINPUT_
#define _GUIINPUT_

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
		enum MouseButtons { ButtonLeft, ButtonMiddle, ButtonRight };

		/// <summary>
		/// Extra keys
		/// </summary>
		enum KeyboardKeys {
			KeyNone = 0,
			KeyBackspace = 0x00000008,
			KeyTab = 0x00000009,
			KeyEnter = 0x0000000D,
			KeyEscape = 0x0000001B,
			KeyLeftArrow = 0x00000086,
			KeyRightArrow = 0x00000087,
			KeyUpArrow = 0x00000088,
			KeyDownArrow = 0x00000089,
			KeyInsert = 0x00000095,
			KeyDelete = 0x00000096,
			KeyHome = 0x00000097,
			KeyEnd = 0x00000098,
			KeyPageUp = 0x00000099,
			KeyPageDown = 0x0000009A
		};

		/// <summary>
		/// Modifiers
		/// </summary>
		enum KeyModifiers {
			ModNone = 0x00,
			ModShift = 0x01,
			ModCtrl = 0x02,
			ModAlt = 0x04,
			ModCommand = 0x08
		};

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIInput object in system memory.
		/// </summary>
		/// <param name="keyJoyMouseCursor">Whether the keyboard and joysticks also can control the mouse cursor.</param>
		explicit GUIInput(bool keyJoyMouseCursor = false) : m_KeyJoyMouseCursor(keyJoyMouseCursor) { Clear(); }
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
		/// <param name="bufferArray">Buffer array.</param>
		void GetKeyboardBuffer(unsigned char *bufferArray) const;

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
		unsigned char GetAsciiState(unsigned char ascii) const { return m_KeyboardBuffer[ascii]; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="scanCode"></param>
		/// <returns></returns>
		unsigned char GetScanCodeState(unsigned char scanCode) const { return m_ScanCodeState[scanCode]; }
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
#pragma endregion

#pragma region Virtual Methods
		/// <summary>
		/// Updates the Input.
		/// </summary>
		virtual void Update() {}
#pragma endregion

	protected:

		enum InputConstants {
			KeyboardBufferSize = 256,
			MouseButtonCount = 3
		};

		// Keyboard buffer holding the key states
		unsigned char m_KeyboardBuffer[InputConstants::KeyboardBufferSize];
		unsigned char m_ScanCodeState[InputConstants::KeyboardBufferSize];

		// Mouse button states. Order: Left, Middle, Right
		int m_MouseButtonsEvents[InputConstants::MouseButtonCount];
		int m_MouseButtonsStates[InputConstants::MouseButtonCount];

		int m_MouseX;
		int m_MouseY;
		// These offset the mouse positions so that the cursor is shifted for all events
		int m_MouseOffsetX;
		int m_MouseOffsetY;

		int	m_MouseWheelChange; //!< the amount and direction that the mouse wheel has moved.

		int m_KeyModifier;

		bool m_KeyJoyMouseCursor; //!< Whether the keyboard and joysticks also control the mouse.

	private:

		/// <summary>
		/// Clears all the member variables of this GUIInput, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();

		// Disallow the use of some implicit methods.
		GUIInput(const GUIInput &reference) = delete;
		GUIInput & operator=(const GUIInput &rhs) = delete;
	};
};
#endif