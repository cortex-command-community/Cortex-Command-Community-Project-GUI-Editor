#ifndef _GUICOLLECTIONBOX_
#define _GUICOLLECTIONBOX_

namespace RTE {

	/// <summary>
	/// A collection box control class that contains child controls.
	/// </summary>
	class GUICollectionBox : public GUIControl {

	public:

		/// <summary>
		/// Drawing type.
		/// </summary>
		enum DrawType { Color, Image };

		GUIControlOverrideMethods;

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUICollectionBox object in system memory.
		/// </summary>
		/// <param name="owningManager">GUIControlManager.</param>
		explicit GUICollectionBox(GUIControlManager *owningManager) { m_OwningManager = owningManager; m_IsContainer = true; }

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
		/// Destructor method used to clean up this before deletion from memory.
		/// </summary>
		~GUICollectionBox() override = default;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the children list.
		/// </summary>
		/// <returns></returns>
		std::deque<GUIControl *> * GetChildren() override { return &m_Children; }

		/// <summary>
		/// Gets the current drawing type.
		/// </summary>
		/// <returns>Type.</returns>
		DrawType GetDrawType() const { return (m_Properties.GetPropertyValue<std::string>("DrawType") == "Color") ? DrawType::Color : DrawType::Image; }

		/// <summary>
		/// Sets the drawing type.
		/// </summary>
		/// <param name="newDrawType">Type.</param>
		void SetDrawType(DrawType newDrawType) { m_Properties.SetPropertyValue("DrawType", (newDrawType == DrawType::Color) ? "Color" : "Image"); }

		/// <summary>
		/// Sets whether to draw the background.
		/// </summary>
		bool GetDrawBackground() const { return m_Properties.GetPropertyValue<bool>("DrawBackground"); }

		/// <summary>
		/// Sets whether to draw the background.
		/// </summary>
		/// <param name="drawBackground">Draw.</param>
		void SetDrawBackground(bool drawBackground) { m_Properties.SetPropertyValue("DrawBackground", drawBackground); }

		/// <summary>
		/// Gets the drawing color.
		/// </summary>
		/// <returns>Color.</returns>
		int GetDrawColor() const { return m_Properties.GetPropertyValue<int>("DrawColor"); }

		/// <summary>
		/// Sets the drawing color.
		/// </summary>
		/// <param name="newColor">Color.</param>
		void SetDrawColor(int newColor) { m_Properties.SetPropertyValue("DrawColor", newColor); }

		/// <summary>
		/// Gets the drawing image bitmap that is being drawn
		/// </summary>
		/// <returns>Bitmap, ownership IS NOT transferred!</returns>
		GUIBitmap * GetDrawImage() const { return m_DrawBitmap.get(); }

		/// <summary>
		/// Sets the drawing image bitmap to draw
		/// </summary>
		/// <param name="newImage">Bitmap, ownership IS transferred!</param>
		void SetDrawImage(GUIBitmap *newImage) { m_DrawBitmap.reset(newImage); }
#pragma endregion

#pragma region Positioning and Sizing
		/// <summary>
		/// Called when the control needs to be moved.
		/// </summary>
		/// <param name="newPosX">New position.</param>
		/// <param name="newPosY"></param>
		void Move(int newPosX, int newPosY) override;

		/// <summary>
		/// Moves the position of the panel by a relative amount.
		/// </summary>
		/// <param name="relX">X.</param>
		/// <param name="relY">Y.</param>
		void MoveRelative(int relX, int relY) override;
#pragma endregion

#pragma region Mouse Event Handling
		/// <summary>
		/// Called when the mouse goes down on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) override { CaptureMouse(); }

		/// <summary>
		/// Called when the mouse goes up on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) override { ReleaseMouse(); AddEvent(GUIEventType::Notification, GUIEventCode::Clicked, buttons); }

		/// <summary>
		/// Called when the mouse moves (over the panel, or when captured).
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseMove(int mousePosX, int mousePosY, int buttons, int modifier) override { AddEvent(GUIEventType::Notification, GUIEventCode::MouseMove, buttons); }
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

		static const std::string_view c_ControlType;

		static constexpr int m_MinWidth = 10;
		static constexpr int m_MinHeight = 10;
		static constexpr int m_DefaultWidth = 100;
		static constexpr int m_DefaultHeight = 100;

		/// <summary>
		/// Create the bitmap to draw.
		/// </summary>
		void BuildBitmap();
	};
};
#endif