#include "EditorSelection.h"

#include "AllegroScreen.h"
#include "GUIInputWrapper.h"

namespace RTEGUI {

	int EditorSelection::s_SnapGridSize = 5;
	bool EditorSelection::s_SnapToGrid = true;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::GrabControl(GUIControl *control, int mousePosX, int mousePosY) {
		m_Control = control;
		m_GrabbingControl = true;
		m_GrabbingHandle = false;

		m_Control->GetControlRect(&m_GrabX, &m_GrabY, nullptr, nullptr);
		m_ClickX = mousePosX;
		m_ClickY = mousePosY;
		m_GrabX -= m_ClickX;
		m_GrabY -= m_ClickY;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::GrabHandle(int handleIndex, int mousePosX, int mousePosY) {
		m_GrabbingControl = false;
		m_GrabbingHandle = true;
		m_HandleIndex = handleIndex;
		m_GrabX = m_ClickX = mousePosX;
		m_GrabY = m_ClickY = mousePosY;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::CheckMovementAndSetTriggerGrab(int movedDistX, int movedDistY) {
		bool result = m_GrabTriggered;
		if ((m_GrabbingControl || m_GrabbingHandle) && !m_GrabTriggered) {
			int moveDist = 4;
			result = std::abs(m_ClickX - movedDistX) >= moveDist || std::abs(m_ClickY - movedDistY) >= moveDist;
		}
		m_GrabTriggered = result;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int EditorSelection::ProcessSnapCoord(int position) const {
		if (s_SnapToGrid) { position = static_cast<int>(std::round(static_cast<float>(position) / static_cast<float>(s_SnapGridSize))) * s_SnapGridSize; }
		return position;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::CalculateHandleResize(int mousePosX, int mousePosY, int &xPos, int &yPos, int &width, int &height) {
		int controlPosX = 0;
		int controlPosY = 0;
		int controlWidth = 0;
		int controlHeight = 0;
		int parentPosX = 0;
		int parentPosY = 0;
		int parentWidth = 0;
		int parentHeight = 0;
		m_Control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);
		GUIControl *parent = m_Control->GetParent();
		parent->GetControlRect(&parentPosX, &parentPosY, &parentWidth, &parentHeight);

		int minSize = 10;

		// Left Move/Resize
		if (m_HandleIndex == 0 || m_HandleIndex == 3 || m_HandleIndex == 6) {
			int diff = mousePosX - m_GrabX;
			if (controlPosX + diff < parentPosX) { diff = parentPosX - controlPosX; }
			if (controlWidth - diff < minSize) { diff = controlWidth - minSize; }

			diff = ProcessSnapCoord(diff);
			controlPosX += diff;
			controlWidth -= diff;
		}
		// Top Move/Resize
		if (m_HandleIndex == 0 || m_HandleIndex == 1 || m_HandleIndex == 2) {
			int diff = mousePosY - m_GrabY;
			if (controlPosY + diff < parentPosY) { diff = parentPosY - controlPosY; }
			if (controlHeight - diff < minSize) { diff = controlHeight - minSize; }

			diff = ProcessSnapCoord(diff);
			controlPosY += diff;
			controlHeight -= diff;
		}
		// Right Resize
		if (m_HandleIndex == 2 || m_HandleIndex == 5 || m_HandleIndex == 8) {
			int diff = mousePosX - m_GrabX;
			if (controlPosX + controlWidth + diff > parentPosX + parentWidth) { diff = (parentPosX + parentWidth) - (controlPosX + controlWidth); }

			diff = ProcessSnapCoord(diff);
			controlWidth += diff;
		}
		// Bottom Resize
		if (m_HandleIndex == 6 || m_HandleIndex == 7 || m_HandleIndex == 8) {
			int diff = mousePosY - m_GrabY;
			if (controlPosY + controlHeight + diff > parentPosY + parentHeight) { diff = (parentPosY + parentHeight) - (controlPosY + controlHeight); }

			diff = ProcessSnapCoord(diff);
			controlHeight += diff;
		}

		controlWidth = std::max(controlWidth, minSize);
		controlHeight = std::max(controlHeight, minSize);

		xPos = controlPosX;
		yPos = controlPosY;
		width = controlWidth;
		height = controlHeight;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorSelection::MoveSelection(int newPosX, int newPosY) const {
		int xPosToMoveTo = ProcessSnapCoord(newPosX + m_GrabX);
		int yPosToMoveTo = ProcessSnapCoord(newPosY + m_GrabY);
		int currentPosX = 0;
		int currentPosY = 0;
		m_Control->GetControlRect(&currentPosX, &currentPosY, nullptr, nullptr);

		if (xPosToMoveTo == currentPosX && yPosToMoveTo == currentPosY) {
			return false;
		}
		m_Control->Move(xPosToMoveTo, yPosToMoveTo);
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorSelection::NudgeSelection(NudgeDirection nudgeDirection, bool preciseNudge) const {
		int nudgeSize = preciseNudge ? 1 : s_SnapGridSize;
		int currentPosX = 0;
		int currentPosY = 0;
		m_Control->GetControlRect(&currentPosX, &currentPosY, nullptr, nullptr);

		switch (nudgeDirection) {
			case NudgeDirection::NudgeUp:
				m_Control->Move(currentPosX, currentPosY - nudgeSize);
				break;
			case NudgeDirection::NudgeDown:
				m_Control->Move(currentPosX, currentPosY + nudgeSize);
				break;
			case NudgeDirection::NudgeLeft:
				m_Control->Move(currentPosX - nudgeSize, currentPosY);
				break;
			case NudgeDirection::NudgeRight:
				m_Control->Move(currentPosX + nudgeSize, currentPosY);
				break;
			default:
				break;
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorSelection::ResizeSelection(int xPosToCalcResize, int yPosToCalcResize) {
		int xPos = 0;
		int yPos = 0;
		int width = 0;
		int height = 0;
		CalculateHandleResize(xPosToCalcResize, yPosToCalcResize, xPos, yPos, width, height);
		m_Control->Move(xPos, yPos);
		m_Control->Resize(width, height);
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::ClearSelection() {
		m_GrabbingControl = false;
		m_GrabbingHandle = false;
		m_GrabTriggered = false;
		m_Control = nullptr;
		m_HandleIndex = 0;
		m_GrabX = 0;
		m_GrabY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::DrawSelectionBox(GUIScreen *screen, const GUIInput *input) {
		if (!m_Control) {
			return;
		}

		int mousePosX;
		int mousePosY;
		input->GetMousePosition(&mousePosX, &mousePosY);

		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		m_Control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);

		// If we've grabbed and moved the control, draw the selection box where the mouse was moved to
		if (m_GrabbingControl && m_GrabTriggered) {
			controlPosX = ProcessSnapCoord(mousePosX + m_GrabX);
			controlPosY = ProcessSnapCoord(mousePosY + m_GrabY);
		}

		// Grabbed handles
		if (m_GrabbingHandle && m_GrabTriggered) { CalculateHandleResize(mousePosX, mousePosY, controlPosX, controlPosY, controlWidth, controlHeight); }

		GUIRect clipRect;
		SetRect(&clipRect, controlPosX - 6, controlPosY - 6, controlPosX + controlWidth + 6, controlPosY + controlHeight + 6);
		screen->GetBitmap()->SetClipRect(&clipRect);

		screen->GetBitmap()->DrawRectangle(controlPosX, controlPosY, controlWidth, controlHeight, 0xFFCCCCCC, false);

		// Draw the handles
		for (int i = 0; i < 3; i++) {
			DrawSelectionBoxHandle(screen, controlPosX, controlPosY + i * (controlHeight / 2));
			if (i != 1) { DrawSelectionBoxHandle(screen, controlPosX + controlWidth / 2, controlPosY + i * (controlHeight / 2)); }
			DrawSelectionBoxHandle(screen, controlPosX + controlWidth, controlPosY + i * (controlHeight / 2));
		}
		screen->GetBitmap()->SetClipRect(nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorSelection::DrawSelectionBoxHandle(const GUIScreen *screen, int xPos, int yPos) const {
		int boxSize = 5;
		screen->GetBitmap()->DrawRectangle(xPos - boxSize / 2, yPos - boxSize / 2, boxSize, boxSize, 0x000000, true);
		screen->GetBitmap()->DrawRectangle(xPos - boxSize / 2, yPos - boxSize / 2, boxSize, boxSize, 0xFFFFFF, false);
	}
}