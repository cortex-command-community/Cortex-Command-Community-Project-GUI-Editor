#include "EditorManager.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "GUILabel.h"
#include "GUITextBox.h"

#include "allegro.h"

namespace RTEGUI {

	EditorSelection EditorManager::s_SelectionInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::Initialize(GUIScreen *screen, GUIInput *input, const std::string &skinDir, const std::string &skinFilename) {
		m_EditorControlManager = std::make_unique<GUIControlManager>();
		m_EditorControlManager->Create(screen, input, skinDir, skinFilename);
		m_EditorControlManager->EnableMouse();

		m_WorkspaceManager = std::make_unique<GUIControlManager>();
		m_WorkspaceManager->Create(screen, input, skinDir, skinFilename);

		m_EditorBase.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("EditorBase", "COLLECTIONBOX", nullptr, 0, 0, screen->GetBitmap()->GetWidth(), screen->GetBitmap()->GetHeight())));
		m_EditorBase->SetDrawBackground(true);
		m_EditorBase->SetDrawColor(makecol(32, 32, 32));
		m_EditorBase->SetDrawType(GUICollectionBox::Color);

		GUILabel *frameTimeLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("FrameTimer", "LABEL", m_EditorBase.get(), 300, 10, 100, 20));
		frameTimeLabel->SetText("Frame Time: 0");

		m_LeftColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("LeftColumn", "COLLECTIONBOX", nullptr, 0, 0, 290, screen->GetBitmap()->GetHeight())));
		m_LeftColumn->SetDrawBackground(true);
		m_LeftColumn->SetDrawColor(makecol(23, 23, 23));
		m_LeftColumn->SetDrawType(GUICollectionBox::Color);

		GUICollectionBox *filePanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("FilePanel", "COLLECTIONBOX", m_LeftColumn.get(), 5, 5, 270, 55));
		filePanel->SetDrawType(GUICollectionBox::Panel);

		GUIButton *toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("NewButton", "BUTTON", filePanel, 6, 5, 61, 20));
		toolboxButton->SetText("New");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("LoadButton", "BUTTON", filePanel, 71, 5, 62, 20));
		toolboxButton->SetText("Load");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("AddButton", "BUTTON", filePanel, 71, 30, 62, 20));
		toolboxButton->SetText("Add File");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("SaveButton", "BUTTON", filePanel, 137, 5, 62, 20));
		toolboxButton->SetText("Save");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("SaveAsButton", "BUTTON", filePanel, 137, 30, 62, 20));
		toolboxButton->SetText("Save As");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("QuitButton", "BUTTON", filePanel, 203, 5, 61, 20));
		toolboxButton->SetText("Quit");

		GUICollectionBox *editorControls = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("EditorControlsPanel", "COLLECTIONBOX", m_LeftColumn.get(), filePanel->GetRelXPos(), filePanel->GetRelYPos() + 65, 270, 155));
		editorControls->SetDrawType(GUICollectionBox::Panel);

		GUICollectionBox *elementPanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("NewElementPanel", "COLLECTIONBOX", editorControls, 5, 25, 260, 105));
		elementPanel->SetDrawType(GUICollectionBox::Panel);
		GUILabel *newElementLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("NewElementLabel", "LABEL", editorControls, elementPanel->GetRelXPos() + 5, elementPanel->GetRelYPos() - 20, 100, 20));
		newElementLabel->SetText("Add New Element :");

		GUIButton *elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_COLLECTIONBOX", "BUTTON", elementPanel, 5, 5, 80, 20));
		elementButton->SetText("CollectionBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_LISTBOX", "BUTTON", elementPanel, 5, 30, 80, 20));
		elementButton->SetText("ListBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_BUTTON", "BUTTON", elementPanel, 5, 55, 80, 20));
		elementButton->SetText("Button");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_COMBOBOX", "BUTTON", elementPanel, 5, 80, 80, 20));
		elementButton->SetText("ComboBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_LABEL", "BUTTON", elementPanel, 90, 5, 80, 20));
		elementButton->SetText("Label");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_CHECKBOX", "BUTTON", elementPanel, 90, 30, 80, 20));
		elementButton->SetText("CheckBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_TEXTBOX", "BUTTON", elementPanel, 90, 55, 80, 20));
		elementButton->SetText("TextBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_RADIOBUTTON", "BUTTON", elementPanel, 90, 80, 80, 20));
		elementButton->SetText("RadioButton");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_SCROLLBAR", "BUTTON", elementPanel, 175, 5, 80, 20));
		elementButton->SetText("ScrollBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_SLIDER", "BUTTON", elementPanel, 175, 30, 80, 20));
		elementButton->SetText("Slider");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_PROGRESSBAR", "BUTTON", elementPanel, 175, 55, 80, 20));
		elementButton->SetText("ProgressBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_TAB", "BUTTON", elementPanel, 175, 80, 80, 20));
		elementButton->SetText("Tab");

		GUILabel *gridSizeLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("GridSizeLabel", "LABEL", editorControls, 10, 135, 90, 15));
		gridSizeLabel->SetText("Grid/Nudge Size :");
		GUITextBox *gridSizeTextbox = dynamic_cast<GUITextBox *>(m_EditorControlManager->AddControl("GridSizeTextBox", "TEXTBOX", editorControls, gridSizeLabel->GetRelXPos() + 90, gridSizeLabel->GetRelYPos(), 30, 15));
		gridSizeTextbox->SetText(std::to_string(EditorSelection::s_SnapGridSize));

		GUICheckbox *snapCheckbox = dynamic_cast<GUICheckbox *>(m_EditorControlManager->AddControl("SnapCheckBox", "CHECKBOX", editorControls, gridSizeLabel->GetRelXPos() + 130, gridSizeLabel->GetRelYPos(), 75, 15));
		snapCheckbox->SetText("Snap to Grid");
		snapCheckbox->SetCheck(GUICheckbox::Checked);

		GUICheckbox *zoomCheckBox = dynamic_cast<GUICheckbox *>(m_EditorControlManager->AddControl("ZoomCheckBox", "CHECKBOX", editorControls, snapCheckbox->GetRelXPos() + 85, snapCheckbox->GetRelYPos(), 75, 15));
		zoomCheckBox->SetText("Zoom");
		zoomCheckBox->SetCheck(GUICheckbox::Unchecked);

		GUICollectionBox *propertyPagePanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("PropertyPagePanel", "COLLECTIONBOX", m_LeftColumn.get(), 5, editorControls->GetYPos() + editorControls->GetHeight() + 10, 270, 360));
		propertyPagePanel->SetDrawType(GUICollectionBox::Panel);

		GUILabel *propertyPageLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("PropertyPageLabel", "LABEL", propertyPagePanel, 10, 5, 110, 20));
		propertyPageLabel->SetText("Element Properties :");
		m_PropertyPage.reset(dynamic_cast<GUIPropertyPage *>(m_EditorControlManager->AddControl("PropertyPage", "PROPERTYPAGE", m_LeftColumn.get(), 10, 260, 260, 330)));

		m_RightColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("RightColumn", "COLLECTIONBOX", nullptr, 990, 0, 290, screen->GetBitmap()->GetHeight())));
		m_RightColumn->SetDrawBackground(true);
		m_RightColumn->SetDrawColor(makecol(23, 23, 23));
		m_RightColumn->SetDrawType(GUICollectionBox::Color);

		GUICollectionBox *listsPanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("ControlListsPanel", "COLLECTIONBOX", m_RightColumn.get(), 15, 5, m_RightColumn->GetWidth() - 20, 590));
		listsPanel->SetDrawType(GUICollectionBox::Panel);

		GUILabel *collectionBoxListLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("CollectionBoxListLabel", "LABEL", listsPanel, 10, 5, 100, 20));
		collectionBoxListLabel->SetText("Active Containers :");
		m_CollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorControlManager->AddControl("CollectionBoxList", "LISTBOX", listsPanel, 5, collectionBoxListLabel->GetHeight() + 5, 260, 230)));
		m_CollectionBoxList->SetMouseScrolling(true);

		GUILabel *controlsInCollectionBoxListLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("ControlsInCollectionBoxListLabel", "LABEL", listsPanel, 10, collectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetHeight() + 10, 110, 20));
		controlsInCollectionBoxListLabel->SetText("Container Elements :");
		m_ControlsInCollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorControlManager->AddControl("ControlsInCollectionBoxList", "LISTBOX", listsPanel, 5, controlsInCollectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetRelYPos() + 235, 260, 305)));
		m_ControlsInCollectionBoxList->SetMouseScrolling(true);

		// Create the workspace area showing the editing box
		GUICollectionBox *workspace = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("Workspace", "COLLECTIONBOX", m_EditorBase.get(), m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		workspace->SetDrawBackground(true);
		workspace->SetDrawColor(makecol(64, 64, 64));
		workspace->SetDrawType(GUICollectionBox::Color);

		CreateRootControl();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::CreateRootControl() {
		GUICollectionBox *rootCollectionBox = dynamic_cast<GUICollectionBox *>(m_WorkspaceManager->AddControl("root", "COLLECTIONBOX", nullptr, m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		rootCollectionBox->SetDrawBackground(false);
		m_RootControl = rootCollectionBox;
		m_CollectionBoxList->AddItem(m_RootControl->GetName());
		m_CollectionBoxList->SetSelectedIndex(0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::DisableZoomCheckbox() const {
		m_EditorControlManager->GetControl("ZoomCheckBox")->SetEnabled(false);
		m_EditorControlManager->GetControl("ZoomCheckBox")->SetVisible(false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SetFrameTimeLabelText(int64_t frameTime) const {
		dynamic_cast<GUILabel *>(m_EditorControlManager->GetControl("FrameTimer"))->SetText("Frame Time: " + std::to_string(frameTime) + "ms");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::AddNewControl(GUIEvent &editorEvent) {
		if (s_SelectionInfo.GetControl() && s_SelectionInfo.GetControl()->GetID() != "COLLECTIONBOX") {
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}
		std::string controlClass = editorEvent.GetControl()->GetName().substr(2, std::string::npos);
		std::string controlName = GenerateControlName(controlClass);

		GUIControl *parent = m_RootControl;

		// If the focused control is a container set it as parent so controls are added to it
		if (s_SelectionInfo.GetControl() && s_SelectionInfo.GetControl()->IsContainer()) { parent = s_SelectionInfo.GetControl(); }

		if (parent) { m_WorkspaceManager->AddControl(controlName, controlClass, parent, 0, 0, -1, -1); }

		UpdateCollectionBoxList();
		UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(parent));

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::RemoveControl(GUIControl *controlToRemove) const {
		m_WorkspaceManager->RemoveControl(controlToRemove->GetName(), true);
		if (controlToRemove->GetID() == "COLLECTIONBOX") {
			ClearCurrentSelection();
			UpdateCollectionBoxList();
		} else {
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
			UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(controlToRemove->GetParent()));
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string EditorManager::GenerateControlName(std::string controlType) const {
		for (int i = 1; i < 1000; i++) {
			std::string controlName = controlType;
			std::transform(controlName.begin(), controlName.end(), controlName.begin(), tolower);
			controlName.append(std::to_string(i));

			// Check if this name exists
			bool found = false;
			for (GUIControl *control : *m_WorkspaceManager->GetControlList()) {
				if (control->GetName() == controlName) {
					found = true;
					break;
				}
			}
			if (!found) {
				return controlName;
			}
		}
		return controlType;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlFromParentList() const {
		const GUIListPanel::Item *selectedItem = m_CollectionBoxList->GetSelected();

		if (selectedItem) {
			// Try to find the box of that name, and select it
			GUIControl *control = m_WorkspaceManager->GetControl(selectedItem->m_Name.substr(selectedItem->m_Name.find_first_not_of('\t'), std::string::npos));
			if (control) {
				// If the selected item is the root control don't grab it but proceed to populate the children list from it
				if (selectedItem->m_Name == m_RootControl->GetName()) {
					s_SelectionInfo.ClearSelection();
					m_PropertyPage->ClearValues();
				} else {
					s_SelectionInfo.ReleaseAnyGrabs();
					s_SelectionInfo.SetControl(control);
				}
				UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(control));
			}
		} else {
			// Deselection if clicked on no list item
			ClearCurrentSelection();
			// When nothing is selected populate the children list with the root control's children to show any "loose" controls
			UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(m_RootControl));
		}
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlFromChildrenList() const {
		const GUIListPanel::Item *selectedItem = m_ControlsInCollectionBoxList->GetSelected();
		if (selectedItem) {
			// Try to find the control of that name, and select it
			GUIControl *control = m_WorkspaceManager->GetControl(selectedItem->m_Name);
			if (control) {
				s_SelectionInfo.ReleaseAnyGrabs();
				s_SelectionInfo.SetControl(control);
			}
		} else {
			// Deselection if clicked on no list item
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlInParentList(GUIControl *control) const {
		// Check if this is selected in the editor and select it in the list too
		for (const GUIListBox::Item *listEntry : *m_CollectionBoxList->GetItemList()) {
			if (listEntry->m_Name.substr(listEntry->m_Name.find_first_not_of('\t'), std::string::npos) == control->GetName()) {
				m_CollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
		}
		UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(control));
		m_ControlsInCollectionBoxList->SetSelectedIndex(-1);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlInChildrenList(GUIControl *control) const {
		// Check if this is selected in the editor and select it's parent in the parent list and then select it in the children list
		SelectActiveControlInParentList(control->GetParent());
		for (const GUIListBox::Item *listEntry : *m_ControlsInCollectionBoxList->GetItemList()) {
			if (listEntry->m_Name == control->GetName()) {
				m_ControlsInCollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateCollectionBoxList() const {
		// Store the last collection box that was selected before the update so it can be selected if it still exists, if no selection then select root
		std::string lastSelection = (m_CollectionBoxList->GetSelectedIndex() != -1) ? m_CollectionBoxList->GetSelected()->m_Name : "";

		m_CollectionBoxList->ClearList();
		m_CollectionBoxList->AddItem(m_RootControl->GetName());

		// Lambda expression to recursively add lower-level CollectionBoxes belonging to the higher-level CollectionBoxes
		std::function<void(GUICollectionBox *, const std::string &)> recursiveAddItem = [&recursiveAddItem, this](GUICollectionBox *control, const std::string &indent) {
			m_CollectionBoxList->AddItem(indent + control->GetName());
			for (GUIControl *childControl : *control->GetChildren()) {
				if ((control = dynamic_cast<GUICollectionBox *>(childControl))) { recursiveAddItem(control, indent + "\t"); }
			}
		};

		GUICollectionBox *collectionBox = nullptr;
		for (GUIControl *control : *m_WorkspaceManager->GetControlList()) {
			if ((collectionBox = dynamic_cast<GUICollectionBox *>(control)) && collectionBox->GetParent() == m_RootControl) { recursiveAddItem(collectionBox, "\t"); }
		}

		for (const GUIListBox::Item *listEntry : *m_CollectionBoxList->GetItemList()) {
			if (listEntry->m_Name == lastSelection) {
				m_CollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
			m_CollectionBoxList->SetSelectedIndex(0);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateCollectionBoxChildrenList(GUICollectionBox *collectionBox) const {
		m_ControlsInCollectionBoxList->ClearList();

		// Go through all the top-level (directly under root) controls and add only the CollectionBoxes to the list here
		for (GUIControl *control : *collectionBox->GetChildren()) {
			if (control->GetID() != "COLLECTIONBOX") { m_ControlsInCollectionBoxList->AddItem(control->GetName()); }
			// Check if this is selected in the editor, and if so, select it in the list too
			if (collectionBox == s_SelectionInfo.GetControl()) { m_ControlsInCollectionBoxList->SetSelectedIndex(-1); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::MouseInsideBox(int mousePosX, int mousePosY, int boxPosX, int boxPosY, int boxWidth, int boxHeight) const {
		return (mousePosX >= boxPosX && mousePosX <= boxPosX + boxWidth && mousePosY >= boxPosY && mousePosY <= boxPosY + boxHeight) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * EditorManager::ControlUnderMouse(GUIControl *control, int mousePosX, int mousePosY) {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);
		if (!MouseInsideBox(mousePosX, mousePosY, controlPosX, controlPosY, controlWidth, controlHeight)) {
			return nullptr;
		}

		// Check children. Check in reverse because top most visible control is last in the list.
		for (std::vector<GUIControl *>::reverse_iterator childListEntry = control->GetChildren()->rbegin(); childListEntry != control->GetChildren()->rend(); childListEntry++) {
			GUIControl *childControl = ControlUnderMouse(*childListEntry, mousePosX, mousePosY);
			if (childControl) {
				return childControl;
			}
		}
		return control;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int EditorManager::HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);

		int regionSize = 6;
		int handle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(mousePosX, mousePosY, controlPosX - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (i != 1 && MouseInsideBox(mousePosX, mousePosY, controlPosX + controlWidth / 2 - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (MouseInsideBox(mousePosX, mousePosY, controlPosX + controlWidth - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;
		}
		// Not over any handle
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::ClearCurrentSelection() const {
		s_SelectionInfo.ClearSelection();
		m_PropertyPage->ClearValues();
		m_CollectionBoxList->SetSelectedIndex(0);
		m_ControlsInCollectionBoxList->ClearList();

		// Clear focused control of the manager itself so it doesn't persist between selection changes (e.g property page line remains selected after clearing or changing selection)
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateSnapGridSize(GUIEvent &editorEvent) const {
		std::string newValue = dynamic_cast<GUITextBox *>(editorEvent.GetControl())->GetText();
		if (newValue.empty()) { newValue = "1"; }

		bool validEntry = true;
		for (const char &stringChar : newValue) {
			if (!std::isdigit(stringChar)) {
				validEntry = false;
				break;
			}
		}
		EditorSelection::s_SnapGridSize = validEntry ? std::clamp(std::stoi(newValue), 0, 255) : EditorSelection::s_SnapGridSize;
		dynamic_cast<GUITextBox *>(editorEvent.GetControl())->SetText(std::to_string(EditorSelection::s_SnapGridSize));

		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::UpdateControlProperties(GUIControl *control, bool manualEdit) const {
		bool result = false;
		if (control) {
			if (manualEdit) {
				control->ApplyProperties(m_PropertyPage->GetPropertyValues());
				result = true;
			} else {
				//control->StoreProperties();
				GUIProperties properties;
				properties.Update(control->GetProperties(), true);
				control->GetPanel()->BuildProperties(&properties);
				m_PropertyPage->SetPropertyValues(&properties);
				result = true;
			}
		}
		RemoveFocus();
		return result;
	}
}