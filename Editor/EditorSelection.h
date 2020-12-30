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
		/// 
		/// </summary>
		/// <returns></returns>
		GUIControl * GetControl() const { return m_Control; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="newControl"></param>
		void SetControl(GUIControl *newControl) { m_Control = newControl; }
#pragma endregion

#pragma region Grabbing and Releasing
		/// <summary>
		/// 
		/// </summary>
		void ReleaseAnyGrabs() { m_GrabbingControl = false; m_GrabbingHandle = false; m_GrabTriggered = false; }

		/// <summary>
		/// 
		/// </summary>
		void GrabControl(GUIControl *control, int mousePosX, int mousePosY);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool IsGrabbingControl() const { return m_GrabbingControl; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool ControlGrabbedAndTriggered() const { return m_GrabbingControl && m_GrabTriggered; }

		/// <summary>
		/// 
		/// </summary>
		void GrabHandle(int handleIndex, int mousePosX, int mousePosY);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool IsGrabbingHandle() const { return m_GrabbingHandle; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool HandleGrabbedAndTriggered() const { return m_GrabbingHandle && m_GrabTriggered; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="movedDistX"></param>
		/// <param name="movedDistY"></param>
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
		/// Calculates new position/size of a control given a handle movement.
		/// </summary>
		/// <param name="MouseX">Mouse.</param>
		/// <param name="MouseY"></param>
		/// <param name="X">Position.</param>
		/// <param name="Y"></param>
		/// <param name="Width">Size.</param>
		/// <param name="Height"></param>
		void CalculateHandleResize(int mousePosX, int mousePosY, int &xPos, int &yPos, int &width, int &height);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="newPosX"></param>
		/// <param name="newPosY"></param>
		/// <returns></returns>
		bool MoveSelection(int newPosX, int newPosY) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="nudgeDirection"></param>
		/// <param name="preciseNudge"></param>
		/// <returns></returns>
		bool NudgeSelection(NudgeDirection nudgeDirection, bool preciseNudge) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="xPosToCalcResize"></param>
		/// <param name="yPosToCalcResize"></param>
		/// <returns></returns>
		bool ResizeSelection(int xPosToCalcResize, int yPosToCalcResize);

		/// <summary>
		/// Clears the current selection box.
		/// </summary>
		void ClearSelection();
#pragma endregion

#pragma region Drawing
		/// <summary>
		/// Draws the selection box around the selected control.
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

		bool m_GrabbingControl = false;
		bool m_GrabbingHandle = false;
		bool m_GrabTriggered = false;

		int m_HandleIndex = 0;

		int m_GrabX = 0;
		int m_GrabY = 0;
		int	m_ClickX = 0; //!< The X position of the mouse when a control or a handle was clicked on and grabbed. Used for mouse movement check to trigger the grab.
		int m_ClickY = 0; //!< The Y position of the mouse when a control or a handle was clicked on and grabbed. Used for mouse movement check to trigger the grab.

	};
}
#endif