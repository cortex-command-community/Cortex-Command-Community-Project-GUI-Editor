#include "GUI.h"

#include "GUIButton.h"
#include "GUIListBox.h"
#include "GUICheckbox.h"
#include "GUIRadioButton.h"
#include "GUITab.h"
#include "GUICollectionBox.h"
#include "GUIProgressBar.h"
#include "GUISlider.h"
#include "GUITextBox.h"
#include "GUILabel.h"
#include "GUIScrollbar.h"
#include "GUIComboBox.h"
#include "GUIPropertyPage.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlFactory::CreateControl(GUIControlManager *owningManager, const std::string_view &controlType, const std::string &name, int posX, int posY, int width, int height) {
		if (controlType == "BUTTON") {
			GUIButton *newControl = new GUIButton(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "LISTBOX") {
			GUIListBox *newControl = new GUIListBox(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "CHECKBOX") {
			GUICheckbox *newControl = new GUICheckbox(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "RADIOBUTTON") {
			GUIRadioButton *newControl = new GUIRadioButton(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "TAB") {
			GUITab *newControl = new GUITab(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "COLLECTIONBOX") {
			GUICollectionBox *newControl = new GUICollectionBox(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "PROGRESSBAR") {
			GUIProgressBar *newControl = new GUIProgressBar(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "SLIDER") {
			GUISlider *newControl = new GUISlider(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "TEXTBOX") {
			GUITextBox *newControl = new GUITextBox(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "LABEL") {
			GUILabel *newControl = new GUILabel(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "SCROLLBAR") {
			GUIScrollbar *newControl = new GUIScrollbar(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "COMBOBOX") {
			GUIComboBox *newControl = new GUIComboBox(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "PROPERTYPAGE") {
			GUIPropertyPage *newControl = new GUIPropertyPage(owningManager);
			newControl->Create(name, posX, posY, width, height);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		return nullptr;
	}

	GUIControl * GUIControlFactory::CreateControl(GUIControlManager *owningManager, const std::string_view &controlType, GUIProperties *reference) {
		if (controlType == "BUTTON") {
			GUIButton *newControl = new GUIButton(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "LISTBOX") {
			GUIListBox *newControl = new GUIListBox(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "CHECKBOX") {
			GUICheckbox *newControl = new GUICheckbox(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "RADIOBUTTON") {
			GUIRadioButton *newControl = new GUIRadioButton(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "TAB") {
			GUITab *newControl = new GUITab(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "COLLECTIONBOX") {
			GUICollectionBox *newControl = new GUICollectionBox(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "PROGRESSBAR") {
			GUIProgressBar *newControl = new GUIProgressBar(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "SLIDER") {
			GUISlider *newControl = new GUISlider(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "TEXTBOX") {
			GUITextBox *newControl = new GUITextBox(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "LABEL") {
			GUILabel *newControl = new GUILabel(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "SCROLLBAR") {
			GUIScrollbar *newControl = new GUIScrollbar(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "COMBOBOX") {
			GUIComboBox *newControl = new GUIComboBox(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		if (controlType == "PROPERTYPAGE") {
			GUIPropertyPage *newControl = new GUIPropertyPage(owningManager);
			newControl->Create(reference);
			newControl->ChangeSkin(owningManager->GetSkin());
			return newControl;
		}
		return nullptr;
	}
}