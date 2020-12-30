#include "EditorApp.h"
#include "EditorUtil.h"
#include "GUICheckbox.h"
#include "GUITextBox.h"
#include "winalleg.h"

namespace RTEGUI {

	int64_t EditorApp::s_FrameTime = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::Initialize() {
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

		m_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		m_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI
		m_EditorManager = std::make_unique<EditorManager>(m_Screen.get(), m_Input.get(), "Assets", "EditorSkin.ini");

		// Only allow workspace zoom if the screen resolution is FHD or above, smaller resolutions can't fully display it
		if (m_BackBuffer->w < 1920 && m_BackBuffer->h < 1080) {
			m_EditorManager->DisableZoomCheckbox();
		} else {
			m_ZoomBuffer = create_bitmap(m_EditorManager->GetWorkspaceWidth() * 2, m_EditorManager->GetWorkspaceHeight() * 2);
			clear_to_color(m_ZoomBuffer, 0);
		}

		//show_os_cursor(MOUSE_CURSOR_ARROW);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::DestroyBackBuffers() {
		destroy_bitmap(m_BackBuffer);
		if (m_ZoomBuffer) { destroy_bitmap(m_ZoomBuffer); }
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

	bool EditorApp::UpdateEditor() {
		m_EditorManager->GetControlManager()->Update();
		GUIEvent editorEvent;
		while (m_EditorManager->GetControlManager()->GetEvent(&editorEvent)) {
			std::string controlName = editorEvent.GetControl()->GetName();
			switch (editorEvent.GetType()) {
				case GUIEvent::Command:
					if (controlName == "QuitButton") {
						OnQuitButton();
					} else if (controlName == "LoadButton") {
						OnLoadButton();
					} else if (controlName == "AddButton") {
						OnLoadButton(true);
					} else if (controlName == "SaveButton") {
						OnSaveButton();
					} else if (controlName == "SaveAsButton") {
						OnSaveButton(true);
					} else if (controlName.substr(0, 2).compare("C_") == 0) {
						m_UnsavedChanges = m_EditorManager->AddNewControl(editorEvent);
					}
					break;
				case GUIEvent::Notification:
					if (controlName == "PropertyPage") {
						m_UnsavedChanges = m_EditorManager->UpdatePropertyPage(editorEvent);
					} else if (controlName == "CollectionBoxList" && editorEvent.GetMsg() == GUIListBox::MouseDown) {
						m_EditorManager->UpdateCollectionBoxList();
					} else if (controlName == "ControlsInCollectionBoxList" && editorEvent.GetMsg() == GUIListBox::MouseDown) {
						//m_EditorManager->UpdateControlsInCollectionBoxList();
					} else if (controlName == "GridSizeTextBox" && editorEvent.GetMsg() == GUITextBox::Enter) {
						m_EditorManager->UpdateSnapGridSize(editorEvent);
					} else if (controlName == "SnapCheckBox") {
						EditorSelection::s_SnapToGrid = dynamic_cast<GUICheckbox *>(editorEvent.GetControl())->GetCheck() == GUICheckbox::Checked;
					} else if (controlName == "ZoomCheckBox") {
						m_ZoomWorkspace = (dynamic_cast<GUICheckbox *>(editorEvent.GetControl()))->GetCheck() == GUICheckbox::Checked;
					}
					break;
				default:
					break;
			}
		}
		ProcessMouseInput();
		ProcessKeyboardInput();

		m_EditorManager->SetFrameTimeLabelText(s_FrameTime);

		return !m_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::DrawEditor() {
		if (m_WindowResized) {
			acknowledge_resize();
			m_WindowResized = false;
		}
		clear_to_color(m_BackBuffer, 0);

		m_EditorManager->GetEditorBase()->Draw(m_Screen.get());
		m_EditorManager->GetWorkspaceManager()->Draw();
		m_EditorManager->GetCurrentSelection().DrawSelectionBox(m_Screen.get(), m_Input.get());
		m_EditorManager->GetLeftColumn()->Draw(m_Screen.get());
		m_EditorManager->GetRightColumn()->Draw(m_Screen.get());
		m_EditorManager->GetControlManager()->DrawMouse();

		if (m_ZoomWorkspace) {
			stretch_blit(m_BackBuffer, m_ZoomBuffer, m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY(), m_EditorManager->GetWorkspaceWidth(), m_EditorManager->GetWorkspaceHeight(), 0, 0, m_EditorManager->GetWorkspaceWidth() * 2, m_EditorManager->GetWorkspaceHeight() * 2);
			blit(m_ZoomBuffer, m_BackBuffer, 0, 0, m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY() - 30, m_EditorManager->GetWorkspaceWidth() * 2, m_EditorManager->GetWorkspaceHeight() * 2);
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, screen->w, screen->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnLoadButton(bool addControls) {
		if (m_UnsavedChanges) {
			int result = EditorUtil::DisplayDialogBox("Save changes made?", win_get_window());
			if (result == 0) {
				return;
			} else if (result == 1) {
				OnSaveButton();
			}
		}
		std::string newFilename;
		if (EditorUtil::DisplayLoadFileDialogBox(newFilename, win_get_window())) {
			m_EditorManager->GetWorkspaceManager()->Load(newFilename, addControls);
			m_ActiveFileName = newFilename;

			GUIControl *newRootControl = m_EditorManager->GetWorkspaceManager()->GetControlList()->front();
			newRootControl->Move(m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY());
			newRootControl->StoreProperties();

			GUIProperties newRootControlProps;
			newRootControlProps.Update(newRootControl->GetProperties(), true);
			newRootControl->GetPanel()->BuildProperties(&newRootControlProps);

			m_EditorManager->ClearCurrentSelection();
			m_EditorManager->SetRootControl(newRootControl);
			m_UnsavedChanges = false;

			m_EditorManager->PopulateCollectionBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnSaveButton(bool saveAsNewFile) {
		if (saveAsNewFile || m_ActiveFileName.empty()) {
			std::string newFilename;
			if (EditorUtil::DisplaySaveFileDialogBox(newFilename, win_get_window())) { m_ActiveFileName = newFilename; }
		}
		// Move the root object to top left corner before saving so it is displayed correctly in-game.
		m_EditorManager->GetRootControl()->Move(0, 0);

		m_EditorManager->GetWorkspaceManager()->Save(m_ActiveFileName);

		// Move the root object back to the workspace position in the editor
		m_EditorManager->GetRootControl()->Move(m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY());

		m_UnsavedChanges = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnQuitButton() {
		int quitResult = 1;
		if (m_UnsavedChanges) {
			quitResult = EditorUtil::DisplayDialogBox("Save changes made?", win_get_window());
			if (quitResult == 1) { OnSaveButton(); }
		}
		m_Quit = (quitResult != 0) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo) {
		m_EditorManager->GetRightColumn()->Move(resizeInfo->new_w - m_EditorManager->GetRightColumn()->GetWidth(), 0);
		m_WindowResized = true;
	}
}
