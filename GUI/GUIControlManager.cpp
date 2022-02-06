#include "GUI.h"
#include "GUIReader.h"
#include "GUIWriter.h"
#include "GUIControlFactory.h"
#include "Timer.h"

namespace RTE {

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
		m_DoubleClickButtons = GUIControl::MOUSE_NONE;

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
				GUIProperties *p = new GUIProperties(std::string_view(line).substr(1, line.size() - 2));
				CurProp = p;
				ControlList.push_back(p);
				continue;
			}
			// Is the line a valid property?
			size_t Position = line.find_first_of('=');
			if (Position != std::string::npos) {
				// Break the line into variable & value, but only add a property if controlItr belongs to a section
				if (CurProp) { CurProp->AddProperty(reader.TrimString(line.substr(0, Position)), reader.TrimString(line.substr(Position + 1, std::string::npos))); }
				continue;
			}
		}
		for (GUIProperties *property : ControlList) {
			AddControl(property);
			delete(property);
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

		for (GUIControl *control : m_ControlList) {
			control->Save(writer);
			writer->NewLine();
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::GetEvent(GUIEvent *eventPtr) {
		if (eventPtr && !m_EventQueue.empty()) {
			*eventPtr = *m_EventQueue.back();
			delete m_EventQueue.back();
			m_EventQueue.pop_back();
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::SetFocus(GUIControl *newFocusedControl) {
		// Send the LoseFocus event to the old control (if there is one), then send the GainFocus event to the new control.
		if (m_FocusedControl) { m_FocusedControl->OnLoseFocus(); }
		m_FocusedControl = newFocusedControl;
		if (m_FocusedControl) { m_FocusedControl->OnGainFocus(); }
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

	GUIControl * GUIControlManager::FindTopControl(int X, int Y) {
		for (std::vector<GUIControl *>::reverse_iterator controlItr = m_ControlList.rbegin(); controlItr != m_ControlList.rend(); controlItr++) {
			if (GUIControl *control = *controlItr) {
				if (GUIControl *topControl = control->TopPanelUnderPoint(X, Y)) {
					return topControl;
				}
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::FindBottomControl(int X, int Y) {
		for (GUIControl *control : m_ControlList) {
			if (GUIControl *bottomPanel = control->BottomPanelUnderPoint(X, Y)) {
				return bottomPanel;
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::AddEvent(GUIEvent *Event) {
		if (Event) { m_EventQueue.emplace_back(Event); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::GetControl(const std::string_view &Name) {
		for (GUIControl *control : m_ControlList) {
			if (control->GetName() == Name) {
				return control;
			}
		}
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
		GUIControl *newControl = GUIControlFactory::CreateControl(this, Type);
		if (!newControl) {
			return nullptr;
		}
		newControl->Create(Name, X, Y, Width, Height);
		newControl->ChangeSkin(m_Skin);

		if (Parent) {
			int Z = 0;

			// Get the last panel in the list
			if (!m_ControlList.empty()) {
				const GUIControl *p = m_ControlList.at(m_ControlList.size() - 1);
				Z = p->GetZPos() + 1;
			}
			// Setup the panel
			newControl->Setup(this, Z);

			Parent->AddChild(newControl);
		}
		// Add the control to the list
		m_ControlList.push_back(newControl);

		// Ready
		newControl->Activate();

		return newControl;
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
		GUIControl *newControl = GUIControlFactory::CreateControl(this, Type);
		if (!newControl) {
			return nullptr;
		}
		newControl->Create(Property);
		newControl->ChangeSkin(m_Skin);

		// Get the parent control
		std::string Parent;
		Property->GetPropertyValue("Parent", &Parent);

		GUIControl *Par = GetControl(Parent);
		if (Par && Parent.compare("None") != 0) {

			int Z = 0;

			// Get the last panel in the list
			if (!m_ControlList.empty()) {
				const GUIControl *p = m_ControlList.at(m_ControlList.size() - 1);
				Z = p->GetZPos() + 1;
			}
			// Setup the panel
			newControl->Setup(this, Z);

			Par->AddChild(newControl);
		}

		// Add the control to the list
		m_ControlList.push_back(newControl);

		// Ready
		newControl->Activate();

		return newControl;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::RemoveControl(const std::string &Name, bool RemoveFromParent) {
		// NOTE: We can't simply remove it because some controls need to remove extra panels and it's silly to add 'remove' to every control to remove their extra panels (ie. Combobox).
		// Signals and stuff are also linked in so we just remove the controls from the list and not from memory.
		for (std::vector<GUIControl *>::iterator controlItr = m_ControlList.begin(); controlItr != m_ControlList.end(); controlItr++) {
			GUIControl *control = *controlItr;
			if (control->GetName() == Name) {

				// Just remove controlItr from the list
				control->SetVisible(false);
				m_ControlList.erase(controlItr);

				// Remove all my children
				control->RemoveAllChildren();

				// Remove me from my parent
				if (control->GetParent() && RemoveFromParent) { control->GetParent()->RemoveChild(Name); }

				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ClearAllControls() {
		for (GUIControl *control : m_ControlList) {
			delete control;
		}
		m_ControlList.clear();

		for (GUIEvent *event : m_EventQueue) {
			delete event;
		}
		m_EventQueue.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::CaptureMouse(GUIControl *Panel) {
		GUIAssert(Panel, "");

		// Release any old capture
		ReleaseMouse();

		// Setup the new capture
		m_CaptruredControl = Panel;
		m_CaptruredControl->SetCaptureState(true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ReleaseMouse() {
		if (m_CaptruredControl) { m_CaptruredControl->SetCaptureState(false); }
		m_CaptruredControl = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::TrackMouseHover(GUIControl *Pan, bool Enabled, float Delay) {
		GUIAssert(Pan, "");
		m_HoverTrack = Enabled;
		m_HoveredControl = Pan;
		if (m_HoverTrack) { m_HoverTime = static_cast<float>(m_Timer->GetElapsedRealTimeMS()) + (Delay / 1000.0F); }
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ChangeSkin(const std::string &SkinDir, const std::string &SkinFilename) {
		m_Skin->Destroy();
		m_Skin->Load(SkinDir, SkinFilename);

		for (GUIControl *control : m_ControlList) {
			control->ChangeSkin(m_Skin);
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
		int Released = GUIControl::MouseButtons::MOUSE_NONE;
		int Pushed = GUIControl::MouseButtons::MOUSE_NONE;
		int Buttons = GUIControl::MouseButtons::MOUSE_NONE;
		int Repeated = GUIControl::MouseButtons::MOUSE_NONE;
		int Modifier = GUIInput::KeyModifiers::ModNone;
		int Mod = GUIControl::MouseModifiers::MODI_NONE;

		float CurTime = m_Timer->GetElapsedRealTimeMS();

		// Build the modifier state
		Modifier = m_Input->GetKeyModifier();
		if (Modifier & GUIInput::KeyModifiers::ModShift) { Mod |= GUIControl::MouseModifiers::MODI_SHIFT; }
		if (Modifier & GUIInput::KeyModifiers::ModCtrl) { Mod |= GUIControl::MouseModifiers::MODI_CTRL; }
		if (Modifier & GUIInput::KeyModifiers::ModAlt) { Mod |= GUIControl::MouseModifiers::MODI_ALT; }
		if (Modifier & GUIInput::KeyModifiers::ModCommand) { Mod |= GUIControl::MouseModifiers::MODI_COMMAND; }

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
			GUIControl *CurPanel = m_CaptruredControl;

			// Find the lowest panel in the tree that the mouse is over
			if (!CurPanel) { CurPanel = FindTopControl(MouseX, MouseY); }

			// Build the states
			for (i = 0; i < 3; i++) {
				if (MouseButtons[i] == GUIInput::InputEvents::Released) { Released |= 1 << i; }
				if (MouseButtons[i] == GUIInput::InputEvents::Pushed) { Pushed |= 1 << i; }
				if (MouseButtons[i] == GUIInput::InputEvents::Repeat) { Repeated |= 1 << i; }
				if (MouseStates[i] == GUIInput::InputStates::Down) { Buttons |= 1 << i; }
			}

			// Mouse Up
			if (Released != GUIControl::MouseButtons::MOUSE_NONE && CurPanel) { CurPanel->OnMouseUp(MouseX, MouseY, Released, Mod); }

			// Double click (on the mouse up)
			if (Released != GUIControl::MouseButtons::MOUSE_NONE && m_DoubleClickButtons != GUIControl::MouseButtons::MOUSE_NONE) {
				if (CurPanel) { CurPanel->OnDoubleClick(MouseX, MouseY, m_DoubleClickButtons, Mod); }
				m_LastMouseDown[0] = m_LastMouseDown[1] = m_LastMouseDown[2] = -99999.0f;
			}

			// Mouse Down
			if (Pushed != GUIControl::MouseButtons::MOUSE_NONE) {
				// Double click settings
				m_DoubleClickButtons = GUIControl::MouseButtons::MOUSE_NONE;

				// Check for a double click
				for (i = 0; i < 3; i++) {
					if (Pushed & (1 << i)) {
						if (CurTime - m_LastMouseDown[i] < (float)(m_DoubleClickTime) && MouseInRect(&m_DoubleClickRect, MouseX, MouseY)) {
							m_DoubleClickButtons |= (1 << i);
						} else {
							// Setup the first click
							m_DoubleClickButtons = GUIControl::MouseButtons::MOUSE_NONE;
							m_LastMouseDown[i] = CurTime;
						}
					}
				}

				// Setup the double click rectangle
				if (m_DoubleClickButtons == GUIControl::MouseButtons::MOUSE_NONE) { SetRect(&m_DoubleClickRect, MouseX - m_DoubleClickSize, MouseY - m_DoubleClickSize, MouseX + m_DoubleClickSize, MouseY + m_DoubleClickSize); }

				// OnMouseDown event
				if (CurPanel) { CurPanel->OnMouseDown(MouseX, MouseY, Pushed, Mod); }
			}

			// Mouse move
			if ((DeltaX != 0 || DeltaY != 0) && CurPanel) { CurPanel->OnMouseMove(MouseX, MouseY, Buttons, Mod); }

			// Mouse Hover
			if (m_HoverTrack && m_HoverTime < CurTime) {
				// Disable it (panel will have to re-enable it if it wants to continue)
				m_HoverTrack = false;

				if (m_HoveredControl && m_HoveredControl->PointInside(MouseX, MouseY)/*GetUniqueID() == topControl->GetUniqueID()*/) {
					// call the OnMouseHover event
					m_HoveredControl->OnMouseHover(MouseX, MouseY, Buttons, Mod);
				}
			}

			// Mouse enter & leave
			bool Enter = false;
			bool Leave = false;
			if (!m_MousedOverControl && CurPanel) { Enter = true; }
			if (!CurPanel && m_MousedOverControl) { Leave = true; }
			if (m_MousedOverControl && CurPanel && m_MousedOverControl->GetUniqueID() != CurPanel->GetUniqueID()) {
				Enter = true;
				Leave = true;
			}

			// OnMouseEnter
			if (Enter && CurPanel) { CurPanel->OnMouseEnter(MouseX, MouseY, Buttons, Mod); }

			// OnMouseLeave
			if (Leave &&m_MousedOverControl) { m_MousedOverControl->OnMouseLeave(MouseX, MouseY, Buttons, Mod); }

			if (MouseWheelChange &&CurPanel) { CurPanel->OnMouseWheelChange(MouseX, MouseY, Mod, MouseWheelChange); }

			m_MousedOverControl = CurPanel;
		}

		if (!ignoreKeyboardEvents) {
			// Keyboard Events
			uint8_t KeyboardBuffer[256];
			m_Input->GetKeyboardBuffer(KeyboardBuffer);

			// If we don't have a panel with focus, just ignore keyboard events
			if (!m_FocusedControl) {
				return;
			}
			// If the panel is not enabled, don't send controlItr key events
			if (!m_FocusedControl->GetEnabled()) {
				return;
			}
			for (i = 1; i < 256; i++) {
				switch (KeyboardBuffer[i]) {
					// KeyDown & KeyPress
					case GUIInput::InputEvents::Pushed:
						m_FocusedControl->OnKeyDown(i, Mod);
						m_FocusedControl->OnKeyPress(i, Mod);
						break;

						// KeyUp
					case GUIInput::InputEvents::Released:
						m_FocusedControl->OnKeyUp(i, Mod);
						break;

						// KeyPress
					case GUIInput::InputEvents::Repeat:
						m_FocusedControl->OnKeyPress(i, Mod);
						break;
					default:
						break;
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Draw(GUIScreen *Screen) {
		for (GUIControl *control : m_ControlList) {
			if (control->GetVisible()) { control->Draw(Screen); }
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