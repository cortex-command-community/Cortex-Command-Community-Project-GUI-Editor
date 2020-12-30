#include "EditorApp.h"
#include "EditorUtil.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "GUILabel.h"
#include "GUITextBox.h"
#include "winalleg.h"

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorApp::Initialize() {
		set_color_depth(32);
		set_color_conversion(COLORCONV_MOST);
		set_window_title("Cortex Command GUI Editor");
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_ResX, m_ResY, 0, 0);
		set_close_button_callback(QuitHandler);
		set_resize_callback(ResizeHandler);
		set_display_switch_callback(SWITCH_OUT, SwitchOutHandler);
		set_display_switch_callback(SWITCH_IN, SwitchInHandler);

		// Don't want to deal with recreating the backbuffer on window resize so just create one as large as the screen.
		m_BackBuffer = create_bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		clear_to_color(m_BackBuffer, 0);

		m_ZoomBuffer = create_bitmap(m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
		clear_to_color(m_ZoomBuffer, 0);

		m_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		m_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI

		return true;
	}


		// Only allow workspace zoom if the screen resolution is FHD or above, smaller resolutions can't fully display it
		}

	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::DestroyBackBuffers() {
		destroy_bitmap(m_BackBuffer);
		destroy_bitmap(m_ZoomBuffer);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorApp::Update() {
		m_EditorManager->Update();
		GUIEvent editorEvent;
		while (m_EditorManager->GetEvent(&editorEvent)) {
			switch (editorEvent.GetType()) {
				case GUIEvent::Command:
					if (editorEvent.GetControl()->GetName() == "QuitButton") {
						OnQuitButton();
					} else if (editorEvent.GetControl()->GetName() == "LoadButton") {
						OnLoadButton();
					} else if (editorEvent.GetControl()->GetName() == "AddButton") {
						OnLoadButton(true);
					} else if (editorEvent.GetControl()->GetName() == "SaveButton") {
						OnSaveButton();
					} else if (editorEvent.GetControl()->GetName() == "SaveAsButton") {
						OnSaveButton(true);
					} else if (editorEvent.GetControl()->GetName().substr(0, 2).compare("C_") == 0) {
						AddNewControl(editorEvent);
					}
					break;
				case GUIEvent::Notification:
					if (editorEvent.GetControl()->GetName() == "ActiveCollectionBoxes" && editorEvent.GetMsg() == GUIListBox::MouseDown) {
						UpdateCollectionBoxList();
					} else if (editorEvent.GetControl()->GetName() == "PropertyPage") {
						UpdatePropertyPage(editorEvent);
					} else if (editorEvent.GetControl()->GetName() == "GridSizeTextBox" && editorEvent.GetMsg() == GUITextBox::Enter) {
						UpdateGridSize(editorEvent);
					} else if (editorEvent.GetControl()->GetName() == "SnapCheckBox") {
						m_SnapToGrid = (dynamic_cast<GUICheckbox *>(editorEvent.GetControl()))->GetCheck() == GUICheckbox::Checked;
					} else if (editorEvent.GetControl()->GetName() == "ZoomCheckBox") {
						m_Zoom = (dynamic_cast<GUICheckbox *>(editorEvent.GetControl()))->GetCheck() == GUICheckbox::Checked;
					}
					break;
				default:
					break;
			}
		}
		ProcessMouseInput();
		ProcessKeyboardInput();

		return !m_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::ProcessMouseInput() {
		std::array<int, 3> mouseButtons;
		std::array<int, 3> mouseStates;
		int mousePosX;
		int mousePosY;
		m_Input->GetMouseButtons(mouseButtons.data(), mouseStates.data());
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		// If click released
		if (mouseButtons.at(0) == GUIInput::Released) {
			// Move the control after a grab
			if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				// TODO: Check if not moved to another parent
				int destX = ProcessSnapCoord(mousePosX + m_SelectionInfo.GrabX);
				int destY = ProcessSnapCoord(mousePosY + m_SelectionInfo.GrabY);
				m_SelectionInfo.Control->Move(destX, destY);

				m_UnsavedChanges = true;
			}

			// Resize/Move control after a grab
			if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				int xPos;
				int yPos;
				int width;
				int height;
				CalculateHandleResize(mousePosX, mousePosY, &xPos, &yPos, &width, &height);

				m_SelectionInfo.Control->Move(xPos, yPos);
				m_SelectionInfo.Control->Resize(width, height);

				m_UnsavedChanges = true;
			}

			// Update properties
			if (!ControlUnderMouse(m_PropertyPage.get(), mousePosX, mousePosY) && m_SelectionInfo.Control) { UpdateControlProperties(m_SelectionInfo.Control); }

			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
			m_SelectionInfo.TriggerGrab = false;
		}


		// Check for grabbing handles
		if (!m_SelectionInfo.GrabbedControl && m_SelectionInfo.Control && mouseButtons.at(0) == GUIInput::Pushed) {
			int HandleIndex = HandleUnderMouse(m_SelectionInfo.Control, mousePosX, mousePosY);
			if (HandleIndex != -1) {
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = true;
				m_SelectionInfo.HandleIndex = HandleIndex;

				m_SelectionInfo.GrabX = mousePosX;
				m_SelectionInfo.GrabY = mousePosY;
				m_SelectionInfo.ClickX = mousePosX;
				m_SelectionInfo.ClickY = mousePosY;
			}
		}

		// If we've grabbed a control or handle, and we've moved far enough from the starting spot, trigger the grab
		// This prevents quickly selecting a control and slightly moving a couple pixels before releasing
		if ((m_SelectionInfo.GrabbedControl || m_SelectionInfo.GrabbedHandle) && !m_SelectionInfo.TriggerGrab) {
			int moveDist = 4;
			if (std::fabs(m_SelectionInfo.ClickX - mousePosX) >= moveDist || std::fabs(m_SelectionInfo.ClickY - mousePosY) >= moveDist) { m_SelectionInfo.TriggerGrab = true; }
		}

		// Check if mouse clicked on a control
		if (!m_SelectionInfo.GrabbedControl && !m_SelectionInfo.GrabbedHandle && mouseButtons.at(0) == GUIInput::Pushed) {
			GUIControl *control = ControlUnderMouse(m_RootControl, mousePosX, mousePosY);
			if (control && control != m_RootControl) {
				int xPos;
				int yPos;
				int width;
				int height;
				control->GetControlRect(&xPos, &yPos, &width, &height);

				m_SelectionInfo.GrabbedControl = true;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = control;

				m_SelectionInfo.GrabX = xPos - mousePosX;
				m_SelectionInfo.GrabY = yPos - mousePosY;
				m_SelectionInfo.ClickX = mousePosX;
				m_SelectionInfo.ClickY = mousePosY;

				UpdateControlProperties(m_SelectionInfo.Control, false);

				SelectActiveControlInList();
			} else if (control == m_RootControl) {
				// Unselect control
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = nullptr;

				m_PropertyPage->ClearValues();
				SelectActiveControlInList();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::ProcessKeyboardInput() {
		// Handle keyboard input directly from Allegro instead of through AllegroInput to make life easier.
		for (int i = 0; i < KEY_MAX; ++i) {
			m_KeyStates.at(i) = key[i];
		}
		int pressed = -1;

		bool modCtrl = m_KeyStates.at(KEY_LCONTROL) == pressed || m_KeyStates.at(KEY_RCONTROL) == pressed;
		bool modShift = m_KeyStates.at(KEY_LSHIFT) == pressed || m_KeyStates.at(KEY_RSHIFT) == pressed;
		bool modAlt = m_KeyStates.at(KEY_ALT) == pressed;

		if (!m_PropertyPage->HasTextFocus() && m_SelectionInfo.Control) {
			if (m_KeyStates.at(KEY_DEL) == pressed) {
				m_ControlManager->RemoveControl(m_SelectionInfo.Control->GetName(), true);
				m_SelectionInfo.Control = nullptr;
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_PropertyPage->ClearValues();
			} else {
				const GUIPanel *selectedElement = dynamic_cast<GUIPanel *>(m_SelectionInfo.Control);

				int nudgeSize = modShift ? 1 : m_GridSize;

				if (m_KeyStates.at(KEY_UP) == pressed && m_PrevKeyStates.at(KEY_UP) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos(), selectedElement->GetYPos() - nudgeSize);
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_DOWN) == pressed && m_PrevKeyStates.at(KEY_DOWN) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos(), selectedElement->GetYPos() + nudgeSize);
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_LEFT) == pressed && m_PrevKeyStates.at(KEY_LEFT) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos() - nudgeSize, selectedElement->GetYPos());
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_RIGHT) == pressed && m_PrevKeyStates.at(KEY_RIGHT) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos() + nudgeSize, selectedElement->GetYPos());
					UpdateControlProperties(m_SelectionInfo.Control);
				}
			}
		}

		// Escape key - Undo any grab
		if (m_KeyStates.at(KEY_ESC) == pressed) {
			m_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}

		if (modCtrl) {
			if (m_KeyStates.at(KEY_S) == pressed) {
				OnSaveButton(modShift ? true : false);
			} else if (m_KeyStates.at(KEY_O) == pressed) {
				OnLoadButton(modShift ? true : false);
			}
		}

		if (modAlt && m_KeyStates.at(KEY_F4)) { OnQuitButton(); }

		m_PrevKeyStates = m_KeyStates;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		}


	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::DrawEditor() {
		if (m_WindowResized) {
			acknowledge_resize();
			m_WindowResized = false;
		}
		clear_to_color(m_BackBuffer, 0);

		m_EditorBase->Draw(m_Screen.get());
		m_ControlManager->Draw();
		if (m_SelectionInfo.Control) { DrawSelectionBox(m_SelectionInfo.Control); }
		m_LeftColumn->Draw(m_Screen.get());
		m_RightColumn->Draw(m_Screen.get());
		m_EditorManager->DrawMouse();

		if (m_Zoom) {
			stretch_blit(m_BackBuffer, m_ZoomBuffer, m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight, 0, 0, m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
			blit(m_ZoomBuffer, m_BackBuffer, 0, 0, m_WorkspacePosX, m_WorkspacePosY - 30, m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, screen->w, screen->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnLoadButton(bool addControls) {
		std::string newFilename;
		if (EditorUtil::DisplayLoadGUIFile(&newFilename, win_get_window())) {
			m_ControlManager->Load(newFilename, addControls);
			m_Filename = newFilename;

			GUIControl *newRootControl = m_ControlManager->GetControlList()->front();
			m_RootControl = newRootControl;
			newRootControl->Move(m_WorkspacePosX, m_WorkspacePosY);
			newRootControl->StoreProperties();

			GUIProperties newRootControlProps;
			newRootControlProps.Update(newRootControl->GetProperties(), true);
			newRootControl->GetPanel()->BuildProperties(&newRootControlProps);

			m_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
			m_UnsavedChanges = false;

			PopulateCollectionBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnSaveButton(bool saveAsNewFile) {
		if (saveAsNewFile || m_Filename.empty()) {
			std::string newFilename;
			if (EditorUtil::DisplaySaveGUIFile(&newFilename, win_get_window())) { m_Filename = newFilename; }
		}
		// Move the root object to top left corner before saving so it is displayed correctly in-game.
		m_RootControl->Move(0, 0);

		m_ControlManager->Save(m_Filename);

		// Move the root object back to the workspace position in the editor
		m_RootControl->Move(m_WorkspacePosX, m_WorkspacePosY);

		m_UnsavedChanges = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnQuitButton() {
		int quitResult = 1;
		if (m_UnsavedChanges) {
			quitResult = EditorUtil::QuitMessageBox("Save changes made?", win_get_window());
			if (quitResult == 1) { OnSaveButton(); }
		}
		m_Quit = (quitResult != 0) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo) {
		m_EditorBase->Resize(resizeInfo->new_w, resizeInfo->new_h);
		m_LeftColumn->Resize(m_LeftColumn->GetWidth(), resizeInfo->new_h);
		m_RightColumn->Resize(m_RightColumn->GetWidth(), resizeInfo->new_h);
		m_RightColumn->Move(resizeInfo->new_w - m_RightColumn->GetWidth(), 0);
		m_ControlsInActiveCollectionBoxList->Resize(m_CollectionBoxList->GetWidth(), resizeInfo->new_h - m_ControlsInActiveCollectionBoxList->GetRelYPos() - 5);
		m_WindowResized = true;
	}
}
