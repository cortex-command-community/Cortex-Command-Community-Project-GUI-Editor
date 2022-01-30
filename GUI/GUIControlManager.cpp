#include "GUI.h"
#include "GUIReader.h"
#include "GUIWriter.h"
#include "Timer.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Clear() {
		m_Screen = nullptr;
		m_Input = nullptr;
		m_Skin = nullptr;
		m_ControlList.clear();
		m_EventQueue.clear();

		m_CursorType = CursorType::Pointer;

		m_PanelList.clear();
		m_CapturedPanel = nullptr;
		m_MouseOverPanel = nullptr;
		m_FocusPanel = nullptr;
		m_MouseEnabled = true;
		m_OldMouseX = m_OldMouseY = 0;
		m_UniqueIDCount = 0;

		m_HoverTrack = false;
		m_HoverPanel = nullptr;

		// Double click times
		m_LastMouseDown[0] = -99999.0F;
		m_LastMouseDown[1] = -99999.0F;
		m_LastMouseDown[2] = -99999.0F;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::Create(GUIScreen *guiScreen, GUIInput *guiInput, const std::string &skinDir, const std::string &skinFile) {
		GUIAssert(guiScreen && guiInput, "");

		m_Screen = guiScreen;
		m_Input = guiInput;

		m_Skin = new GUISkin(guiScreen);
		if (!m_Skin) {
			return false;
		}
		if (!m_Skin->Load(skinDir, skinFile)) {
			delete m_Skin;
			m_Skin = nullptr;
			return false;
		}

		// Maximum time allowed between two clicks for a double click
		// In milliseconds
		//m_DoubleClickTime = GetDoubleClickTime(); // Use windows' system value
		m_DoubleClickTime = 500;
		//m_DoubleClickSize = GetSystemMetrics(SM_CXDOUBLECLK)/2; // Use windows' system value
		m_DoubleClickSize = 2;
		m_DoubleClickButtons = GUIPanel::MOUSE_NONE;

		m_Timer = new Timer();

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Destroy() {
		// Free the skin
		if (m_Skin) {
			m_Skin->Destroy();
			delete m_Skin;
			m_Skin = nullptr;
		}

		// Destroy the controls & event queue
		ClearAllControls();

		delete m_Timer;
		m_Timer = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::Load(const std::string &Filename, bool keepOld) {
		GUIReader reader;
		if (reader.Create(Filename.c_str()) != 0) {
			return false;
		}
		// Clear the current layout, IF directed to
		if (!keepOld) { ClearAllControls(); }

		std::vector<GUIProperties *> ControlList;
		ControlList.clear();

		GUIProperties *CurProp = nullptr;

		while (!reader.GetStream()->eof()) {
			std::string line = reader.ReadLine();
			if (line.empty()) {
				continue;
			}
			// Is the line a section?
			if (line.front() == '[' && line.back() == ']') {
				GUIProperties *p = new GUIProperties(line.substr(1, line.size() - 2));
				CurProp = p;
				ControlList.push_back(p);
				continue;
			}
			// Is the line a valid property?
			size_t Position = line.find_first_of('=');
			if (Position != std::string::npos) {
				// Break the line into variable & value, but only add a property if it belongs to a section
				if (CurProp) {
					// Grab the variable & value strings and trim them
					std::string Name = reader.TrimString(line.substr(0, Position));
					std::string Value = reader.TrimString(line.substr(Position + 1, std::string::npos));

					// Add it to the current property
					CurProp->AddProperty(Name, Value);
				}
				continue;
			}
		}
		// Go through each control item and create it
		for (std::vector<GUIProperties *>::iterator it = ControlList.begin(); it != ControlList.end(); it++) {
			GUIProperties *Prop = *it;
			AddControl(Prop);
			// Free the property class
			delete Prop;
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::Save(const std::string &fileName) {
		GUIWriter writer;
		if (writer.Create(fileName) != 0) {
			return false;
		}
		bool Result = Save(&writer);
		writer.EndWrite();
		return Result;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::Save(GUIWriter *writer) {
		GUIAssert(writer, "");

		for (std::vector<GUIControl *>::iterator it = m_ControlList.begin(); it != m_ControlList.end(); it++) {
			GUIControl *C = *it;
			C->Save(writer);
			// Separate controls by one line
			writer->NewLine();
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::GetEvent(GUIEvent *Event) {
		if (Event && !m_EventQueue.empty()) {
			*Event = *m_EventQueue.back();
			if (GUIEvent *ptr = m_EventQueue.at(m_EventQueue.size() - 1)) { delete ptr; }
			m_EventQueue.pop_back();
			return true;
		}
		// Empty queue OR null Event pointer
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::SetFocus(GUIPanel *Pan) {
		// Send the LoseFocus event to the old panel (if there is one)
		if (m_FocusPanel) { m_FocusPanel->OnLoseFocus(); }

		m_FocusPanel = Pan;

		// Send the GainFocus event to the new panel
		if (m_FocusPanel) { m_FocusPanel->OnGainFocus(); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::MouseInRect(const GUIRect *Rect, int X, int Y) const {
		if (!Rect) {
			return false;
		}
		if (X >= Rect->left && X <= Rect->right && Y >= Rect->top && Y <= Rect->bottom) {
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIPanel * GUIControlManager::FindTopPanel(int X, int Y) {
		std::vector<GUIPanel *>::reverse_iterator it;

		for (it = m_PanelList.rbegin(); it != m_PanelList.rend(); it++) {
			if (GUIPanel *P = *it) {
				if (GUIPanel *CurPanel = P->TopPanelUnderPoint(X, Y)) {
					return CurPanel;
				}
			}
		}
		// No panel found
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIPanel * GUIControlManager::FindBottomPanel(int X, int Y) {
		for (std::vector<GUIPanel *>::iterator it = m_PanelList.begin(); it != m_PanelList.end(); it++) {
			if (GUIPanel *P = *it) {
				if (GUIPanel *CurPanel = P->BottomPanelUnderPoint(X, Y)) {
					return CurPanel;
				}
			}
		}
		// No panel found
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::AddEvent(GUIEvent *Event) {
		// Add the event to the queue
		if (Event) { m_EventQueue.push_back(Event); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::GetControl(const std::string_view &Name) {
		for (std::vector<GUIControl *>::iterator it = m_ControlList.begin(); it != m_ControlList.end(); it++) {
			GUIControl *C = *it;
			if (C->GetName().compare(Name) == 0) {
				return C;
			}
		}
		// Not found
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::GetControlUnderPoint(int pointX, int pointY, GUIControl *parent, int depth) {
		// Default to the root object if no parent specified
		if (!parent) { parent = m_ControlList.front(); }
		if (!parent) {
			return nullptr;
		}
		// Clicked on the parent?
		int X;
		int Y;
		int Width;
		int Height;
		parent->GetControlRect(&X, &Y, &Width, &Height);

		if (pointX < X || pointX > X + Width) {
			return nullptr;
		}
		if (pointY < Y || pointY > Y + Height) {
			return nullptr;
		}

		// Check children
		std::vector<GUIControl *> *List = parent->GetChildren();

		GUIAssert(List, "");

		// Control the depth. If negative, it'll go forever
		if (depth != 0) {
			for (std::vector<GUIControl *>::reverse_iterator it = List->rbegin(); it != List->rend(); it++) {
				// Only check visible controls
				if ((*it)->GetVisible()) {
					GUIControl *C = GetControlUnderPoint(pointX, pointY, *it, depth - 1);
					if (C) {
						return C;
					}
				}
			}
		}
		// If not asked to search for the root object, return the parent if point is on it
		return parent == m_ControlList.front() ? nullptr : parent;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::AddControl(const std::string &Name, const std::string &Type, GUIControl *Parent, int X, int Y, int Width, int Height) {
		// Skip if we already have a control of this name
		if (GetControl(Name)) {
			return nullptr;
		}
		// Create the control
		GUIControl *Control = GUIControlFactory::CreateControl(this, Type);
		if (!Control) {
			return nullptr;
		}
		Control->Create(Name, X, Y, Width, Height);
		Control->ChangeSkin(m_Skin);

		GUIPanel *Pan = nullptr;
		if (Parent) {
			Pan = Parent->GetPanel();
			Parent->AddChild(Control);
		}
		if (Pan) {
			Pan->AddChild(Control->GetPanel());
		} else {
			AddPanel(Control->GetPanel());
		}
		// Add the control to the list
		m_ControlList.push_back(Control);

		// Ready
		Control->Activate();

		return Control;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::AddControl(GUIProperties *Property) {
		GUIAssert(Property, "");

		// Get the control type and name
		std::string Type;
		Property->GetPropertyValue("ControlType", &Type);
		std::string Name;
		Property->GetPropertyValue("Name", &Name);

		// Skip if we already have a control of this name
		if (GetControl(Name)) {
			return nullptr;
		}
		// Create the control
		GUIControl *Control = GUIControlFactory::CreateControl(this, Type);
		if (!Control) {
			return nullptr;
		}

		Control->Create(Property);
		Control->ChangeSkin(m_Skin);

		// Get the parent control
		std::string Parent;
		Property->GetPropertyValue("Parent", &Parent);

		GUIControl *Par = GetControl(Parent);
		GUIPanel *Pan = nullptr;
		if (Par && Parent.compare("None") != 0) {
			Pan = Par->GetPanel();
			Par->AddChild(Control);
		}

		if (Pan) {
			Pan->AddChild(Control->GetPanel());
		} else {
			AddPanel(Control->GetPanel());
		}

		// Add the control to the list
		m_ControlList.push_back(Control);

		// Ready
		Control->Activate();

		return Control;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::RemoveControl(const std::string &Name, bool RemoveFromParent) {
		// NOTE: We can't simply remove it because some controls need to remove extra panels and it's silly to add 'remove' to every control to remove their extra panels (ie. Combobox).
		// Signals and stuff are also linked in so we just remove the controls from the list and not from memory.
		for (std::vector<GUIControl *>::iterator it = m_ControlList.begin(); it != m_ControlList.end(); it++) {
			GUIControl *C = *it;
			if (C->GetName().compare(Name) == 0) {

				// Just remove it from the list
				C->SetVisible(false);
				m_ControlList.erase(it);

				// Remove all my children
				C->RemoveAllChildren();

				// Remove me from my parent
				if (C->GetParent() && RemoveFromParent) { C->GetParent()->RemoveChild(Name); }

				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ClearAllControls() {
		for (std::vector<GUIControl *>::iterator it = m_ControlList.begin(); it != m_ControlList.end(); it++) {
			GUIControl *C = *it;
			C->Destroy();
			delete C;
		}
		m_ControlList.clear();
		m_PanelList.clear();

		for (std::vector<GUIEvent *>::iterator ite = m_EventQueue.begin(); ite != m_EventQueue.end(); ite++) {
			GUIEvent *E = *ite;
			if (E) { delete E; }
		}
		m_EventQueue.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::AddPanel(GUIPanel *panel) {
		if (panel) {
			int Z = 0;

			// Get the last panel in the list
			if (!m_PanelList.empty()) {
				const GUIPanel *p = m_PanelList.at(m_PanelList.size() - 1);
				Z = p->GetZPos() + 1;
			}
			// Setup the panel
			panel->Setup(this, Z);

			// Add the panel to the list
			m_PanelList.push_back(panel);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::CaptureMouse(GUIPanel *Panel) {
		GUIAssert(Panel, "");

		// Release any old capture
		ReleaseMouse();

		// Setup the new capture
		m_CapturedPanel = Panel;
		m_CapturedPanel->SetCaptureState(true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ReleaseMouse() {
		if (m_CapturedPanel) { m_CapturedPanel->SetCaptureState(false); }
		m_CapturedPanel = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::TrackMouseHover(GUIPanel *Pan, bool Enabled, int Delay) {
		GUIAssert(Pan, "");
		m_HoverTrack = Enabled;
		m_HoverPanel = Pan;
		if (m_HoverTrack) { m_HoverTime = m_Timer->GetElapsedRealTimeMS() + ((float)Delay / 1000.0F); }
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ChangeSkin(const std::string &SkinDir, const std::string &SkinFilename) {
		m_Skin->Destroy();
		m_Skin->Load(SkinDir, SkinFilename);

		// Go through every control and change its skin
		for (std::vector<GUIControl *>::iterator it = m_ControlList.begin(); it != m_ControlList.end(); it++) {
			GUIControl *C = *it;
			C->ChangeSkin(m_Skin);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Update(bool ignoreKeyboardEvents) {
		m_EventQueue.clear();

		m_Input->Update();

		// Mouse Events
		int i;
		int MouseX = 0;
		int MouseY = 0;
		int DeltaX;
		int DeltaY;
		int MouseButtons[3];
		int MouseStates[3];
		int MouseWheelChange = 0;
		int Released = GUIPanel::MouseButtons::MOUSE_NONE;
		int Pushed = GUIPanel::MouseButtons::MOUSE_NONE;
		int Buttons = GUIPanel::MouseButtons::MOUSE_NONE;
		int Repeated = GUIPanel::MouseButtons::MOUSE_NONE;
		int Modifier = GUIInput::KeyModifiers::ModNone;
		int Mod = GUIPanel::MouseModifiers::MODI_NONE;

		float CurTime = m_Timer->GetElapsedRealTimeMS();

		// Build the modifier state
		Modifier = m_Input->GetKeyModifier();
		if (Modifier & GUIInput::KeyModifiers::ModShift) { Mod |= GUIPanel::MouseModifiers::MODI_SHIFT; }
		if (Modifier & GUIInput::KeyModifiers::ModCtrl) { Mod |= GUIPanel::MouseModifiers::MODI_CTRL; }
		if (Modifier & GUIInput::KeyModifiers::ModAlt) { Mod |= GUIPanel::MouseModifiers::MODI_ALT; }
		if (Modifier & GUIInput::KeyModifiers::ModCommand) { Mod |= GUIPanel::MouseModifiers::MODI_COMMAND; }

		// Get the mouse data
		if (m_MouseEnabled) {
			m_Input->GetMousePosition(&MouseX, &MouseY);
			m_Input->GetMouseButtons(MouseButtons, MouseStates);
			MouseWheelChange = m_Input->GetMouseWheelChange();

			// Calculate mouse movement
			DeltaX = MouseX - m_OldMouseX;
			DeltaY = MouseY - m_OldMouseY;
			m_OldMouseX = MouseX;
			m_OldMouseY = MouseY;

			// Panels that have captured the mouse get the events over anything else
			// Regardless where the mouse currently is
			GUIPanel *CurPanel = m_CapturedPanel;

			// Find the lowest panel in the tree that the mouse is over
			if (!CurPanel) { CurPanel = FindTopPanel(MouseX, MouseY); }

			// Build the states
			for (i = 0; i < 3; i++) {
				if (MouseButtons[i] == GUIInput::InputEvents::Released) { Released |= 1 << i; }
				if (MouseButtons[i] == GUIInput::InputEvents::Pushed) { Pushed |= 1 << i; }
				if (MouseButtons[i] == GUIInput::InputEvents::Repeat) { Repeated |= 1 << i; }
				if (MouseStates[i] == GUIInput::InputStates::Down) { Buttons |= 1 << i; }
			}

			// Mouse Up
			if (Released != GUIPanel::MouseButtons::MOUSE_NONE && CurPanel) { CurPanel->OnMouseUp(MouseX, MouseY, Released, Mod); }

			// Double click (on the mouse up)
			if (Released != GUIPanel::MouseButtons::MOUSE_NONE && m_DoubleClickButtons != GUIPanel::MouseButtons::MOUSE_NONE) {
				if (CurPanel) { CurPanel->OnDoubleClick(MouseX, MouseY, m_DoubleClickButtons, Mod); }
				m_LastMouseDown[0] = m_LastMouseDown[1] = m_LastMouseDown[2] = -99999.0f;
			}

			// Mouse Down
			if (Pushed != GUIPanel::MouseButtons::MOUSE_NONE) {
				// Double click settings
				m_DoubleClickButtons = GUIPanel::MouseButtons::MOUSE_NONE;

				// Check for a double click
				for (i = 0; i < 3; i++) {
					if (Pushed & (1 << i)) {
						if (CurTime - m_LastMouseDown[i] < (float)(m_DoubleClickTime) && MouseInRect(&m_DoubleClickRect, MouseX, MouseY)) {
							m_DoubleClickButtons |= (1 << i);
						} else {
							// Setup the first click
							m_DoubleClickButtons = GUIPanel::MouseButtons::MOUSE_NONE;
							m_LastMouseDown[i] = CurTime;
						}
					}
				}

				// Setup the double click rectangle
				if (m_DoubleClickButtons == GUIPanel::MouseButtons::MOUSE_NONE) { SetRect(&m_DoubleClickRect, MouseX - m_DoubleClickSize, MouseY - m_DoubleClickSize, MouseX + m_DoubleClickSize, MouseY + m_DoubleClickSize); }

				// OnMouseDown event
				if (CurPanel) { CurPanel->OnMouseDown(MouseX, MouseY, Pushed, Mod); }
			}

			// Mouse move
			if ((DeltaX != 0 || DeltaY != 0) && CurPanel) { CurPanel->OnMouseMove(MouseX, MouseY, Buttons, Mod); }

			// Mouse Hover
			if (m_HoverTrack && m_HoverTime < CurTime) {
				// Disable it (panel will have to re-enable it if it wants to continue)
				m_HoverTrack = false;

				if (m_HoverPanel && m_HoverPanel->PointInside(MouseX, MouseY)/*GetPanelID() == CurPanel->GetPanelID()*/) {
					// call the OnMouseHover event
					m_HoverPanel->OnMouseHover(MouseX, MouseY, Buttons, Mod);
				}
			}

			// Mouse enter & leave
			bool Enter = false;
			bool Leave = false;
			if (!m_MouseOverPanel && CurPanel) { Enter = true; }
			if (!CurPanel && m_MouseOverPanel) { Leave = true; }
			if (m_MouseOverPanel && CurPanel && m_MouseOverPanel->GetPanelID() != CurPanel->GetPanelID()) {
				Enter = true;
				Leave = true;
			}

			// OnMouseEnter
			if (Enter && CurPanel) { CurPanel->OnMouseEnter(MouseX, MouseY, Buttons, Mod); }

			// OnMouseLeave
			if (Leave &&m_MouseOverPanel) { m_MouseOverPanel->OnMouseLeave(MouseX, MouseY, Buttons, Mod); }

			if (MouseWheelChange &&CurPanel) { CurPanel->OnMouseWheelChange(MouseX, MouseY, Mod, MouseWheelChange); }

			m_MouseOverPanel = CurPanel;
		}

		if (!ignoreKeyboardEvents) {
			// Keyboard Events
			uint8_t KeyboardBuffer[256];
			m_Input->GetKeyboardBuffer(KeyboardBuffer);

			// If we don't have a panel with focus, just ignore keyboard events
			if (!m_FocusPanel) {
				return;
			}
			// If the panel is not enabled, don't send it key events
			if (!m_FocusPanel->_GetEnabled()) {
				return;
			}
			for (i = 1; i < 256; i++) {
				switch (KeyboardBuffer[i]) {
					// KeyDown & KeyPress
					case GUIInput::InputEvents::Pushed:
						m_FocusPanel->OnKeyDown(i, Mod);
						m_FocusPanel->OnKeyPress(i, Mod);
						break;

						// KeyUp
					case GUIInput::InputEvents::Released:
						m_FocusPanel->OnKeyUp(i, Mod);
						break;

						// KeyPress
					case GUIInput::InputEvents::Repeat:
						m_FocusPanel->OnKeyPress(i, Mod);
						break;
					default:
						break;
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Draw(GUIScreen *Screen) {
		// Go through drawing panels that are invalid
		std::vector<GUIPanel *>::iterator it;

		for (it = m_PanelList.begin(); it != m_PanelList.end(); it++) {
			GUIPanel *p = *it;

			// Draw the panel
			if (p->_GetVisible()) { p->Draw(Screen); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::DrawMouse() {
		int mousePosX;
		int mousePosY;
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		switch (m_CursorType) {
			case CursorType::Pointer:
				m_Skin->DrawMouse(0, mousePosX, mousePosY);
				break;
			case CursorType::Text:
				m_Skin->DrawMouse(1, mousePosX, mousePosY);
				break;
			case CursorType::HorSize:
				m_Skin->DrawMouse(2, mousePosX, mousePosY);
				break;
			default:
				break;
		}
	}
}