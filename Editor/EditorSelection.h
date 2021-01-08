#ifndef _RTEGUIEDITORSELECTION_
#define _RTEGUIEDITORSELECTION_

#include "GUI.h"

using namespace RTE;

class RTE::GUIScreen;
class RTE::GUIInput;

namespace RTEGUI {

	/// <summary>
	/// GUI element selection structure.
	/// </summary>
	class EditorSelection {

	public:

		static bool s_SnapToGrid; //!< Whether the selection position/size should snap to a grid when moved/resized.
		static int s_SnapGridSize; //!< The size of the snap grid in pixels.

		/// <summary>
		/// Enumeration for the directions to move when nudging a selection with keyboard input.
		/// </summary>
		enum struct NudgeDirection { NudgeUp, NudgeDown, NudgeLeft, NudgeRight };

#pragma region Getters and Setters
		/// <summary>
		/// Gets the GUI element that is currently selected by this.
		/// </summary>
		/// <returns>Pointer to the GUIControl that is selected by this.</returns>
		GUIControl * GetControl() const { return m_Control; }

		/// <summary>
		/// Sets the GUI element this is currently selected by this.
		/// </summary>
		/// <param name="newControl">The GUIControl to set as selected by this.</param>
		void SetControl(GUIControl *newControl) { m_Control = newControl; }
#pragma endregion

#pragma region Grabbing and Releasing
		/// <summary>
		/// Release any grabs made by this. Does not deselect the GUI element itself.
		/// </summary>
		void ReleaseAnyGrabs() { m_GrabbingControl = false; m_GrabbingHandle = false; m_GrabTriggered = false; }

		/// <summary>
		/// Sets the GUI element as the current grab.
		/// </summary>
		void GrabControl(GUIControl *control, int mousePosX, int mousePosY);

		/// <summary>
		/// Gets whether the GUI element is grabbed or not.
		/// </summary>
		/// <returns>Whether the GUI element is grabbed or not.</returns>
		bool IsGrabbingControl() const { return m_GrabbingControl; }

		/// <summary>
		/// Gets whether the GUI element was grabbed and triggered by enough mouse movement.
		/// </summary>
		/// <returns>True if both grabbed and moved enough to be triggered.</returns>
		bool ControlGrabbedAndTriggered() const { return m_GrabbingControl && m_GrabTriggered; }

		/// <summary>
		/// Sets a resize handle as the current grab.
		/// </summary>
		void GrabHandle(int handleIndex, int mousePosX, int mousePosY);

		/// <summary>
		/// Gets whether a resize handle is grabbed or not.
		/// </summary>
		/// <returns>Whether a resize handle is grabbed or not.</returns>
		bool IsGrabbingHandle() const { return m_GrabbingHandle; }

		/// <summary>
		/// Gets whether a resize handle was grabbed and triggered by enough mouse movement.
		/// </summary>
		/// <returns>True if both grabbed and moved enough to be triggered.</returns>
		bool HandleGrabbedAndTriggered() const { return m_GrabbingHandle && m_GrabTriggered; }

		/// <summary>
		/// Checks whether there was enough mouse movement to trigger the grab function.
		/// </summary>
		/// <param name="movedDistX">Mouse X distance moved from grab position.</param>
		/// <param name="movedDistY">Mouse Y distance moved from grab position.</param>
		void CheckMovementAndSetTriggerGrab(int movedDistX, int movedDistY);
#pragma endregion

#pragma region Selection Handling
		/// <summary>
		/// Adjusts the position to the nearest snap point if snapping to grid is enabled.
		/// </summary>
		/// <param name="Position">Position to adjust.</param>
		/// <returns>Position adjusted to nearest snap point.</returns>
		int ProcessSnapCoord(int position) const;

		/// <summary>
		/// Calculates new position/size of a GUI element given a handle movement.
		/// </summary>
		/// <param name="MouseX">X position of the mouse.</param>
		/// <param name="MouseY">Y position of the mouse.</param>
		/// <param name="X">X position of the element. Will be updated with the new position.</param>
		/// <param name="Y">Y position of the element. Will be updated with the new position.</param>
		/// <param name="Width">Width of the element. Will be updated with the new width.</param>
		/// <param name="Height">Height of the element. Will be update with the new height.</param>
		void CalculateHandleResize(int mousePosX, int mousePosY, int &xPos, int &yPos, int &width, int &height);

		/// <summary>
		/// Move the selected GUI element to the specified position.
		/// </summary>
		/// <param name="newPosX">New X position of the element in the workspace.</param>
		/// <param name="newPosY">New Y position of the element in the workspace.</param>
		/// <returns>True to indicate the change was made.</returns>
		bool MoveSelection(int newPosX, int newPosY) const;

		/// <summary>
		/// Nudge the selected GUI element in the specified direction.
		/// </summary>
		/// <param name="nudgeDirection">The direction to nudge the element.</param>
		/// <param name="preciseNudge">Whether precise nudge (1px) is enabled or not.</param>
		/// <returns>True to indicate the change was made.</returns>
		bool NudgeSelection(NudgeDirection nudgeDirection, bool preciseNudge) const;

		/// <summary>
		/// Resize the selected GUI element with the resize handle.
		/// </summary>
		/// <param name="xPosToCalcResize">X position of the mouse to calculate the new size with.</param>
		/// <param name="yPosToCalcResize">Y position of the mouse to calculate the new size with.</param>
		/// <returns>True to indicate the change was made.</returns>
		bool ResizeSelection(int xPosToCalcResize, int yPosToCalcResize);

		/// <summary>
		/// Clears the current selection box.
		/// </summary>
		void ClearSelection();
#pragma endregion

#pragma region Drawing
		/// <summary>
		/// Draws the selection box around the selected GUI element.
		/// </summary>
		/// <param name="Control">GUI element to draw selection box around.</param>
		void DrawSelectionBox(GUIScreen *screen, GUIInput *input);

		/// <summary>
		/// Draws a selection box resize handle.
		/// </summary>
		/// <param name="X">X position of drawn box.</param>
		/// <param name="Y">Y position of drawn box.</param>
		void DrawSelectionBoxHandle(GUIScreen *screen, int xPos, int yPos) const;
#pragma endregion

	private:

		GUIControl *m_Control = nullptr;

		bool m_GrabbingControl = false; //!< Indicates the element itself is grabbed by the mouse.
		bool m_GrabbingHandle = false; //!< Indicates a resize handle is grabbed by the mouse.
		bool m_GrabTriggered = false; //!< Indicates there was enough mouse movement from the grab position to trigger it's function.

		int m_HandleIndex = 0; //!< Resize handle index.

		int m_GrabX = 0;
		int m_GrabY = 0;
		int	m_ClickX = 0; //!< The X position of the mouse when a control or a handle was clicked on and grabbed. Used for mouse movement check to trigger the grab.
		int m_ClickY = 0; //!< The Y position of the mouse when a control or a handle was clicked on and grabbed. Used for mouse movement check to trigger the grab.

	};
}
#endif