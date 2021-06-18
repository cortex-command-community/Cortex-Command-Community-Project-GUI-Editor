#ifndef _ALLEGROINPUT_
#define _ALLEGROINPUT_

#include "GUIInput.h"

namespace RTE {

	class Timer;

	/// <summary>
	/// An input interface using the CDX Library for input GUIInput.
	/// </summary>
	class AllegroInput : public GUIInput {

	public:

		/// <summary>
		/// Constructor method used to instantiate a AllegroInput object in system memory.
		/// </summary>
		/// <param name="whichPlayer"></param>
		/// <param name="keyJoyMouseCursor">Whether the keyboard and joysticks also can control the mouse cursor.</param>
		AllegroInput(int whichPlayer, bool keyJoyMouseCursor = false);

		/// <summary>
		/// Changes the mouse movement speed based on the graphics driver in use. The default (2) movement speed is slow and sluggish in dedicated fullscreen so this is used to change between default and faster (1) when switching drivers.
		/// </summary>
		/// <param name="graphicsDriver">The graphics driver to adjust mouse movement speed to.</param>
		void AdjustMouseMovementSpeedToGraphicsDriver(int graphicsDriver) const;

		/// <summary>
		/// Updates the input.
		/// </summary>
		void Update() override;

	private:

		enum AllegroMouseButtons {
			ButtonLeft = 1,
			ButtonRight = 2,
			ButtonMiddle = 4
		};

		const float m_KeyRepeatDelay = 0.300F;
		std::array<float, 256> m_KeyboardTimes;

		std::unique_ptr<Timer> m_Timer;
		std::unique_ptr<Timer> m_CursorAccelTimer;

		/// <summary>
		/// Converts from Allegro's key push to that used by this GUI lib, with timings for repeats taken into consideration.
		/// </summary>
		/// <param name="allegroKey">The Allegro scancode.</param>
		/// <param name="guilibKey">The corresponding GUIlib scancode</param>
		/// <param name="elapsedS">The elapsed time since the last update, in seconds.</param>
		void ConvertKeyEvent(int allegroKey, int guilibKey, float elapsedS);

		/// <summary>
		/// Updates the keyboard input.
		/// </summary>
		void UpdateKeyboardInput(float keyElapsedTime);

		/// <summary>
		/// Updates the mouse input.
		/// </summary>
		void UpdateMouseInput();

		/// <summary>
		/// Updates the mouse input using the joystick or keyboard.
		/// </summary>
		void UpdateKeyJoyMouseInput(float keyElapsedTime);
	};
};
#endif