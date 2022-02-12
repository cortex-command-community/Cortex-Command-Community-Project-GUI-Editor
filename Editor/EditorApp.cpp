#include "EditorApp.h"
#include "EditorUtil.h"
#include "GUIConstants.h"
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
		int desktopResX;
		int desktopResY;
		get_desktop_resolution(&desktopResX, &desktopResY);
		m_BackBuffer = create_bitmap(desktopResX, desktopResY);
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

		install_mouse();
		select_mouse_cursor(MOUSE_CURSOR_ARROW);
		show_mouse(screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::DestroyBackBuffers() {
		destroy_bitmap(m_BackBuffer);
		if (m_ZoomBuffer) { destroy_bitmap(m_ZoomBuffer); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::ProcessMouseInput() {
		std::array<int, 3> mouseButtons;
		int mousePosX;
		int mousePosY;
		m_Input->GetMouseButtons(mouseButtons.data(), nullptr);
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		EditorSelection &currentSelection = m_EditorManager->GetCurrentSelection();

		// Trigger the grab only if we grabbed the control/handle and moved it far enough from the starting spot, this prevents accidental small movements when grabbing/releasing.
		currentSelection.CheckMovementAndSetTriggerGrab(mousePosX, mousePosY);

		if (mouseButtons.at(0) == GUIInput::InputEvents::Released) {
			if (currentSelection.GetControl()) {
				// Move the control after a grab
				if (currentSelection.ControlGrabbedAndTriggered()) { m_UnsavedChanges = currentSelection.MoveSelection(mousePosX, mousePosY); }
				// Resize/Move control after a grab
				if (currentSelection.HandleGrabbedAndTriggered()) { m_UnsavedChanges = currentSelection.ResizeSelection(mousePosX, mousePosY); }
				// Update properties
				if (m_UnsavedChanges && !m_EditorManager->ControlUnderMouse(m_EditorManager->GetPropertyPage(), mousePosX, mousePosY)) { m_EditorManager->UpdateControlProperties(currentSelection.GetControl()); }
			}
			currentSelection.ReleaseAnyGrabs();
		} else if (mouseButtons.at(0) == GUIInput::InputEvents::Pushed) {
			// Check for grabbing handles
			if (currentSelection.GetControl() && !currentSelection.IsGrabbingControl()) {
				int handleIndex = m_EditorManager->HandleUnderMouse(currentSelection.GetControl(), mousePosX, mousePosY);
				if (handleIndex != -1) { currentSelection.GrabHandle(handleIndex, mousePosX, mousePosY); }
			}
			// Check if mouse clicked on a control
			if (!currentSelection.IsGrabbingControl() && !currentSelection.IsGrabbingHandle()) {
				GUIControl *clickedControl = m_EditorManager->ControlUnderMouse(m_EditorManager->GetRootControl(), mousePosX, mousePosY);
				if (clickedControl && clickedControl != m_EditorManager->GetRootControl()) {
					currentSelection.GrabControl(clickedControl, mousePosX, mousePosY);

					m_EditorManager->UpdateControlProperties(currentSelection.GetControl());
					if (currentSelection.GetControl()->GetControlType() == "COLLECTIONBOX") {
						m_EditorManager->SelectActiveControlInParentList(currentSelection.GetControl());
					} else {
						m_EditorManager->SelectActiveControlInChildrenList(currentSelection.GetControl());
					}
					// Remove focus from the currently focused editor manager element between selection changes so the currently selected property page line doesn't persist between selection changes
					m_EditorManager->RemoveFocus();
				} else if (clickedControl == m_EditorManager->GetRootControl()) {
					// Deselect control if the workspace was clicked
					m_EditorManager->ClearCurrentSelection();
					m_EditorManager->SelectActiveControlInParentList(m_EditorManager->GetRootControl());
				}
			}
		}

		if (mouse_z != 0) {
			if (m_EditorManager->GetControlManager()->GetControlUnderPoint(mousePosX, mousePosY, m_EditorManager->GetControlManager()->GetControl("CollectionBoxList"))) {
				dynamic_cast<GUIListPanel *>(m_EditorManager->GetControlManager()->GetControl("CollectionBoxList"))->OnMouseWheelChange(mousePosX, mousePosY, 0, mouse_z);
			} else if (m_EditorManager->GetControlManager()->GetControlUnderPoint(mousePosX, mousePosY, m_EditorManager->GetControlManager()->GetControl("ControlsInCollectionBoxList"))) {
				dynamic_cast<GUIListPanel *>(m_EditorManager->GetControlManager()->GetControl("ControlsInCollectionBoxList"))->OnMouseWheelChange(mousePosX, mousePosY, 0, mouse_z);
			}
			position_mouse_z(0);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::ProcessKeyboardInput() {
		// Handle keyboard input directly from Allegro instead of through AllegroInput to make life easier.
		int emptyStates = 0;
		for (int i = 0; i < KEY_MAX; ++i) {
			if (key[i] == 0) { ++emptyStates; }
			m_KeyStates.at(i) = key[i];
		}
		// Skip processing if no keyboard input
		if (emptyStates < KEY_MAX) {
			int pressed = -1;

			bool modCtrl = m_KeyStates.at(KEY_LCONTROL) == pressed || m_KeyStates.at(KEY_RCONTROL) == pressed;
			bool modShift = m_KeyStates.at(KEY_LSHIFT) == pressed || m_KeyStates.at(KEY_RSHIFT) == pressed;

			if (m_KeyStates.at(KEY_ALT) && m_KeyStates.at(KEY_F4)) { OnQuitButton(); }

			// Escape key - Undo any grab
			if (m_KeyStates.at(KEY_ESC) == pressed) { m_EditorManager->ClearCurrentSelection(); }

			const EditorSelection &currentSelection = m_EditorManager->GetCurrentSelection();

			if (modCtrl) {
				if (m_KeyStates.at(KEY_C) == pressed && m_PrevKeyStates.at(KEY_C) != pressed) {
					m_EditorManager->StoreCurrentSelectionCopyInfo();
				} else if (m_KeyStates.at(KEY_V) == pressed && m_PrevKeyStates.at(KEY_V) != pressed) {
					m_EditorManager->AddNewControlFromStoredCopyInfo();
				}

				if (m_KeyStates.at(KEY_S) == pressed) {
					OnSaveButton(modShift ? true : false);
				} else if (m_KeyStates.at(KEY_O) == pressed) {
					OnLoadButton(modShift ? true : false);
				}

				if (m_KeyStates.at(KEY_0) == pressed) {
					m_EditorManager->GetWorkspaceManager()->ChangeSkin("Assets", "EditorSkin.ini");
				} else if (m_KeyStates.at(KEY_1) == pressed) {
					m_EditorManager->GetWorkspaceManager()->ChangeSkin("Assets/Workspace", "SkinBlue.ini");
				} else if (m_KeyStates.at(KEY_2) == pressed) {
					m_EditorManager->GetWorkspaceManager()->ChangeSkin("Assets/Workspace", "SkinGreen.ini");
				} else if (m_KeyStates.at(KEY_3) == pressed) {
					m_EditorManager->GetWorkspaceManager()->ChangeSkin("Assets/Workspace", "SkinBrown.ini");
				} else if (m_KeyStates.at(KEY_4) == pressed) {
					m_EditorManager->GetWorkspaceManager()->ChangeSkin("Assets/Workspace", "SkinGray.ini");
				}
			}

			if (currentSelection.GetControl() && !m_EditorManager->GetPropertyPage()->HasTextFocus()) {
				if (m_KeyStates.at(KEY_DEL) == pressed) {
					m_EditorManager->RemoveControl(currentSelection.GetControl());
				} else {
					bool selectionNudged = false;
					if (m_KeyStates.at(KEY_UP) == pressed && m_PrevKeyStates.at(KEY_UP) != pressed) {
						selectionNudged = currentSelection.NudgeSelection(EditorSelection::NudgeDirection::NudgeUp, modShift);
					} else if (m_KeyStates.at(KEY_DOWN) == pressed && m_PrevKeyStates.at(KEY_DOWN) != pressed) {
						selectionNudged = currentSelection.NudgeSelection(EditorSelection::NudgeDirection::NudgeDown, modShift);
					}
					if (m_KeyStates.at(KEY_LEFT) == pressed && m_PrevKeyStates.at(KEY_LEFT) != pressed) {
						selectionNudged = currentSelection.NudgeSelection(EditorSelection::NudgeDirection::NudgeLeft, modShift);
					} else if (m_KeyStates.at(KEY_RIGHT) == pressed && m_PrevKeyStates.at(KEY_RIGHT) != pressed) {
						selectionNudged = currentSelection.NudgeSelection(EditorSelection::NudgeDirection::NudgeRight, modShift);
					}
					if (selectionNudged) { m_UnsavedChanges = m_EditorManager->UpdateControlProperties(currentSelection.GetControl()); }
				}
			}
		}
		m_PrevKeyStates = m_KeyStates;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorApp::UpdateEditor() {
		m_EditorManager->GetControlManager()->Update();
		GUIEvent editorEvent;
		while (m_EditorManager->GetControlManager()->GetEvent(&editorEvent)) {
			std::string controlName = editorEvent.GetOrigin()->GetName();
			switch (editorEvent.GetType()) {
				case GUIEventType::Command:
					if (controlName == "NewButton") {
						OnNewButton();
					} else if (controlName == "LoadButton") {
						OnLoadButton();
					} else if (controlName == "AddButton") {
						OnLoadButton(true);
					} else if (controlName == "SaveButton") {
						OnSaveButton();
					} else if (controlName == "SaveAsButton") {
						OnSaveButton(true);
					} else if (controlName == "QuitButton") {
						OnQuitButton();
					} else if (controlName.substr(0, 2).compare("C_") == 0) {
						m_UnsavedChanges = m_EditorManager->AddNewControl(editorEvent);
					}
					break;
				case GUIEventType::Notification:
					if (controlName == "PropertyPage" && editorEvent.GetMsg() == GUIEventCode::Enter) {
						m_UnsavedChanges = m_EditorManager->UpdateControlProperties(m_EditorManager->GetCurrentSelection().GetControl(), true);
						m_EditorManager->UpdateCollectionBoxList();
						m_EditorManager->UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(m_EditorManager->GetCurrentSelection().GetControl()));
						m_EditorManager->RemoveFocus();
					} else if (controlName == "CollectionBoxList" && editorEvent.GetMsg() == GUIEventCode::MouseDown) {
						m_EditorManager->SelectActiveControlFromParentList();
					} else if (controlName == "ControlsInCollectionBoxList" && editorEvent.GetMsg() == GUIEventCode::MouseDown) {
						m_EditorManager->SelectActiveControlFromChildrenList();
					} else if (controlName == "GridSizeTextBox" && editorEvent.GetMsg() == GUIEventCode::Enter) {
						m_EditorManager->UpdateSnapGridSize(editorEvent);
					} else if (controlName == "SnapCheckBox") {
						EditorSelection::s_SnapToGrid = dynamic_cast<GUICheckbox *>(editorEvent.GetOrigin())->GetCheck() == GUICheckbox::State::Checked;
						m_EditorManager->RemoveFocus();
					} else if (controlName == "ZoomCheckBox") {
						m_ZoomWorkspace = (dynamic_cast<GUICheckbox *>(editorEvent.GetOrigin()))->GetCheck() == GUICheckbox::State::Checked;
						show_mouse(m_ZoomWorkspace ? nullptr : screen);
						m_EditorManager->RemoveFocus();
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
			show_mouse(m_ZoomWorkspace ? nullptr : screen);
			m_WindowResized = false;
		}
		clear_to_color(m_BackBuffer, 0);

		m_EditorManager->GetEditorBase()->Draw(m_Screen.get());
		m_EditorManager->GetWorkspaceManager()->Draw();
		m_EditorManager->GetCurrentSelection().DrawSelectionBox(m_Screen.get(), m_Input.get());
		m_EditorManager->GetLeftColumn()->Draw(m_Screen.get());
		m_EditorManager->GetRightColumn()->Draw(m_Screen.get());
		m_EditorManager->GetToolBar()->Draw(m_Screen.get());

		if (m_ZoomWorkspace) {
			m_EditorManager->GetControlManager()->DrawMouse();

			stretch_blit(m_BackBuffer, m_ZoomBuffer, m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY(), m_EditorManager->GetWorkspaceWidth(), m_EditorManager->GetWorkspaceHeight(), 0, 0, m_EditorManager->GetWorkspaceWidth() * 2, m_EditorManager->GetWorkspaceHeight() * 2);
			blit(m_ZoomBuffer, m_BackBuffer, 0, 0, m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY(), m_EditorManager->GetWorkspaceWidth() * 2, m_EditorManager->GetWorkspaceHeight() * 2);
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, screen->w, screen->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorApp::OnNewButton() {
		if (m_UnsavedChanges) {
			int result = EditorUtil::DisplayDialogBox("Save changes made?", win_get_window());
			if (result == 0) {
				return;
			} else if (result == 1) {
				OnSaveButton();
			}
		}
		m_ActiveFileName.clear();
		m_EditorManager->ClearCurrentSelection();
		m_EditorManager->GetWorkspaceManager()->ClearAllControls();
		m_EditorManager->CreateRootControl();
		m_EditorManager->UpdateCollectionBoxList();
		m_EditorManager->UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(m_EditorManager->GetRootControl()));
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
			m_EditorManager->GetWorkspaceManager()->LoadLayout(newFilename, addControls);
			m_ActiveFileName = newFilename;

			GUIControl *newRootControl = m_EditorManager->GetWorkspaceManager()->GetControlList()->front();
			newRootControl->Move(m_EditorManager->GetWorkspacePosX(), m_EditorManager->GetWorkspacePosY());
			newRootControl->StoreProperties();

			GUIProperties newRootControlProps;
			newRootControlProps.OverwriteProperties(newRootControl->GetProperties(), true);
			newRootControl->BuildProperties(&newRootControlProps);

			m_EditorManager->ClearCurrentSelection();
			m_EditorManager->SetRootControl(newRootControl);
			m_UnsavedChanges = false;

			m_EditorManager->UpdateCollectionBoxList();
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

		m_EditorManager->GetWorkspaceManager()->SaveLayout(m_ActiveFileName);

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
