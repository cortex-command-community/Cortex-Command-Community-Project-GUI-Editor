#ifndef _GUILABEL_
#define _GUILABEL_

#ifdef GUI_STANDALONE
#include "Timer.h"
#endif

namespace RTE {

	/// <summary>
	/// A label control class.
	/// </summary>
	class GUILabel : public GUIControl {

	public:

		GUIControlOverrideMethods;

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUILabel object in system memory.
		/// </summary>
		/// <param name="owningManager">GUIControlManager.</param>
		explicit GUILabel(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		void Create(const std::string_view &name, int posX, int posY, int width = -1, int height = -1) override;

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="reference">Properties.</param>
		void Create(GUIProperties *reference) override;
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		~GUILabel() override = default;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the text of the label.
		/// </summary>
		/// <returns></returns>
		std::string GetText() const { return m_Properties.GetPropertyValue<std::string>("Text"); }

		/// <summary>
		/// Sets the text of the label.
		/// </summary>
		/// <param name="newText">Text.</param>
		void SetText(const std::string_view &newText) { m_Properties.SetPropertyValue("Text", newText); }
#pragma endregion

#pragma region Positioning and Sizing
		/// <summary>
		/// Shows how tall the current text is with the current width and font etc.
		/// </summary>
		/// <returns>The text height, in pixels.</returns>
		int GetTextHeight();
#pragma endregion


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          SetHAlignment
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Sets the horizontal alignment of the text of this label.
		// Arguments:       The desired alignment.

		void SetHAlignment(int HAlignment = GUIFont::HAlignment::Left) { m_HAlignment = HAlignment; }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          SetVAlignment
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Sets the vertical alignment of the text of this label.
		// Arguments:       The desired alignment.

		void SetVAlignment(int VAlignment = GUIFont::VAlignment::Top) { m_VAlignment = VAlignment; }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetHAlignment
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets the horizontal alignment of the text of this label.
		// Arguments:       None.

		int GetHAlignment() const { return m_HAlignment; }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetVAlignment
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets the vertical alignment of the text of this label.
		// Arguments:       The desired alignment.

		int GetVAlignment() const { return m_VAlignment; }

		/// <summary>
		/// Gets whether or not this GUILabel should scroll horizontally (right) when it overflows.
		/// </summary>
		/// <returns>Whether or not this GUILabel should scroll horizontally when it overflows.</returns>
		bool GetHorizontalOverflowScroll() const { return m_HorizontalOverflowScroll; }

		/// <summary>
		/// Sets whether or not this GUILabel should scroll horizontally (right) when it overflows. Mutually exclusive with horizontal overflow scrolling.
		/// </summary>
		/// <param name="newOverflowScroll">Whether or not this GUILabel should scroll horizontally when it overflows.</param>
		void SetHorizontalOverflowScroll(bool newOverflowScroll);

		/// <summary>
		/// Gets whether or not this GUILabel should scroll vertically (down) when it overflows.
		/// </summary>
		/// <returns>Whether or not this GUILabel should scroll vertically when it overflows.</returns>
		bool GetVerticalOverflowScroll() const { return m_VerticalOverflowScroll; }

		/// <summary>
		/// Sets whether or not this GUILabel should scroll vertically (down) when it overflows. Mutually exclusive with horizontal overflow scrolling.
		/// </summary>
		/// <param name="newOverflowScroll">Whether or not this GUILabel should scroll vertically when it overflows.</param>
		void SetVerticalOverflowScroll(bool newOverflowScroll);

		/// <summary>
		/// Gets whether or not horizontal or vertical overflow scrolling is turned on.
		/// </summary>
		/// <returns>Whether or not horizontal or vertical overflow scrolling is turned on.</returns>
		bool OverflowScrollIsEnabled() const { return m_HorizontalOverflowScroll || m_VerticalOverflowScroll; }

		/// <summary>
		/// Gets whether or not horizontal/vertical scrolling is happening.
		/// </summary>
		/// <returns>Whether or not horizontal/vertical scrolling is happening.</returns>
		bool OverflowScrollIsActivated() const { return OverflowScrollIsEnabled() && m_OverflowScrollState != OverflowScrollState::Deactivated; }

		/// <summary>
		/// Sets whether or not horizontal/vertical scrolling should be happening. When it's deactivated, text will instantly go back to un-scrolled.
		/// </summary>
		/// <param name="activateScroll">Whether the overflow scrolling should activate (true) or deactivate (false).</param>
		void ActivateDeactivateOverflowScroll(bool activateScroll);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          StoreProperties
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets the control to store the values into properties.
		// Arguments:       None.
		void StoreProperties() override;


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          ApplyProperties
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Applies new properties to the control.
		// Arguments:       GUIProperties.
		void ApplyProperties(GUIProperties *Props) override;


#pragma region Mouse Event Handling
		/// <summary>
		/// Called when the mouse goes down on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) override;

		/// <summary>
		/// Called when the mouse goes up on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) override;
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Resize the height of the label to fit the amount of text it has to display.
		/// </summary>
		/// <returns>The new height in pixels.</returns>
		int ResizeHeightToFit();

		/// <summary>
		/// Draws the Label to the given GUIBitmap.
		/// </summary>
		/// <param name="Bitmap">The GUIBitmap to draw the label to.</param>
		/// <param name="overwiteFontColorAndKerning">Whether to overwrite the font's color and kerning with the stored values. Defaults to true, which is usually what you want.</param>
		void Draw(GUIBitmap *targetBitmap, bool overwiteFontColorAndKerning = true);
#pragma endregion

#pragma region Virtual Override Methods
		/// <summary>
		/// Called when the skin has been changed.
		/// </summary>
		/// <param name="newSkin">New skin pointer.</param>
		void ChangeSkin(GUISkin *newSkin) override;

		/// <summary>
		/// Draws the panel.
		/// </summary>
		/// <param name="targetScreen">Screen class.</param>
		void Draw(GUIScreen *targetScreen) override;
#pragma endregion

	private:

		/// <summary>
		/// 
		/// </summary>
		enum class OverflowScrollState { Deactivated, WaitAtStart, Scrolling, WaitAtEnd };

		static const std::string_view c_ControlType;

		static constexpr int m_MinWidth = 20;
		static constexpr int m_MinHeight = 10;
		static constexpr int m_DefaultWidth = 30;
		static constexpr int m_DefaultHeight = 16;

		int m_HAlignment = GUIFont::HAlignment::Left;
		int m_VAlignment = GUIFont::VAlignment::Middle;
		bool m_HorizontalOverflowScroll = false; //!< Note that horizontal overflow scrolling means text will always be on one line.
		bool m_VerticalOverflowScroll = false;

		OverflowScrollState m_OverflowScrollState = OverflowScrollState::Deactivated;

		Timer m_OverflowScrollTimer = Timer();
	};
};
#endif