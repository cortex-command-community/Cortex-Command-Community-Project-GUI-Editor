#include "GUIEditorApp.h"
#include "GUIEditorUtil.h"

#include "GUIButton.h"
#include "GUICheckbox.h"
#include "AllegroBitmap.h"
#include "RTEError.h"

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Quit Handler for Allegro.
	/// </summary>
	void QuitHandler() { g_GUIEditor.OnQuitButton(); }

	/// <summary>
	/// 
	/// </summary>
	void ResizeHandler(RESIZE_DISPLAY_EVENT *resizeInfo) { g_GUIEditor.OnWindowResize(resizeInfo); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::Clear() {
		m_WindowResized = false;
		m_Quit = false;
		m_ResX = 1024;
		m_ResY = 768;
		m_BackBuffer = nullptr;
		m_ControlManager = nullptr;
		m_EditorManager = nullptr;
		m_PropertyPage = nullptr;
		m_ActiveBoxList = nullptr;
		m_RootControl = nullptr;
		m_EditorBase = nullptr;
		m_Filename.clear();
		m_UnsavedChanges = false;
		m_SnapToGrid = true;
		m_GridSize = 5;
		m_RootOriginX = 300;
		m_RootOriginY = 120;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Initialize() {
		set_color_depth(32);
		set_color_conversion(COLORCONV_MOST);
		set_window_title("Cortex Command: GUI Editor");
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_ResX, m_ResY, 0, 0);
		set_close_button_callback(QuitHandler);
		set_resize_callback(ResizeHandler);

		// Don't want to deal with recreating the backbuffer on window resize so just create one as large as the screen.
		m_BackBuffer = create_bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		clear_to_color(m_BackBuffer, 0);

		m_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		m_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI

		m_ControlManager = std::make_unique<GUIControlManager>();
		m_ControlManager->Create(m_Screen.get(), m_Input.get(), "Assets");

		m_EditorManager = std::make_unique<GUIControlManager>();
		m_EditorManager->Create(m_Screen.get(), m_Input.get(), "Assets");

		m_EditorManager->EnableMouse();

		m_EditorBase.reset(dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("base", "COLLECTIONBOX", nullptr, 0, 0, 1024, 768)));
		m_EditorBase->SetDrawBackground(true);
		m_EditorBase->SetDrawColor(makecol(32, 32, 32));
		m_EditorBase->SetDrawType(GUICollectionBox::Color);

		// Add an area showing the editing box
		GUICollectionBox *editorArea = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("editArea", "COLLECTIONBOX", m_EditorBase.get(), m_RootOriginX, m_RootOriginY, 640, 480));
		editorArea->SetDrawBackground(true);
		editorArea->SetDrawColor(makecol(64, 64, 64));
		editorArea->SetDrawType(GUICollectionBox::Color);

		// Add the root collection box for the edited document
		GUICollectionBox *rootBox = dynamic_cast<GUICollectionBox *>(m_ControlManager->AddControl("root", "COLLECTIONBOX", nullptr, m_RootOriginX, m_RootOriginY, 640, 480));
		rootBox->SetDrawBackground(false);
		m_RootControl.reset(rootBox);

		// Add the left tool buttons
		GUIButton *quitButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("QuitButton", "BUTTON", m_EditorBase.get(), 5, 5, 80, 20));
		quitButton->SetText("Quit");
		GUIButton *loadButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("LoadButton", "BUTTON", m_EditorBase.get(), 5, 30, 80, 20));
		loadButton->SetText("Load");
		GUIButton *addButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("AddButton", "BUTTON", m_EditorBase.get(), 90, 30, 40, 20));
		addButton->SetText("Add");
		GUIButton *saveButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("SaveButton", "BUTTON", m_EditorBase.get(), 5, 55, 80, 20));
		saveButton->SetText("Save");
		GUIButton *saveAsButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("SaveAsButton", "BUTTON", m_EditorBase.get(), 5, 80, 80, 20));
		saveAsButton->SetText("Save As");

		GUIButton *addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COLLECTIONBOX", "BUTTON", m_EditorBase.get(), 160, 5, 80, 20));
		addControlButton->SetText("COLLECTIONBOX");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LISTBOX", "BUTTON", m_EditorBase.get(), 160, 30, 80, 20));
		addControlButton->SetText("LISTBOX");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_BUTTON", "BUTTON", m_EditorBase.get(), 160, 55, 80, 20));
		addControlButton->SetText("BUTTON");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COMBOBOX", "BUTTON", m_EditorBase.get(), 160, 80, 80, 20));
		addControlButton->SetText("COMBOBOX");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LABEL", "BUTTON", m_EditorBase.get(), 250, 5, 80, 20));
		addControlButton->SetText("LABEL");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_CHECKBOX", "BUTTON", m_EditorBase.get(), 250, 30, 80, 20));
		addControlButton->SetText("CHECKBOX");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_TEXTBOX", "BUTTON", m_EditorBase.get(), 250, 55, 80, 20));
		addControlButton->SetText("TEXTBOX");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_RADIOBUTTON", "BUTTON", m_EditorBase.get(), 250, 80, 80, 20));
		addControlButton->SetText("RADIOBUTTON");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SCROLLBAR", "BUTTON", m_EditorBase.get(), 340, 5, 80, 20));
		addControlButton->SetText("SCROLLBAR");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SLIDER", "BUTTON", m_EditorBase.get(), 340, 30, 80, 20));
		addControlButton->SetText("SLIDER");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_PROGRESSBAR", "BUTTON", m_EditorBase.get(), 340, 55, 80, 20));
		addControlButton->SetText("PROGRESSBAR");
		addControlButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_TAB", "BUTTON", m_EditorBase.get(), 340, 80, 80, 20));
		addControlButton->SetText("TAB");

		m_PropertyPage.reset(dynamic_cast<GUIPropertyPage *>(m_EditorManager->AddControl("props", "PROPERTYPAGE", m_EditorBase.get(), 5, 120, 250, 250)));

		m_ActiveBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorManager->AddControl("active", "LISTBOX", m_EditorBase.get(), 5, 400, 150, 250)));

		GUICheckbox *snapCheckbox = dynamic_cast<GUICheckbox *>(m_EditorManager->AddControl("snap", "CHECKBOX", m_EditorBase.get(), 450, 10, 80, 16));
		snapCheckbox->SetText("Snap");
		snapCheckbox->SetCheck(GUICheckbox::Checked);

		ClearSelection();

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Update() {
		m_EditorManager->Update();

		// Process the editor events
		GUIEvent event;
		while (m_EditorManager->GetEvent(&event)) {
			switch (event.GetType()) {
				case GUIEvent::Command:
					if (event.GetControl()->GetName() == "QuitButton") {
						OnQuitButton();
						break;
					}
					if (event.GetControl()->GetName() == "LoadButton") {
						OnLoadButton();
						break;
					}
					if (event.GetControl()->GetName() == "AddButton") {
						OnLoadButton(true);
						break;
					}
					if (event.GetControl()->GetName() == "SaveButton") {
						OnSaveButton();
						break;
					}
					if (event.GetControl()->GetName() == "SaveAsButton") {
						OnSaveAsButton();
						break;
					}

					// Add a control
					if (event.GetControl()->GetName().substr(0, 2).compare("C_") == 0) {
						std::string Class = (dynamic_cast<GUIButton *>(event.GetControl()))->GetText();
						GUIControl *Parent = m_RootControl.get();

						// Is the focused control a container?
						if (m_SelectionInfo.Control && m_SelectionInfo.Control->IsContainer()) { Parent = m_SelectionInfo.Control; }

						// Find a suitable control name
						std::string Name = GenerateControlName(Class/*.substr(2)*/);

						if (Parent) { m_ControlManager->AddControl(Name, Class, Parent, 0, 0, -1, -1); }

						break;
					}

					break;

				// Notification
				case GUIEvent::Notification:

					// Property Page changed
					if (event.GetControl()->GetName() == "props") {

						if (event.GetMsg() == GUIPropertyPage::Enter) {
							// Update the focused control properties

							// Apply the properties
							GUIControl *C = m_SelectionInfo.Control;
							if (C) {
								C->ApplyProperties(m_PropertyPage->GetPropertyValues());
								// Update the active box list in case the name of a top-level box changed
								UpdateActiveBoxList();
							}

							m_UnsavedChanges = true;
						}

						if (event.GetMsg() == GUIPropertyPage::Changed) {
							// The properties are dirty and need to be updated
							m_UnsavedChanges = true;
						}
					}

					// Active Box changed
					if (event.GetControl()->GetName() == "active" && event.GetMsg() == GUIListBox::MouseDown) {
						const GUIListPanel::Item *pItem = m_ActiveBoxList->GetSelected();
						if (pItem) {
							// Try to find the box of that name, and select it
							GUIControl *pBoxControl = m_ControlManager->GetControl(pItem->m_Name);
							if (pBoxControl) {
								m_SelectionInfo.GrabbedControl = false;
								m_SelectionInfo.GrabbedHandle = false;
								m_SelectionInfo.Control = pBoxControl;
							}
						}
						// Deselection if clicked on no list item
						else {
							m_SelectionInfo.GrabbedControl = false;
							m_SelectionInfo.GrabbedHandle = false;
							m_SelectionInfo.Control = nullptr;
						}
					}

					// Snap
					if (event.GetControl()->GetName() == "snap") { m_SnapToGrid = (dynamic_cast<GUICheckbox *>(event.GetControl()))->GetCheck() == GUICheckbox::Checked; }
					break;

				default:
					break;
			}
		}

		m_EditorManager->Draw();

		m_ControlManager->Draw();

		// Process the editor
		ProcessEditor();

		if (m_SelectionInfo.Control) { DrawSelectedControl(m_SelectionInfo.Control); }

		m_EditorManager->DrawMouse();

		return !m_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo) {
		m_EditorBase.get()->Resize(resizeInfo->new_w, resizeInfo->new_h);
		m_WindowResized = true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::FlipFrameBuffers() {
		if (m_WindowResized) {
			acknowledge_resize();
			m_WindowResized = false;
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, m_BackBuffer->w, m_BackBuffer->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ClearBackBuffer() const {
		clear_to_color(m_BackBuffer, 0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnQuitButton() {
		if (m_UnsavedChanges) {
			int result = GUIEditorUtil::QuitMessageBox("Save changes made?", "Cortex Command: GUI Editor");
			if (result == 1) {
				OnSaveButton();
				m_Quit = true;
			}
			if (result == -1) { m_Quit = true; }
		} else {
			m_Quit = true;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnLoadButton(bool addControls) {
		std::string filename;
		if (GUIEditorUtil::DisplayLoadGUIFile(&filename, "Cortex Command: GUI Editor")) {
			m_ControlManager->Load(filename, addControls);

			GUIControl *control = m_ControlManager->GetControlList()->front();
			m_RootControl.reset(control);

			control->Move(m_RootOriginX, m_RootOriginY);

			control->StoreProperties();

			GUIProperties controlProps;
			controlProps.Update(control->GetProperties(), true);
			control->GetPanel()->BuildProperties(&controlProps);

			// Clear settings
			m_UnsavedChanges = false;
			ClearSelection();
			m_PropertyPage->ClearValues();

			UpdateActiveBoxList();

			m_Filename = filename;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnSaveAsButton() {
		std::string filename;
		if (GUIEditorUtil::DisplaySaveGUIFile(&filename, "Cortex Command: GUI Editor")) {
			// Move the root object to the origin before saving
			m_RootControl->Move(0, 0);

			m_ControlManager->Save(filename);

			// Move it back
			m_RootControl->Move(m_RootOriginX, m_RootOriginY);

			m_Filename = filename;

			m_UnsavedChanges = false;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnSaveButton() {
		if (m_Filename.size() == 0) {
			OnSaveAsButton();
		} else {
			// Move the root object to the origin before saving
			m_RootControl->Move(0, 0);

			m_ControlManager->Save(m_Filename);

			// Move it back
			m_RootControl->Move(m_RootOriginX, m_RootOriginY);

			m_UnsavedChanges = false;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ProcessEditor() {
		std::array<int, 3> mouseEvents;
		std::array<int, 3> mouseStates;
		int mousePosX;
		int mousePosY;
		m_Input->GetMouseButtons(mouseEvents.data(), mouseStates.data());
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		std::array<unsigned char, 256> keyboardBuffer;
		m_Input->GetKeyboard(keyboardBuffer.data());

		// Delete key
		if (keyboardBuffer.at(GUIInput::Key_Delete) == GUIInput::Pushed && !m_PropertyPage->HasTextFocus() && m_SelectionInfo.Control) {
			m_ControlManager->RemoveControl(m_SelectionInfo.Control->GetName(), true);
			m_SelectionInfo.Control = nullptr;
			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;

			m_PropertyPage->ClearValues();
		}

		// Escape key
		if (keyboardBuffer.at(GUIInput::Key_Escape) == GUIInput::Pushed) {
			// Undo any grab
			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
		}


		// If click released
		if (mouseEvents.at(0) == GUIInput::Released) {
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
			if (!ControlUnderMouse(m_PropertyPage.get(), mousePosX, mousePosY) && m_SelectionInfo.Control) {
				m_SelectionInfo.Control->StoreProperties();

				GUIProperties properties;
				properties.Update(m_SelectionInfo.Control->GetProperties(), true);
				m_SelectionInfo.Control->GetPanel()->BuildProperties(&properties);
				m_PropertyPage->SetPropertyValues(&properties);

				m_UnsavedChanges = true;
			}

			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
			m_SelectionInfo.TriggerGrab = false;
		}


		// Check for grabbing handles
		if (!m_SelectionInfo.GrabbedControl && m_SelectionInfo.Control && mouseEvents.at(0) == GUIInput::Pushed) {
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
		if (!m_SelectionInfo.GrabbedControl && !m_SelectionInfo.GrabbedHandle && mouseEvents.at(0) == GUIInput::Pushed) {
			GUIControl *control = ControlUnderMouse(m_RootControl.get(), mousePosX, mousePosY);
			if (control && control != m_RootControl.get()) {
				int xPos;
				int yPos;
				int width;
				int height;
				control->GetControlRect(&xPos, &yPos, &width, &height);

				m_SelectionInfo.GrabbedHandle = false;

				m_SelectionInfo.GrabbedControl = true;
				m_SelectionInfo.GrabX = xPos - mousePosX;
				m_SelectionInfo.GrabY = yPos - mousePosY;
				m_SelectionInfo.ClickX = mousePosX;
				m_SelectionInfo.ClickY = mousePosY;

				m_SelectionInfo.Control = control;

				// Set the properties
				control->StoreProperties();

				GUIProperties properties;
				properties.Update(control->GetProperties(), true);
				control->GetPanel()->BuildProperties(&properties);
				m_PropertyPage->SetPropertyValues(&properties);

			} else if (control == m_RootControl.get()) {
				// Unselect control
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = nullptr;

				m_PropertyPage->ClearValues();
			}

			// Update the active box list in case we selected/deselected a top level collection box
			UpdateActiveBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateActiveBoxList() const {
		// Clear the list so we can repopulate it
		m_ActiveBoxList->ClearList();

		// Go through all the top-level (directly under root) controls and add only the ControlBoxs to the list here
		std::vector<GUIControl *> *controls = m_ControlManager->GetControlList();
		GUICollectionBox *collectionBox = nullptr;

		for (std::vector<GUIControl *>::iterator itr = controls->begin(); itr != controls->end(); itr++) {
			// Look for CollectionBoxes with the root control as parent
			if ((collectionBox = dynamic_cast<GUICollectionBox *>(*itr)) && collectionBox->GetParent() == m_RootControl.get()) {
				m_ActiveBoxList->AddItem(collectionBox->GetName());
				// Check if this is selected in the editor, and if so, select it in the list too
				if (collectionBox == m_SelectionInfo.Control) { m_ActiveBoxList->SetSelectedIndex(m_ActiveBoxList->GetItemList()->size() - 1); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIEditorApp::ControlUnderMouse(GUIControl *parent, int mousePosX, int mousePosY) {
		assert(parent);

		// Clicked on the parent?
		int xPos;
		int yPos;
		int width;
		int height;
		parent->GetControlRect(&xPos, &yPos, &width, &height);

		if (mousePosX < xPos || mousePosX > xPos + width) {
			return nullptr;
		}
		if (mousePosY < yPos || mousePosY > yPos + height) {
			return nullptr;
		}

		// Check children
		std::vector<GUIControl *> *list = parent->GetChildren();
		std::vector<GUIControl *>::reverse_iterator it;

		assert(list);

		for (it = list->rbegin(); it != list->rend(); it++) {
			GUIControl *control = ControlUnderMouse(*it, mousePosX, mousePosY);
			if (control) {
				return control;
			}
		}

		// Return this
		return parent;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const {
		int xPos;
		int yPos;
		int width;
		int height;
		control->GetControlRect(&xPos, &yPos, &width, &height);

		int regionSize = 6;
		int handle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(mousePosX, mousePosY, xPos - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (i != 1 && MouseInsideBox(mousePosX, mousePosY, xPos + width / 2 - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (MouseInsideBox(mousePosX, mousePosY, xPos + width - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;
		}

		// Not over any handle
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectedControl(GUIControl *control) {
		int handleSize = 6;

		assert(control);

		int mousePosX;
		int mousePosY;
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		int xPos;
		int yPos;
		int width;
		int height;
		control->GetControlRect(&xPos, &yPos, &width, &height);

		// If we've grabbed the control, draw the selection lines where the mouse is
		if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab) {
			xPos = mousePosX + m_SelectionInfo.GrabX;
			yPos = mousePosY + m_SelectionInfo.GrabY;

			xPos = ProcessSnapCoord(xPos);
			yPos = ProcessSnapCoord(yPos);
		}

		// Grabbed handles
		if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab) { CalculateHandleResize(mousePosX, mousePosY, &xPos, &yPos, &width, &height); }

		GUIRect rect;
		SetRect(&rect, xPos - 6, yPos - 6, xPos + width + 6, yPos + height + 6);
		m_Screen->GetBitmap()->SetClipRect(&rect);

		m_Screen->GetBitmap()->DrawRectangle(xPos, yPos, width, height, 0xFFCCCCCC, false);

		// Draw the handles
		for (int i = 0; i < 3; i++) {
			DrawSelectionHandle(xPos, yPos + i * (height / 2), handleSize, handleSize);
			if (i != 1) { DrawSelectionHandle(xPos + width / 2, yPos + i * (height / 2), handleSize, handleSize); }
			DrawSelectionHandle(xPos + width, yPos + i * (height / 2), handleSize, handleSize);
		}
		m_Screen->GetBitmap()->SetClipRect(nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectionHandle(int xPos, int yPos, int width, int height) const {
		m_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFF000000, true);
		m_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFFFFFFFF, false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ClearSelection() {
		m_SelectionInfo.GrabbedControl = false;
		m_SelectionInfo.GrabbedHandle = false;
		m_SelectionInfo.TriggerGrab = false;

		m_SelectionInfo.Control = nullptr;
		m_SelectionInfo.HandleIndex = 0;

		m_SelectionInfo.GrabX = 0;
		m_SelectionInfo.GrabY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::MouseInsideBox(int mousePosX, int mousePosY, int xPos, int yPos, int width, int height) const {
		return (mousePosX >= xPos && mousePosX <= xPos + width && mousePosY >= yPos && mousePosY <= yPos + height) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::CalculateHandleResize(int mousePosX, int mousePosY, int *xPos, int *yPos, int *width, int *height) {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		m_SelectionInfo.Control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);
		GUIControl *parent = m_SelectionInfo.Control->GetParent();

		int minSize = 5;

		int parentPosX;
		int parentPosY;
		int parentWidth;
		int parentHeight;
		parent->GetControlRect(&parentPosX, &parentPosY, &parentWidth, &parentHeight);

		// Left Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 3 || m_SelectionInfo.HandleIndex == 6) {
			int diff = mousePosX - m_SelectionInfo.GrabX;
			if (controlPosX + diff < parentPosX) { diff = parentPosX - controlPosX; }
			if (controlWidth - diff < minSize) { diff = controlWidth - minSize; }

			diff = ProcessSnapCoord(diff);

			controlPosX += diff;
			controlWidth -= diff;
		}
		// Top Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 1 || m_SelectionInfo.HandleIndex == 2) {
			int diff = mousePosY - m_SelectionInfo.GrabY;
			if (controlPosY + diff < parentPosY) { diff = parentPosY - controlPosY; }
			if (controlHeight - diff < minSize) { diff = controlHeight - minSize; }

			diff = ProcessSnapCoord(diff);

			controlPosY += diff;
			controlHeight -= diff;
		}
		// Right Resize
		if (m_SelectionInfo.HandleIndex == 2 || m_SelectionInfo.HandleIndex == 5 || m_SelectionInfo.HandleIndex == 8) {
			int diff = mousePosX - m_SelectionInfo.GrabX;
			if (controlPosX + controlWidth + diff > parentPosX + parentWidth) { diff = (parentPosX + parentWidth) - (controlPosX + controlWidth); }

			diff = ProcessSnapCoord(diff);

			controlWidth += diff;
		}
		// Bottom Resize
		if (m_SelectionInfo.HandleIndex == 6 || m_SelectionInfo.HandleIndex == 7 || m_SelectionInfo.HandleIndex == 8) {
			int diff = mousePosY - m_SelectionInfo.GrabY;
			if (controlPosY + controlHeight + diff > parentPosY + parentHeight) { diff = (parentPosY + parentHeight) - (controlPosY + controlHeight); }

			diff = ProcessSnapCoord(diff);

			controlHeight += diff;
		}

		controlWidth = std::max(controlWidth, minSize);
		controlHeight = std::max(controlHeight, minSize);

		*xPos = controlPosX;
		*yPos = controlPosY;
		*width = controlWidth;
		*height = controlHeight;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIEditorApp::GenerateControlName(std::string controlType) const {
		// 10,000 should be enough
		for (int i = 1; i < 10000; i++) {
			std::string name = controlType;
			// Use a lower case version of the string
			std::transform(name.begin(), name.end(), name.begin(), tolower);
			name.append(std::to_string(i));

			// Check if this name exists
			std::vector<GUIControl *> *controlList = m_ControlManager->GetControlList();
			std::vector<GUIControl *>::iterator it;

			bool found = false;

			for (it = controlList->begin(); it != controlList->end(); it++) {
				GUIControl *control = *it;
				if (control->GetName().compare(name) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				return name;
			}
		}
		// Error: Tried all 10,000 numbers
		return controlType;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::ProcessSnapCoord(int position) const {
		if (m_SnapToGrid) {
			float fraction = static_cast<float>(position) / static_cast<float>(m_GridSize);
			float value = std::floor(fraction);
			fraction -= value;
			fraction = (fraction >= 0.5F) ? 1 : 0;
			position = static_cast<int>((value + fraction)) * m_GridSize;
		}
		return position;
	}
}
