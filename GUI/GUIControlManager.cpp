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

		m_Skin = std::make_unique<GUISkin>(guiScreen);
		if (!m_Skin.get()) {
			return false;
		}
		if (!m_Skin->Create(skinDir, skinFile)) {
			m_Skin.reset();
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
		ClearAllControls();

		delete m_Timer;
		m_Timer = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::LoadLayout(const std::string &fileName, bool keepOld) {
		GUIReader reader;
		if (!reader.Create(fileName)) {
			return false;
		}
		std::vector<std::shared_ptr<GUIProperties>> loadedControlProperties;
		loadedControlProperties.reserve(50);

		std::shared_ptr<GUIProperties> currentSection = nullptr;

		while (!reader.EndOfFile()) {
			std::string line = reader.ReadLine();
			if (!line.empty()) {
				if (line.front() == '[' && line.back() == ']') {
					currentSection = std::make_shared<GUIProperties>(std::string_view(line).substr(1, line.size() - 2));
					loadedControlProperties.emplace_back(currentSection);
				} else {
					size_t equalPos = line.find_first_of('=');
					if (currentSection && equalPos != std::string::npos) { currentSection->AddProperty(reader.TrimString(line.substr(0, equalPos)), reader.TrimString(line.substr(equalPos + 1, std::string::npos))); }
				}
			}
		}
		if (!keepOld) { ClearAllControls(); }
		for (const std::shared_ptr<GUIProperties> &controlProperties : loadedControlProperties) {
			AddControl(controlProperties.get());
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::SaveLayout(const std::string &fileName) const {
		GUIWriter writer;
		if (!writer.Create(fileName)) {
			return false;
		}
		for (GUIControl *control : m_AllControls) {
			control->Save(writer);
			writer.NewLine(false, 2);
		}
		writer.EndWrite();
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::SetFocusedControl(GUIControl *newFocusedControl) {
		// Send the LoseFocus event to the old control (if there is one), then send the GainFocus event to the new control.
		if (m_FocusedControl) { m_FocusedControl->OnLoseFocus(); }
		m_FocusedControl = newFocusedControl;
		if (m_FocusedControl) { m_FocusedControl->OnGainFocus(); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::GetControl(const std::string_view &name) const {
		for (GUIControl *control : m_AllControls) {
			if (control->GetName() == name) {
				return control;
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::FindTopControl(int pointX, int pointY) {
		for (std::deque<GUIControl *>::reverse_iterator controlItr = m_AllControls.rbegin(); controlItr != m_AllControls.rend(); controlItr++) {
			if (GUIControl *control = *controlItr) {
				if (GUIControl *topControl = control->TopPanelUnderPoint(pointX, pointY)) {
					return topControl;
				}
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::GetControlUnderPoint(int pointX, int pointY, GUIControl *parent, int depth) {
		// Default to the root object if no parent specified
		if (!parent) { parent = m_Containers.front(); }
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

		if (parent->IsContainer()) {
			// Check children
			std::deque<GUIControl *> *List = parent->GetChildren();

			GUIAssert(List, "");

			// Control the depth. If negative, it'll go forever
			if (depth != 0) {
				for (std::deque<GUIControl *>::reverse_iterator it = List->rbegin(); it != List->rend(); it++) {
					// Only check visible controls
					if ((*it)->GetVisible()) {
						GUIControl *C = GetControlUnderPoint(pointX, pointY, *it, depth - 1);
						if (C) {
							return C;
						}
					}
				}
			}
		}
		// If not asked to search for the root object, return the parent if point is on it
		return parent == m_Containers.front() ? nullptr : parent;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::AddControl(const std::string &name, const std::string &type, GUIControl *parent, int posX, int posY, int width, int height) {
		if (!GetControl(name)) {
			if (GUIControl *newControl = GUIControlFactory::CreateControl(this, type)) {
				newControl->Create(name, posX, posY, width, height);
				newControl->ChangeSkin(m_Skin.get());

				if (parent) { parent->AddChild(newControl); }

				if (newControl->IsContainer()) { m_Containers.emplace_back(newControl); }

				m_AllControls.emplace_back(newControl);

				return newControl;
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlManager::AddControl(GUIProperties *properties) {
		GUIAssert(properties, "");

		std::string type;
		properties->GetPropertyValue("ControlType", &type);
		std::string name;
		properties->GetPropertyValue("Name", &name);

		if (!GetControl(name)) {
			if (GUIControl *newControl = GUIControlFactory::CreateControl(this, type)) {
				newControl->Create(properties);
				newControl->ChangeSkin(m_Skin.get());

				std::string parent;
				properties->GetPropertyValue("Parent", &parent);

				if (parent != "None") {
					if (GUIControl *parentControl = GetControl(parent)) { parentControl->AddChild(newControl); }
				}
				if (newControl->IsContainer()) { m_Containers.emplace_back(newControl); }

				m_AllControls.emplace_back(newControl);

				return newControl;
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::RemoveControl(const std::string &Name, bool RemoveFromParent) {
		// NOTE: We can't simply remove it because some controls need to remove extra panels and it's silly to add 'remove' to every control to remove their extra panels (ie. Combobox).
		// Signals and stuff are also linked in so we just remove the controls from the list and not from memory.
		for (std::deque<GUIControl *>::iterator controlItr = m_Containers.begin(); controlItr != m_Containers.end(); controlItr++) {
			GUIControl *control = *controlItr;
			if (control->GetName() == Name) {

				// Just remove controlItr from the list
				control->SetVisible(false);
				m_Containers.erase(controlItr);

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
		for (GUIControl *control : m_Containers) {
			delete control;
		}
		m_Containers.clear();
		m_AllControls.clear();
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

	bool GUIControlManager::GetEvent(GUIEvent *eventPtr) {
		if (eventPtr && !m_EventQueue.empty()) {
			*eventPtr = m_EventQueue.front();
			m_EventQueue.pop_front();
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ChangeSkin(const std::string &SkinDir, const std::string &SkinFilename) const {
		m_Skin->Destroy();
		m_Skin->Create(SkinDir, SkinFilename);

		for (GUIControl *control : m_AllControls) {
			control->ChangeSkin(m_Skin.get());
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIControlManager::ChangeZPosition(int controlID, ZPosChangeType changeType) {
		int controlIndex = -1;
		std::deque<GUIControl *> *controlContainer = nullptr;

		for (int i = 0; i < m_Containers.size(); ++i) {
			if (m_Containers[i]->GetUniqueID() == controlID) {
				controlContainer = &m_Containers;
				controlIndex = i;
				break;
			}
		}
		if (controlIndex < 0) {
			for (GUIControl *container : m_Containers) {
				std::deque<GUIControl *> *containerChildren = container->GetChildren();
				for (int i = 0; i < containerChildren->size(); ++i) {
					if (containerChildren->at(i)->GetUniqueID() == controlID) {
						controlContainer = containerChildren;
						controlIndex = i;
						break;
					}
				}
			}
		}
		if (controlIndex >= 0) {
			GUIControl *control = controlContainer->at(controlIndex);

			switch (changeType) {
				case ZPosChangeType::MoveToTop:
				case ZPosChangeType::MoveUp:
					if (controlIndex < controlContainer->size() - 1) {
						if (changeType == ZPosChangeType::MoveToTop) {
							controlContainer->erase(controlContainer->begin() + controlIndex);
							controlContainer->emplace_back(control);
						} else {
							std::swap(controlContainer->at(controlIndex), controlContainer->at(controlIndex + 1));
						}
						return true;
					}
					break;
				case ZPosChangeType::MoveToBottom:
				case ZPosChangeType::MoveDown:
					if (controlIndex > 0) {
						if (changeType == ZPosChangeType::MoveToBottom) {
							controlContainer->erase(controlContainer->begin() + controlIndex);
							controlContainer->emplace(controlContainer->begin(), control);
						} else {
							std::swap(controlContainer->at(controlIndex), controlContainer->at(controlIndex - 1));
						}
						return true;
					}
					break;
				default:
					break;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Update(bool ignoreKeyboardEvents) {
		m_EventQueue.clear();
		m_Input->Update();

		int keyModifier = m_Input->GetKeyModifier();

		if (m_MouseEnabled) { ProcessMouseInput(keyModifier); }
		if (!ignoreKeyboardEvents && m_FocusedControl && m_FocusedControl->GetEnabled()) { ProcessKeyboardInput(keyModifier); }

		m_Input->SetCurrentMousePosAsPrev();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ProcessMouseInput(int keyModifier) {
		int mousePosX = m_Input->GetMousePosX();
		int mousePosY = m_Input->GetMousePosY();

		// Panels that have captured the mouse get the events over anything else regardless where the mouse currently is
		GUIControl *currentControl = m_CaptruredControl;
		if (!currentControl) { currentControl = FindTopControl(mousePosX, mousePosY); }

		float currentTime = static_cast<float>(m_Timer->GetElapsedRealTimeMS());

		std::array<int, 3> mouseButtonEvents;
		std::array<int, 3> mouseButtonStates;
		m_Input->GetMouseButtons(mouseButtonEvents.data(), mouseButtonStates.data());

		int buttonReleased = GUIControl::MouseButtons::MOUSE_NONE;
		int buttonPushed = GUIControl::MouseButtons::MOUSE_NONE;
		int buttonDown = GUIControl::MouseButtons::MOUSE_NONE;
		int buttonRepeated = GUIControl::MouseButtons::MOUSE_NONE;
		for (int i = 0; i < 3; i++) {
			if (mouseButtonEvents[i] == GUIInput::InputEvents::Released) { buttonReleased |= 1 << i; }
			if (mouseButtonEvents[i] == GUIInput::InputEvents::Pushed) { buttonPushed |= 1 << i; }
			if (mouseButtonEvents[i] == GUIInput::InputEvents::Repeat) { buttonRepeated |= 1 << i; }
			if (mouseButtonStates[i] == GUIInput::InputStates::Down) { buttonDown |= 1 << i; }
		}

		// Mouse Up
		if (currentControl && buttonReleased != GUIControl::MouseButtons::MOUSE_NONE) { currentControl->OnMouseUp(mousePosX, mousePosY, buttonReleased, keyModifier); }

		// Double click (on the mouse up)
		if (buttonReleased != GUIControl::MouseButtons::MOUSE_NONE && m_DoubleClickButtons != GUIControl::MouseButtons::MOUSE_NONE) {
			if (currentControl) { currentControl->OnDoubleClick(mousePosX, mousePosY, m_DoubleClickButtons, keyModifier); }
			m_LastMouseDown[0] = m_LastMouseDown[1] = m_LastMouseDown[2] = -99999.0f;
		}

		// Mouse Down
		if (buttonPushed != GUIControl::MouseButtons::MOUSE_NONE) {
			// Double click settings
			m_DoubleClickButtons = GUIControl::MouseButtons::MOUSE_NONE;

			// Check for a double click
			for (int i = 0; i < 3; i++) {
				if (buttonPushed & (1 << i)) {
					if (currentTime - m_LastMouseDown[i] < static_cast<float>(m_DoubleClickTime) && m_Input->MouseIsInsideRect(&m_DoubleClickRect)) {
						m_DoubleClickButtons |= (1 << i);
					} else {
						// Setup the first click
						m_DoubleClickButtons = GUIControl::MouseButtons::MOUSE_NONE;
						m_LastMouseDown[i] = currentTime;
					}
				}
			}

			// Setup the double click rectangle
			if (m_DoubleClickButtons == GUIControl::MouseButtons::MOUSE_NONE) { SetRect(&m_DoubleClickRect, mousePosX - m_DoubleClickSize, mousePosY - m_DoubleClickSize, mousePosX + m_DoubleClickSize, mousePosY + m_DoubleClickSize); }

			// OnMouseDown event
			if (currentControl) { currentControl->OnMouseDown(mousePosX, mousePosY, buttonPushed, keyModifier); }
		}

		// Mouse move
		if (currentControl && (m_Input->GetMouseMovementX() != 0 || m_Input->GetMouseMovementY() != 0)) { currentControl->OnMouseMove(mousePosX, mousePosY, buttonDown, keyModifier); }

		// Mouse Hover
		if (m_HoverTrack && m_HoverTime < currentTime) {
			// Disable it (panel will have to re-enable it if it wants to continue)
			m_HoverTrack = false;

			if (m_HoveredControl && m_HoveredControl->PointInside(mousePosX, mousePosY)/*GetUniqueID() == topControl->GetUniqueID()*/) {
				// call the OnMouseHover event
				m_HoveredControl->OnMouseHover(mousePosX, mousePosY, buttonDown, keyModifier);
			}
		}

		// Mouse enter & leave
		bool mouseEnter = false;
		bool mouseLeave = false;
		if (!m_MousedOverControl && currentControl) { mouseEnter = true; }
		if (!currentControl && m_MousedOverControl) { mouseLeave = true; }
		if (m_MousedOverControl && currentControl && m_MousedOverControl->GetUniqueID() != currentControl->GetUniqueID()) {
			mouseEnter = true;
			mouseLeave = true;
		}

		// OnMouseEnter
		if (mouseEnter && currentControl) { currentControl->OnMouseEnter(mousePosX, mousePosY, buttonDown, keyModifier); }

		// OnMouseLeave
		if (mouseLeave && m_MousedOverControl) { m_MousedOverControl->OnMouseLeave(mousePosX, mousePosY, buttonDown, keyModifier); }

		int mouseWheelChange = m_Input->GetMouseWheelChange();
		if (currentControl && mouseWheelChange != 0) {
			currentControl->OnMouseWheelChange(mousePosX, mousePosY, keyModifier, mouseWheelChange);
		}

		m_MousedOverControl = currentControl;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::ProcessKeyboardInput(int keyModifier) {
		std::array<uint8_t, c_KeyboardBufferSize> keyBuffer = m_Input->GetKeyboardBuffer();

		for (int key = 1; key < keyBuffer.size(); ++key) {
			switch (keyBuffer[key]) {
				case GUIInput::InputEvents::Pushed:
					m_FocusedControl->OnKeyDown(key, keyModifier);
					m_FocusedControl->OnKeyPress(key, keyModifier);
					break;
				case GUIInput::InputEvents::Released:
					m_FocusedControl->OnKeyUp(key, keyModifier);
					break;
				case GUIInput::InputEvents::Repeat:
					m_FocusedControl->OnKeyPress(key, keyModifier);
					break;
				default:
					break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::DrawMouse() const {
		m_Screen->DrawBitmapTrans(m_Skin->GetMouseCursorBitmap(m_CursorType), m_Input->GetMousePosX() - 1, m_Input->GetMousePosY() - 1, nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIControlManager::Draw(GUIScreen *Screen) const {
		for (GUIControl *container : m_Containers) {
			container->Draw(Screen);
		}
	}
}