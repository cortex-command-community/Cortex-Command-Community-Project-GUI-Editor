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

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIControl * GUIControlFactory::CreateControl(GUIManager *Manager, GUIControlManager *ControlManager, const std::string &ControlName) {
	// Button
	if (ControlName == "BUTTON") {
		return new GUIButton(Manager, ControlManager);
	}
	// Listbox
	if (ControlName == "LISTBOX") {
		return new GUIListBox(Manager, ControlManager);
	}
	// Checkbox
	if (ControlName == "CHECKBOX") {
		return new GUICheckbox(Manager, ControlManager);
	}
	// RadioButton
	if (ControlName == "RADIOBUTTON") {
		return new GUIRadioButton(Manager, ControlManager);
	}
	// Tab
	if (ControlName == "TAB") {
		return new GUITab(Manager, ControlManager);
	}
	// CollectionBox
	if (ControlName == "COLLECTIONBOX") {
		return new GUICollectionBox(Manager, ControlManager);
	}
	// ProgressBar
	if (ControlName == "PROGRESSBAR") {
		return new GUIProgressBar(Manager, ControlManager);
	}
	// Slider
	if (ControlName == "SLIDER") {
		return new GUISlider(Manager, ControlManager);
	}
	// TextBox
	if (ControlName == "TEXTBOX") {
		return new GUITextBox(Manager, ControlManager);
	}
	// Label
	if (ControlName == "LABEL") {
		return new GUILabel(Manager, ControlManager);
	}
	// Scrollbar
	if (ControlName == "SCROLLBAR") {
		return new GUIScrollbar(Manager, ControlManager);
	}
	// ComboBox
	if (ControlName == "COMBOBOX") {
		return new GUIComboBox(Manager, ControlManager);
	}
	// PropertyPage
	if (ControlName == "PROPERTYPAGE") {
		return new GUIPropertyPage(Manager, ControlManager);
	}
	// Not Found
	return 0;
}