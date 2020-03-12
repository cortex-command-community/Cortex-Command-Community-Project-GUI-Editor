//////////////////////////////////////////////////////////////////////////////////////////
// File:            UInputMan.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the UInputMan class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "Reader.h"
#include "Writer.h"
#include "UInputMan.h"
#include "FrameMan.h"
#include "ConsoleMan.h"
#include "GUIInput.h"

extern volatile bool g_Quit;
extern bool g_ResetActivity;
extern bool g_InActivity;
extern int g_IntroState;

namespace RTE
{

	extern UInputMan g_InputMan;

const string UInputMan::m_ClassName = "UInputMan";
const string UInputMan::InputScheme::m_sClassName = "InputScheme";
const string UInputMan::InputScheme::InputMapping::m_sClassName = "InputMapping";
char *UInputMan::s_aLastKeys = new char[KEY_MAX];
char *UInputMan::s_aChangedKeys = new char[KEY_MAX];
GUIInput* UInputMan::s_InputClass = NULL; 
bool UInputMan::m_aMouseButtonState[MAX_MOUSE_BUTTONS];
bool UInputMan::m_aMousePrevButtonState[MAX_MOUSE_BUTTONS];
bool UInputMan::m_aMouseChangedButtonState[MAX_MOUSE_BUTTONS];


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this InputMapping, effectively
//                  resetting the members of this abstraction level only.

void UInputMan::InputScheme::InputMapping::Clear()
{
    m_KeyMap = 0;
    m_MouseButtonMap = -1;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a InputMapping to be identical to another, by deep copy.

int UInputMan::InputScheme::InputMapping::Create(const InputMapping &reference)
{
    m_KeyMap = reference.m_KeyMap;
    m_MouseButtonMap = reference.m_MouseButtonMap;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int UInputMan::InputScheme::InputMapping::ReadProperty(std::string propName, Reader &reader)
{
    if (propName == "KeyMap")
    {
        int key;
        reader >> key;
        if (key != 0)
            m_KeyMap = key;
    }
    else if (propName == "MouseButtonMap")
        reader >> m_MouseButtonMap;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this InputMapping with a Writer for
//                  later recreation with Create(Reader &reader);

int UInputMan::InputScheme::InputMapping::Save(Writer &writer) const
{
    //Serializable::Save(writer);

// Gotto write somehting
//    if (m_KeyMap != 0)
//    {
        writer.NewProperty("KeyMap");
        writer << m_KeyMap;
//    }
    if (m_MouseButtonMap >= 0)
    {
        writer.NewProperty("MouseButtonMap");
        writer << m_MouseButtonMap;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this InputScheme, effectively
//                  resetting the members of this abstraction level only.

void UInputMan::InputScheme::Clear()
{
    m_ActiveDevice = DEVICE_KEYB_ONLY;

    // Clear all mappings
    for (int mapping = 0; mapping < INPUT_COUNT; ++mapping)
    {
        m_aInputMapping[mapping].Clear();
    }

    // Set up the default mouse button bindings - do these here becuase there are no config steps for them yet
    m_aInputMapping[INPUT_FIRE].SetMouseButton(MOUSE_LEFT);
    m_aInputMapping[INPUT_PIEMENU].SetMouseButton(MOUSE_RIGHT);
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a InputScheme to be identical to another, by deep copy.

int UInputMan::InputScheme::Create(const InputScheme &reference)
{
    m_ActiveDevice = reference.m_ActiveDevice;

    for (int mapping = 0; mapping < INPUT_COUNT; ++mapping)
        m_aInputMapping[mapping].Create(reference.m_aInputMapping[mapping]);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int UInputMan::InputScheme::ReadProperty(std::string propName, Reader &reader)
{
    if (propName == "Device")
        reader >> m_ActiveDevice;
    else if (propName == "LeftUp")
        reader >> m_aInputMapping[INPUT_L_UP];
    else if (propName == "LeftDown")
        reader >> m_aInputMapping[INPUT_L_DOWN];
    else if (propName == "LeftLeft")
        reader >> m_aInputMapping[INPUT_L_LEFT];
    else if (propName == "LeftRight")
        reader >> m_aInputMapping[INPUT_L_RIGHT];
    else if (propName == "RightUp")
        reader >> m_aInputMapping[INPUT_R_UP];
    else if (propName == "RightDown")
        reader >> m_aInputMapping[INPUT_R_DOWN];
    else if (propName == "RightLeft")
        reader >> m_aInputMapping[INPUT_R_LEFT];
    else if (propName == "RightRight")
        reader >> m_aInputMapping[INPUT_R_RIGHT];
    else if (propName == "Fire")
        reader >> m_aInputMapping[INPUT_FIRE];
    else if (propName == "Aim")
        reader >> m_aInputMapping[INPUT_AIM];
    else if (propName == "AimUp")
        reader >> m_aInputMapping[INPUT_AIM_UP];
    else if (propName == "AimDown")
        reader >> m_aInputMapping[INPUT_AIM_DOWN];
    else if (propName == "AimLeft")
        reader >> m_aInputMapping[INPUT_AIM_LEFT];
    else if (propName == "AimRight")
        reader >> m_aInputMapping[INPUT_AIM_RIGHT];
    else if (propName == "PieMenu")
        reader >> m_aInputMapping[INPUT_PIEMENU];
    else if (propName == "Jump")
        reader >> m_aInputMapping[INPUT_JUMP];
    else if (propName == "Crouch")
        reader >> m_aInputMapping[INPUT_CROUCH];
    else if (propName == "Next")
        reader >> m_aInputMapping[INPUT_NEXT];
    else if (propName == "Prev")
        reader >> m_aInputMapping[INPUT_PREV];
    else if (propName == "Start")
        reader >> m_aInputMapping[INPUT_START];
    else if (propName == "Back")
        reader >> m_aInputMapping[INPUT_BACK];
	else if (propName == "WeaponChangeNext")
		reader >> m_aInputMapping[INPUT_WEAPON_CHANGE_NEXT];
	else if (propName == "WeaponChangePrev")
		reader >> m_aInputMapping[INPUT_WEAPON_CHANGE_PREV];
	else if (propName == "WeaponPickup")
		reader >> m_aInputMapping[INPUT_WEAPON_PICKUP];
	else if (propName == "WeaponDrop")
		reader >> m_aInputMapping[INPUT_WEAPON_DROP];
	else if (propName == "WeaponReload")
		reader >> m_aInputMapping[INPUT_WEAPON_RELOAD];

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this InputScheme with a Writer for
//                  later recreation with Create(Reader &reader);

int UInputMan::InputScheme::Save(Writer &writer) const
{
    //Serializable::Save(writer);

    writer.NewLine();
    writer << "// 0 = Keyboard only, 1 = Mouse + Keyboard, 2 = Joystick One, 3 = Joystick Two, , 4 = Joystick Three, 5 = Joystick Four";
    writer.NewProperty("Device");
    writer << m_ActiveDevice;
    if (m_SchemePreset > PRESET_NONE)
    {
        writer << "// 0 = No Preset, 1 = WASD, 2 = Cursor Keys, 3 = XBox 360 Controller";
        writer.NewProperty("Preset");
        writer << m_SchemePreset;
    }
    writer.NewProperty("LeftUp");
    writer << m_aInputMapping[INPUT_L_UP];
    writer.NewProperty("LeftDown");
    writer << m_aInputMapping[INPUT_L_DOWN];
    writer.NewProperty("LeftLeft");
    writer << m_aInputMapping[INPUT_L_LEFT];
    writer.NewProperty("LeftRight");
    writer << m_aInputMapping[INPUT_L_RIGHT];
    writer.NewProperty("RightUp");
    writer << m_aInputMapping[INPUT_R_UP];
    writer.NewProperty("RightDown");
    writer << m_aInputMapping[INPUT_R_DOWN];
    writer.NewProperty("RightLeft");
    writer << m_aInputMapping[INPUT_R_LEFT];
    writer.NewProperty("RightRight");
    writer << m_aInputMapping[INPUT_R_RIGHT];
    writer.NewProperty("Fire");
    writer << m_aInputMapping[INPUT_FIRE];
    writer.NewProperty("Aim");
    writer << m_aInputMapping[INPUT_AIM];
    writer.NewProperty("AimUp");
    writer << m_aInputMapping[INPUT_AIM_UP];
    writer.NewProperty("AimDown");
    writer << m_aInputMapping[INPUT_AIM_DOWN];
    writer.NewProperty("AimLeft");
    writer << m_aInputMapping[INPUT_AIM_LEFT];
    writer.NewProperty("AimRight");
    writer << m_aInputMapping[INPUT_AIM_RIGHT];
    writer.NewProperty("PieMenu");
    writer << m_aInputMapping[INPUT_PIEMENU];
    writer.NewProperty("Jump");
    writer << m_aInputMapping[INPUT_JUMP];
    writer.NewProperty("Crouch");
    writer << m_aInputMapping[INPUT_CROUCH];
    writer.NewProperty("Next");
    writer << m_aInputMapping[INPUT_NEXT];
    writer.NewProperty("Prev");
    writer << m_aInputMapping[INPUT_PREV];
    writer.NewProperty("Start");
    writer << m_aInputMapping[INPUT_START];
    writer.NewProperty("Back");
    writer << m_aInputMapping[INPUT_BACK];
	writer.NewProperty("WeaponChangeNext");
	writer << m_aInputMapping[INPUT_WEAPON_CHANGE_NEXT];
	writer.NewProperty("WeaponChangePrev");
	writer << m_aInputMapping[INPUT_WEAPON_CHANGE_PREV];
	writer.NewProperty("WeaponPickup");
	writer << m_aInputMapping[INPUT_WEAPON_PICKUP];
	writer.NewProperty("WeaponDrop");
	writer << m_aInputMapping[INPUT_WEAPON_DROP];
	writer.NewProperty("WeaponReload");
	writer << m_aInputMapping[INPUT_WEAPON_RELOAD];
	writer.NewProperty("JoystickDeadzone");
	writer << m_JoystickDeadzone;
	writer.NewProperty("JoystickDeadzoneType");
	writer << m_JoystickDeadzoneType;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this UInputMan, effectively
//                  resetting the members of this abstraction level only.

void UInputMan::Clear()
{
    m_DebugArmed = false;

    // Init the previous keys so they don't make it seem like things have changed
    memcpy(s_aLastKeys, const_cast<const char *>(key), KEY_MAX);

    // Neutralize the changed keys so that no Releases will be detected initially
    for (int i = 0; i < KEY_MAX; ++i)
    {
        s_aChangedKeys[i] = false;
    }

    m_DisableKeyboard = false;

    // Reset mouse button data
    m_aMouseButtonState[MOUSE_LEFT] = m_aMousePrevButtonState[MOUSE_LEFT] = m_aMouseChangedButtonState[MOUSE_LEFT] = false;
    m_aMouseButtonState[MOUSE_RIGHT] = m_aMousePrevButtonState[MOUSE_RIGHT] = m_aMouseChangedButtonState[MOUSE_RIGHT] = false;
    m_aMouseButtonState[MOUSE_MIDDLE] = m_aMousePrevButtonState[MOUSE_MIDDLE] = m_aMouseChangedButtonState[MOUSE_MIDDLE] = false;
	
    // Set Mouse analog stick emualtion data defaults
    m_RawMouseMovement.Reset();
    m_AnalogMouseData.Reset();
    m_TrapMousePos = false;
    m_MouseTrapRadius = 350;

    // Mouse wheel init
    position_mouse_z(0);
    m_MouseWheelChange = 0;

    m_DisableMouseMoving = false;
    m_PrepareToEnableMouseMoving = false;

    // Reset all schemes
    int player;
    for (player = 0; player < MAX_PLAYERS; ++player)
    {
		//m_TrapMousePosPerPlayer[player] = false;

        m_aControlScheme[player].Clear();

    for (int device = 0; device < DEVICE_COUNT; ++device)
    {
        m_apDeviceIcons[device] = 0;
    }

	m_MouseSensitivity = 0.6;

    // Set up the default Mouse+keyboard key mappings for player one
    InputScheme::InputMapping * pMappings = m_aControlScheme[PLAYER_ONE].GetInputMappings();
    m_aControlScheme[PLAYER_ONE].SetDevice(DEVICE_MOUSE_KEYB);
    pMappings[INPUT_L_UP].SetKey(KEY_W);
    pMappings[INPUT_L_DOWN].SetKey(KEY_S);
    pMappings[INPUT_L_LEFT].SetKey(KEY_A);
    pMappings[INPUT_L_RIGHT].SetKey(KEY_D);
    pMappings[INPUT_R_UP].SetKey(KEY_W);
    pMappings[INPUT_R_DOWN].SetKey(KEY_S);
    pMappings[INPUT_R_LEFT].SetKey(KEY_A);
    pMappings[INPUT_R_RIGHT].SetKey(KEY_D);
//    pMappings[INPUT_FIRE].SetKey(KEY_H);
    pMappings[INPUT_FIRE].SetMouseButton(0);
//    pMappings[INPUT_AIM].SetKey(KEY_J);
//    pMappings[INPUT_AIM_UP].SetKey(KEY_W);
//    pMappings[INPUT_AIM_DOWN].SetKey(KEY_S);
//    pMappings[INPUT_PIEMENU].SetKey(KEY_K);
    pMappings[INPUT_PIEMENU].SetMouseButton(1);
    pMappings[INPUT_JUMP].SetKey(KEY_W);
    pMappings[INPUT_CROUCH].SetKey(KEY_S);
    pMappings[INPUT_NEXT].SetKey(KEY_E);
    pMappings[INPUT_PREV].SetKey(KEY_Q);

	pMappings[INPUT_WEAPON_RELOAD].SetKey(KEY_R);
	pMappings[INPUT_WEAPON_DROP].SetKey(KEY_G);
	pMappings[INPUT_WEAPON_PICKUP].SetKey(KEY_F);
	pMappings[INPUT_WEAPON_CHANGE_NEXT].SetKey(KEY_C);


    // Set up the default keyboard key bindings for player two
    pMappings = m_aControlScheme[PLAYER_TWO].GetInputMappings();
    pMappings[INPUT_L_UP].SetKey(84);
    pMappings[INPUT_L_DOWN].SetKey(85);
    pMappings[INPUT_L_LEFT].SetKey(82);
    pMappings[INPUT_L_RIGHT].SetKey(83);
//    pMappings[INPUT_R_UP].SetKey(KEY_UP);
//    pMappings[INPUT_R_DOWN].SetKey(KEY_DOWN);
//    pMappings[INPUT_R_LEFT].SetKey(KEY_LEFT);
//    pMappings[INPUT_R_RIGHT].SetKey(KEY_RIGHT);
    pMappings[INPUT_FIRE].SetKey(38);
    pMappings[INPUT_AIM].SetKey(39);
    pMappings[INPUT_AIM_UP].SetKey(84);
    pMappings[INPUT_AIM_DOWN].SetKey(85);
    pMappings[INPUT_PIEMENU].SetKey(40);
    pMappings[INPUT_JUMP].SetKey(91);
    pMappings[INPUT_CROUCH].SetKey(90);
    pMappings[INPUT_NEXT].SetKey(42);
    pMappings[INPUT_PREV].SetKey(41);

	pMappings[INPUT_WEAPON_RELOAD].SetKey(KEY_0_PAD);
	pMappings[INPUT_WEAPON_DROP].SetKey(KEY_6_PAD);
	pMappings[INPUT_WEAPON_PICKUP].SetKey(KEY_9_PAD);
	pMappings[INPUT_WEAPON_CHANGE_PREV].SetKey(KEY_7_PAD);
	pMappings[INPUT_WEAPON_CHANGE_NEXT].SetKey(KEY_8_PAD);


    // Player three. These key mappings are for when 
    pMappings = m_aControlScheme[PLAYER_THREE].GetInputMappings();
    m_aControlScheme[PLAYER_THREE].SetDevice(DEVICE_GAMEPAD_1);
    pMappings[INPUT_L_UP].SetKey(KEY_W);
    pMappings[INPUT_L_DOWN].SetKey(KEY_S);
    pMappings[INPUT_L_LEFT].SetKey(KEY_A);
    pMappings[INPUT_L_RIGHT].SetKey(KEY_D);
    pMappings[INPUT_R_UP].SetKey(KEY_W);
    pMappings[INPUT_R_DOWN].SetKey(KEY_S);
    pMappings[INPUT_R_LEFT].SetKey(KEY_A);
    pMappings[INPUT_R_RIGHT].SetKey(KEY_D);
    pMappings[INPUT_FIRE].SetKey(KEY_H);
    pMappings[INPUT_AIM].SetKey(KEY_J);
    pMappings[INPUT_AIM_UP].SetKey(KEY_W);
    pMappings[INPUT_AIM_DOWN].SetKey(KEY_S);
    pMappings[INPUT_PIEMENU].SetKey(KEY_K);
    pMappings[INPUT_JUMP].SetKey(KEY_L);
    pMappings[INPUT_CROUCH].SetKey(KEY_STOP);
    pMappings[INPUT_NEXT].SetKey(KEY_U);
    pMappings[INPUT_PREV].SetKey(KEY_Y);

    // Player four
    pMappings = m_aControlScheme[PLAYER_FOUR].GetInputMappings();
    m_aControlScheme[PLAYER_FOUR].SetDevice(DEVICE_GAMEPAD_2);
    pMappings[INPUT_L_UP].SetKey(KEY_UP);
    pMappings[INPUT_L_DOWN].SetKey(KEY_DOWN);
    pMappings[INPUT_L_LEFT].SetKey(KEY_LEFT);
    pMappings[INPUT_L_RIGHT].SetKey(KEY_RIGHT);
    pMappings[INPUT_R_UP].SetKey(KEY_UP);
    pMappings[INPUT_R_DOWN].SetKey(KEY_DOWN);
    pMappings[INPUT_R_LEFT].SetKey(KEY_LEFT);
    pMappings[INPUT_R_RIGHT].SetKey(KEY_RIGHT);
    pMappings[INPUT_FIRE].SetKey(KEY_1_PAD);
    pMappings[INPUT_AIM].SetKey(KEY_2_PAD);
    pMappings[INPUT_AIM_UP].SetKey(KEY_UP);
    pMappings[INPUT_AIM_DOWN].SetKey(KEY_DOWN);
    pMappings[INPUT_PIEMENU].SetKey(KEY_3_PAD);
    pMappings[INPUT_JUMP].SetKey(KEY_DEL_PAD);
    pMappings[INPUT_CROUCH].SetKey(KEY_STOP);
    pMappings[INPUT_NEXT].SetKey(KEY_5_PAD);
    pMappings[INPUT_PREV].SetKey(KEY_4_PAD);

    pMappings = 0;

	m_LastDeviceWhichControlledGUICursor = 0;
	//m_OverrideInput = false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the UInputMan object ready for use.

int UInputMan::Create()
{
    if (Serializable::Create() < 0)
        return -1;

    // This is a hack to fix the keyboard and mouse losing focus when in windowed mode.. it's a known allegro issue
//    if (!g_FrameMan.IsFullscreen())
//        rest(500);

    // Get the Allegro keyboard going
    install_keyboard();
    // Hack to not have the keyboard lose focus permanently when window is started without focus
    //win_grab_input();

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int UInputMan::ReadProperty(std::string propName, Reader &reader)
{
    int mappedButton = 0;


    if (propName == "P1Scheme")
        reader >> m_aControlScheme[PLAYER_ONE];
    else if (propName == "P2Scheme")
        reader >> m_aControlScheme[PLAYER_TWO];
    else if (propName == "P3Scheme")
        reader >> m_aControlScheme[PLAYER_THREE];
    else if (propName == "P4Scheme")
        reader >> m_aControlScheme[PLAYER_FOUR];
    else if (propName == "MouseSensitivity")
        reader >> m_MouseSensitivity;
	else
        // See if the base class(es) can find a match instead
        return Serializable::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this UInputMan with a Writer for
//                  later recreation with Create(Reader &reader);

int UInputMan::Save(Writer &writer) const
{
    writer.NewProperty("MouseSensitivity");
    writer << m_MouseSensitivity;
    writer.NewProperty("P1Scheme");
    writer << m_aControlScheme[PLAYER_ONE];
    writer.NewProperty("P2Scheme");
    writer << m_aControlScheme[PLAYER_TWO];
    writer.NewProperty("P3Scheme");
    writer << m_aControlScheme[PLAYER_THREE];
    writer.NewProperty("P4Scheme");
    writer << m_aControlScheme[PLAYER_FOUR];

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the UInputMan object.

void UInputMan::Destroy()
{
    Clear();
}
	

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReInitKeyboard
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Re-initalizes the keyboard for when windows regains focus. This is
//                  really used to work around an Allegro bug.

void UInputMan::ReInitKeyboard()
{
//    remove_keyboard();
    install_keyboard();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetInputClass
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the input class for use if one is available
void UInputMan::SetInputClass(GUIInput* pInputClass)
{
	s_InputClass = pInputClass;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSchemeIcon
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Get the current device Icon of a specific player's scheme

const Icon * UInputMan::GetSchemeIcon(int whichPlayer)
{
    if (whichPlayer < Activity::PLAYER_1 || whichPlayer >= Activity::MAXPLAYERCOUNT)
        return 0;

    // Lazy load of these.. can't load earlier
    if (!m_apDeviceIcons[DEVICE_KEYB_ONLY])
    {
        m_apDeviceIcons[DEVICE_KEYB_ONLY] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Keyboard"));
        m_apDeviceIcons[DEVICE_MOUSE_KEYB] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Mouse"));
        m_apDeviceIcons[DEVICE_GAMEPAD_1] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Gamepad 1"));
        m_apDeviceIcons[DEVICE_GAMEPAD_2] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Gamepad 2"));
        m_apDeviceIcons[DEVICE_GAMEPAD_3] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Gamepad 3"));
        m_apDeviceIcons[DEVICE_GAMEPAD_4] = dynamic_cast<const Icon *>(g_PresetMan.GetEntityPreset("Icon", "Device Gamepad 4"));
    }
    return m_apDeviceIcons[m_aControlScheme[whichPlayer].GetDevice()];
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetDeviceIcon
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Get the current Icon of a specific device's scheme

const Icon * UInputMan::GetDeviceIcon(int whichDevice)
{
	if (whichDevice < DEVICE_KEYB_ONLY || whichDevice > DEVICE_GAMEPAD_4)
		return 0;

	// Lazy load of these.. can't load earlier
	if (!m_apDeviceIcons[DEVICE_KEYB_ONLY])
		GetSchemeIcon(0);

	return m_apDeviceIcons[whichDevice];
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetJoystickCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the number of active joysticks

int UInputMan::GetJoystickCount() const
{
	if (num_joysticks > MAX_PLAYERS)
		return MAX_PLAYERS;
	else
		return num_joysticks;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMappingName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the name of the key/mouse/joy button/key/direction that a
//                  particular input element is mapped to.

string UInputMan::GetMappingName(int whichPlayer, int whichElement)
{
    if (whichPlayer < PLAYER_ONE || whichPlayer >= MAX_PLAYERS)
        return "";

    // Which Device is used by this player
    int device = m_aControlScheme[whichPlayer].GetDevice();

    // Which scheme preset, if any, this adheres to
    int preset = m_aControlScheme[whichPlayer].GetPreset();

    // Get handy pointer to the relevant input element
    InputScheme::InputMapping * pElement = &(m_aControlScheme[whichPlayer].GetInputMappings()[whichElement]);

    // If there is a preset, just return the element name set by the preset previously
    if (preset != PRESET_NONE && !pElement->GetPresetDesc().empty())
        return pElement->GetPresetDesc();

    // Joystick input is used, more important to show than keyboard
    if (device >= DEVICE_GAMEPAD_1)
    {
        // Translate the device selection to the joystick numbering (0-3)
        int whichJoy = device - DEVICE_GAMEPAD_1;

    // Mouse input is used, more important to show than keyboard
    if (device == DEVICE_MOUSE_KEYB && pElement->GetMouseButton() != MOUSE_NONE)
    {
        // Check mouse
        int button = pElement->GetMouseButton();
        if (button == MOUSE_LEFT)
            return "Left Mouse";
        else if (button == MOUSE_RIGHT)
            return "Right Mouse";
        else if (button == MOUSE_MIDDLE)
            return "Middle Mouse";
    }

    // Keyboard defualts - don't check certain elements which don't make sense when in mouse mode
    if (device == DEVICE_KEYB_ONLY || (device == DEVICE_MOUSE_KEYB && !(whichElement == INPUT_AIM_UP || whichElement == INPUT_AIM_DOWN)))
    {
        // Return keyboard mapping name
        if (pElement->GetKey() != 0)
            return scancode_to_name(pElement->GetKey());
    }

    return "";
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CaptureKeyMapping
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks for the any key press this frame, and creates an input mapping
//                  for a specific player accordingly.

bool UInputMan::CaptureKeyMapping(int whichPlayer, int whichInput)
{	
    if (whichPlayer < PLAYER_ONE || whichPlayer >= MAX_PLAYERS)
        return false;

    // Update the key array
	if (keyboard_needs_poll())
        poll_keyboard();

    for (int whichKey = 0; whichKey < KEY_MAX; ++whichKey)
    {	
        if (KeyPressed(whichKey) /* s_aLastKeys[whichKey] && s_aChangedKeys[whichKey]*/)
        {
            // Clear out all the mappings for this input first, because otherwise old device mappings may linger and interfere
            m_aControlScheme[whichPlayer].GetInputMappings()[whichInput].Clear();
            SetKeyMapping(whichPlayer, whichInput, whichKey);
            return true;
        }

    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ElementPressed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a specific input element was depressed between the last
//                  update and the one previous to it.

bool UInputMan::ElementPressed(int whichPlayer, int whichElement)
{

    bool pressed = false;

    // Which Device is used by this player
    int device = m_aControlScheme[whichPlayer].GetDevice();

    // Get handy pointer to the relevant input element
    InputScheme::InputMapping * pElement = &(m_aControlScheme[whichPlayer].GetInputMappings()[whichElement]);

    // Keyboard is involved
    // Don't check certain elements which don't make sense when in mouse mode
    if (device == DEVICE_KEYB_ONLY || (device == DEVICE_MOUSE_KEYB && !(whichElement == INPUT_AIM_UP || whichElement == INPUT_AIM_DOWN)))
    {
        // Check keyboard
        pressed = pressed ? true : KeyPressed(pElement->GetKey());
    }

    // Mouse is involved
    if (device == DEVICE_MOUSE_KEYB && m_TrapMousePos)
    {
        // Check mouse
        pressed = pressed ? true : MouseButtonPressed(pElement->GetMouseButton(), whichPlayer);
    }

    return pressed;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ElementReleased
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a specific input element was released between the last
//                  update and the one previous to it.

bool UInputMan::ElementReleased(int whichPlayer, int whichElement)
{
    bool released = false;

    // Which Device is used by this player
    int device = m_aControlScheme[whichPlayer].GetDevice();

    // Get handy pointer to the relevant input element
    InputScheme::InputMapping * pElement = &(m_aControlScheme[whichPlayer].GetInputMappings()[whichElement]);

    // Keyboard is involved
    // Don't check certain elements which don't make sense when in mouse mode
    if (device == DEVICE_KEYB_ONLY || (device == DEVICE_MOUSE_KEYB && !(whichElement == INPUT_AIM_UP || whichElement == INPUT_AIM_DOWN)))
    {
        // Check keyboard
        released = released ? true : KeyReleased(pElement->GetKey());
    }

    // Mouse is involved
    if (device == DEVICE_MOUSE_KEYB && m_TrapMousePos)
    {
        // Check mouse
        released = released ? true : MouseButtonReleased(pElement->GetMouseButton(), whichPlayer);
    }

    return released;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ElementHeld
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a specific input element was held during the last update.

bool UInputMan::ElementHeld(int whichPlayer, int whichElement)
{
    bool held = false;

    // Which Device is used by this player
    int device = m_aControlScheme[whichPlayer].GetDevice();

    // Get handy pointer to the relevant input element
    InputScheme::InputMapping * pElement = &(m_aControlScheme[whichPlayer].GetInputMappings()[whichElement]);

    // Keyboard is involved
    // Don't check certain elements which don't make sense when in mouse mode
    if (device == DEVICE_KEYB_ONLY || (device == DEVICE_MOUSE_KEYB && !(whichElement == INPUT_AIM_UP || whichElement == INPUT_AIM_DOWN)))
    {
        // Check keyboard
        held = held ? true : KeyHeld(pElement->GetKey());
    }

    // Mouse is involved
    if (device == DEVICE_MOUSE_KEYB && m_TrapMousePos)
    {
        // Check mouse
        held = held ? true : MouseButtonHeld(pElement->GetMouseButton(), whichPlayer);
    }

    return held;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          KeyPressed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a key was depressed between the last update and the one
//                  previous to it.

bool UInputMan::KeyPressed(const char keyToTest)
{
    // Keyboard disabled
    if (m_DisableKeyboard && (keyToTest >= KEY_A && keyToTest < KEY_ESC))
        return false;

	bool pressed = false;
	
	if (s_InputClass)
	{		
		pressed = (s_InputClass->GetScanCodeState(keyToTest) == GUIInput::Pushed);
	}
	else
	{
		pressed = s_aLastKeys[keyToTest] && s_aChangedKeys[keyToTest];
	}
	
//    s_aChangedKeys[keyToTest] = false;
    return pressed;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          KeyReleased
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a key was released between the last update and the one
//                  previous to it.

bool UInputMan::KeyReleased(const char keyToTest)
{
    // Keyboard disabled
    if (m_DisableKeyboard && (keyToTest >= KEY_A && keyToTest < KEY_ESC))
        return false;

    bool released = !s_aLastKeys[keyToTest] && s_aChangedKeys[keyToTest];
//    s_aChangedKeys[keyToTest] = false;
    return released;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          KeyHeld
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a key is being held right now.

bool UInputMan::KeyHeld(const char keyToTest)
{
    // Keyboard disabled
    if (m_DisableKeyboard && (keyToTest >= KEY_A && keyToTest < KEY_ESC))
        return false;

    return s_aLastKeys[keyToTest];
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WhichKeyHeld
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows the first keyboard key button which is currently down.

int UInputMan::WhichKeyHeld()
{
    int key = readkey();
    // decode the scancode and return it
    return key >> 8;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MouseButtonPressed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a mosue button was depressed between the last update and
//                  the one previous to it.

bool UInputMan::MouseButtonPressed(int whichButton, int whichPlayer)
{
    if (whichButton < 0 || whichButton >= MAX_MOUSE_BUTTONS)
        return false;

    bool pressed = m_aMouseButtonState[whichButton] && m_aMouseChangedButtonState[whichButton];

    return pressed;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MouseButtonReleased
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a mosue button was released between the last update and
//                  the one previous to it.

bool UInputMan::MouseButtonReleased(int whichButton, int whichPlayer)
{
    if (whichButton < 0 || whichButton >= MAX_MOUSE_BUTTONS)
        return false;

    bool released = !m_aMouseButtonState[whichButton] && m_aMouseChangedButtonState[whichButton];

    return released;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MouseButtonHeld
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether a mouse button is being held down right now.

bool UInputMan::MouseButtonHeld(int whichButton, int whichPlayer)
{
    if (whichButton < 0 || whichButton >= MAX_MOUSE_BUTTONS)
        return false;

    return m_aMouseButtonState[whichButton];
}




//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AnalogAimValues
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the analog aiming values of a specific player's control scheme.

Vector UInputMan::AnalogAimValues(int whichPlayer)
{
    Vector aimValues;

    // Which Device is used by this player
    int device = m_aControlScheme[whichPlayer].GetDevice();
	if (m_OverrideInput)
		device = UInputMan::DEVICE_MOUSE_KEYB;

    // Get handy pointer to the relevant input elements
    InputScheme::InputMapping * pElement = m_aControlScheme[whichPlayer].GetInputMappings();

    // Mouse is involved
    if (device == DEVICE_MOUSE_KEYB)
    {
        // Return the normalized mouse analog stick emulation value
		if (m_OverrideInput && whichPlayer >= 0 && whichPlayer < MAX_PLAYERS)
			aimValues = m_aNetworkAnalogMoveData[whichPlayer] / m_MouseTrapRadius;
		else
			aimValues = m_AnalogMouseData / m_MouseTrapRadius;
    }

    return aimValues;
}





//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMouseMovement
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the relative movement of the mouse since last update. Only returns
//                  true if the selected player actuall is using the mouse.

Vector UInputMan::GetMouseMovement(int whichPlayer)
{
    Vector mouseMovement;

    if (whichPlayer == -1 || m_aControlScheme[whichPlayer].GetDevice() == DEVICE_MOUSE_KEYB)
        mouseMovement = m_RawMouseMovement;

    return mouseMovement;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DisableMouseMoving
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Will temporarily disable positioniong of the mouse. This is so that
//                  when focus is swtiched back to the game window, it avoids having the
//                  window fly away because the user clicked the title bar of the window.
// Arguments:       Whether to disable or not.
// Return value:    None.

void UInputMan::DisableMouseMoving(bool disable)
{
    if (disable)
    {
        m_DisableMouseMoving = true;
        m_PrepareToEnableMouseMoving = false;
        // Set these to outside the screen so the mouse has to be updated first before checking if they're in the screen or not
        mouse_x = mouse_y = -1;
    }
    else
        m_PrepareToEnableMouseMoving = true;
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMousePos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the absolute screen position of the mouse cursor.
// Arguments:       Where to place the mouse.

void UInputMan::SetMousePos(Vector &newPos, int whichPlayer)
{
    // Only mess with the mouse if the original mouse position is not above the screen and may be grabbing the title bar of the game window
    if (!m_DisableMouseMoving && !m_TrapMousePos && (whichPlayer == -1 || m_aControlScheme[whichPlayer].GetDevice() == DEVICE_MOUSE_KEYB))
        position_mouse(newPos.m_X, newPos.m_Y);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ForceMouseWithinPlayerScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the mouse within a specific player's screen area.

void UInputMan::ForceMouseWithinPlayerScreen(int whichPlayer)
{
    if (whichPlayer < PLAYER_ONE || whichPlayer >= PLAYER_FOUR)
        return;

    int screenWidth = g_FrameMan.GetPlayerFrameBufferWidth(whichPlayer);
    int screenHeight = g_FrameMan.GetPlayerFrameBufferHeight(whichPlayer);

    // If we are dealing with split screens, then draw the intermediate draw splitscreen to the appropriate spot on the back buffer
    if (g_FrameMan.GetScreenCount() > 1)
    {
        // Always upper left corner
        if (whichPlayer == 0)
            ForceMouseWithinBox(0, 0, screenWidth, screenHeight, whichPlayer);
        else if (whichPlayer == 1)
        {
            // If both splits, or just Vsplit, then in upper right quadrant
            if ((g_FrameMan.GetVSplit() && !g_FrameMan.GetHSplit()) || (g_FrameMan.GetVSplit() && g_FrameMan.GetHSplit()))
                ForceMouseWithinBox(g_FrameMan.GetResX() / 2, 0, screenWidth, screenHeight, whichPlayer);
            // If only hsplit, then lower left quadrant
            else
                ForceMouseWithinBox(0, g_FrameMan.GetResY() / 2, screenWidth, screenHeight, whichPlayer);
        }
        // Always lower left quadrant
        else if (whichPlayer == 2)
            ForceMouseWithinBox(0, g_FrameMan.GetResY() / 2, screenWidth, screenHeight, whichPlayer);
        // Always lower right quadrant
        else if (whichPlayer == 3)
            ForceMouseWithinBox(g_FrameMan.GetResX() / 2, g_FrameMan.GetResY() / 2, screenWidth, screenHeight, whichPlayer);
    }
    // No splitscreen, jsut constrain hte mouse to the full screen
    else
        ForceMouseWithinBox(0, 0, screenWidth, screenHeight, whichPlayer);
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ForceMouseWithinBox
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the mouse within a box on the screen.

void UInputMan::ForceMouseWithinBox(int x, int y, int width, int height, int whichPlayer)
{
    // Only mess with the mouse if the original mouse position is not above the screen and may be grabbing the title bar of the game window
    if (!m_DisableMouseMoving && !m_TrapMousePos && (whichPlayer == -1 || m_aControlScheme[whichPlayer].GetDevice() == DEVICE_MOUSE_KEYB))
    {
        float windowResMultiplier = g_FrameMan.IsFullscreen() ? g_FrameMan.NxFullscreen() : g_FrameMan.NxWindowed();
        int mouseX = MAX(x, mouse_x);
        int mouseY = MAX(y, mouse_y);
        mouseX = MIN(mouseX, x + width * windowResMultiplier);
        mouseY = MIN(mouseY, y + height * windowResMultiplier);
		
#if !defined(__APPLE__)
		// [CHRISK] OSX really doesn't like this
        position_mouse(mouseX, mouseY);
#endif // !defined(__APPLE__)
    }
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AnyMouseButtonPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return true if there is any mouse button presses at all

bool UInputMan::AnyMouseButtonPress()
{
    bool input = false;

    for (int button = 0; button < MAX_MOUSE_BUTTONS && !input; ++button)
    {
        input = MouseButtonPressed(button, -1) ? true : input;
    }

    return input;
}


void UInputMan::TrapMousePos(bool trap, int whichPlayer)
{ 
	if (whichPlayer == -1 || m_aControlScheme[whichPlayer].GetDevice() == DEVICE_MOUSE_KEYB) 
	{ 
		m_TrapMousePos = trap; 
	} 
	m_TrapMousePosPerPlayer[whichPlayer] = trap;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MouseUsedByPlayer
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reports if any, and which player is using the mouse for control at
//                  this time.

int UInputMan::MouseUsedByPlayer() const
{
    if (m_aControlScheme[PLAYER_ONE].GetDevice() == DEVICE_MOUSE_KEYB)
        return PLAYER_ONE;
    else if (m_aControlScheme[PLAYER_TWO].GetDevice() == DEVICE_MOUSE_KEYB)
        return PLAYER_TWO;
    else if (m_aControlScheme[PLAYER_THREE].GetDevice() == DEVICE_MOUSE_KEYB)
        return PLAYER_THREE;
    else if (m_aControlScheme[PLAYER_FOUR].GetDevice() == DEVICE_MOUSE_KEYB)
        return PLAYER_FOUR;

    return PLAYER_NONE;
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMenuDirectional
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the generic direction input from any and all players which can
//                  affect a shared menu cursor. Normalized to 1.0 max

Vector UInputMan::GetMenuDirectional()
{
    Vector allInput;

	//m_LastDeviceWhichControlledGUICursor = 0;

    // Go through all players
    for (int player = 0; player < MAX_PLAYERS; ++player)
    {
        // Which Device is used by this player
        int device = m_aControlScheme[player].GetDevice();

        // Keyboard input
        if (device == DEVICE_KEYB_ONLY)
        {
            if (ElementHeld(player, INPUT_L_UP))
                allInput.m_Y += -1.0;
            else if (ElementHeld(player, INPUT_L_DOWN))
                allInput.m_Y += 1.0;
            if (ElementHeld(player, INPUT_L_LEFT))
                allInput.m_X += -1.0;
            else if (ElementHeld(player, INPUT_L_RIGHT))
                allInput.m_X += 1.0;
        }
        // Mouse player shouldn't be doing anything here, he should be using the mouse!
        else if (device == DEVICE_MOUSE_KEYB)
        {
            
        }
        // Analog enabled device (joystick, really)
        else if (device >= DEVICE_GAMEPAD_1)
        {
			if (AnalogMoveValues(player).GetLargest() > 0.05)
			{
				allInput += AnalogMoveValues(player);
				m_LastDeviceWhichControlledGUICursor = device;
			}
			if (AnalogAimValues(player).GetLargest() > 0.05)
			{
				allInput += AnalogAimValues(player);
				m_LastDeviceWhichControlledGUICursor = device;
			}
        }
    }

    // Normalize/cap
    allInput.CapMagnitude(1.0);

    return allInput;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MenuButtonPressed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether any generic button with the menu cursor was pressed
//                  between previous update and this.

bool UInputMan::MenuButtonPressed(int whichButton)
{
    bool button = false;

    // Go through all players
    for (int player = 0; player < MAX_PLAYERS && !button; ++player)
    {
        // Which Device is used by this player
        int device = m_aControlScheme[player].GetDevice();

        // Check for primary/secondary button presses
		if (whichButton >= MENU_PRIMARY)
			button = ElementPressed(player, INPUT_FIRE) || MouseButtonPressed(MOUSE_LEFT, player) || button;
		if (whichButton >= MENU_SECONDARY)
			button = ElementPressed(player, INPUT_PIEMENU) || MouseButtonPressed(MOUSE_RIGHT, player) || button;

		if (button/* && m_aControlScheme[player].GetDevice() >= DEVICE_GAMEPAD_1*/)
			m_LastDeviceWhichControlledGUICursor = device;
    }

    return button;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MenuButtonReleased
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether any generic button with the menu cursor is held down.

bool UInputMan::MenuButtonReleased(int whichButton)
{
    bool button = false;

    // Go through all players
    for (int player = 0; player < MAX_PLAYERS && !button; ++player)
    {
        // Which Device is used by this player
        int device = m_aControlScheme[player].GetDevice();

        // Check for primary/secondary button presses
		if (whichButton >= MENU_PRIMARY)
			button = ElementReleased(player, INPUT_FIRE) || MouseButtonReleased(MOUSE_LEFT, player) || button;
        if (whichButton >= MENU_SECONDARY)
            button = ElementReleased(player, INPUT_PIEMENU) || MouseButtonReleased(MOUSE_RIGHT, player) || button;
    }

    return button;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MenuButtonHeld
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows whether any generic button with the menu cursor is held down.

bool UInputMan::MenuButtonHeld(int whichButton)
{
    bool button = false;

    // Go through all players
    for (int player = 0; player < MAX_PLAYERS && !button; ++player)
    {
        // Which Device is used by this player
        int device = m_aControlScheme[player].GetDevice();

        // Check for primary/secondary button presses
		if (whichButton >= MENU_PRIMARY)
			button = ElementHeld(player, INPUT_FIRE) || MouseButtonHeld(MOUSE_LEFT, player) || button;
		if (whichButton >= MENU_SECONDARY)
			button = ElementHeld(player, INPUT_PIEMENU) || MouseButtonHeld(MOUSE_RIGHT, player) || button;

		if (button/* && m_aControlScheme[player].GetDevice() >= DEVICE_GAMEPAD_1*/)
		{
			m_LastDeviceWhichControlledGUICursor = device;
			break;
		}
    }

    return button;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AnyInput
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return true if there is any input at all, keyboard or buttons or d-pad.

bool UInputMan::AnyInput()
{
    // Check keyboard
    bool input = keypressed();

    return input;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AnyPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return true if there is any key, button, or d-pad presses at all.

bool UInputMan::AnyPress()
{
    bool pressed = false;

    // Check keyboard for presses
    for (int testKey = 0; testKey < KEY_MAX; ++testKey)
        pressed = s_aLastKeys[testKey] && s_aChangedKeys[testKey] ? true : pressed;

    // Check mouse buttons for presses
    if (!pressed)
        pressed = AnyMouseButtonPress();

    return pressed;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AnyStartPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return true if there is any start key/button presses at all.
//                  MUST call Update before calling this for it to work properly!

bool UInputMan::AnyStartPress()
{
    bool pressed = false;

    // Check keyboard for presses
    if (KeyPressed(KEY_ESC) || KeyPressed(KEY_SPACE))
        pressed = true;

    // Check all user bound start buttons
    for (int player = 0; player < MAX_PLAYERS && !pressed; ++player)
    {
        pressed = pressed || ElementPressed(player, INPUT_START);
        pressed = pressed || ElementPressed(player, INPUT_BACK);
    }

    return pressed;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WaitForSpace
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Halts thread and waits for space to be pressed. This is for debug
//                  purposes mostly.

void UInputMan::WaitForSpace()
{
/*
    if (m_DebugArmed) {
        while (KeyPressed(KEY_SPACE) != CDXKEY_PRESS)
            m_pInput->Update();

        m_pInput->Update();
    }
*/
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this UInputMan. Supposed to be done every frame.

int UInputMan::Update()
{
	m_LastDeviceWhichControlledGUICursor = 0;

    // Only update inputs on drawn frames
//    if (!g_TimerMan.DrawnSimUpdate())
//        return 0;

//    clear_keybuf();

    if (keyboard_needs_poll())
        poll_keyboard();

    if (mouse_needs_poll())
        poll_mouse();

    poll_joystick();

    // Detect and store key changes since last Update()
    for (int i = 0; i < KEY_MAX; ++i)
        s_aChangedKeys[i] = key[i] != s_aLastKeys[i];

    // Store mouse movement
    int mickeyX, mickeyY;
    get_mouse_mickeys(&mickeyX, &mickeyY);
    m_RawMouseMovement.SetXY(mickeyX, mickeyY);
/* Figured out that it was a acceleration setting in AllegroConfig.txt that caused this problem
    // Adjust for the wackiness that happens to the mickeys when 2x fullscreen screened
    if (g_FrameMan.IsFullscreen() && g_FrameMan.NxFullscreen() > 1)
    {
        m_RawMouseMovement *= 0.2 / g_FrameMan.NxFullscreen();
        m_RawMouseMovement.Ceiling();
    }
*/
// TODO: Have proper mouse sensitivity controls somewhere
    m_RawMouseMovement *= m_MouseSensitivity;

    // Detect and store mouse button input
    m_aMouseButtonState[MOUSE_LEFT] = mouse_b & 1;
    m_aMouseButtonState[MOUSE_RIGHT] = mouse_b & 2;
    m_aMouseButtonState[MOUSE_MIDDLE] = mouse_b & 4;

    // Detect changes in mouse button input
    m_aMouseChangedButtonState[MOUSE_LEFT] = m_aMouseButtonState[MOUSE_LEFT] != m_aMousePrevButtonState[MOUSE_LEFT];
    m_aMouseChangedButtonState[MOUSE_RIGHT] = m_aMouseButtonState[MOUSE_RIGHT] != m_aMousePrevButtonState[MOUSE_RIGHT];
    m_aMouseChangedButtonState[MOUSE_MIDDLE] = m_aMouseButtonState[MOUSE_MIDDLE] != m_aMousePrevButtonState[MOUSE_MIDDLE];

    // Detect and store mouse movement input, translated to analog stick emulation
    int mousePlayer;
    if ((mousePlayer = MouseUsedByPlayer()) != PLAYER_NONE)
    {
// TODO: temporary? Make framerate independent!
        // Mouse analog emulation input returns to 0;
//        m_AnalogMouseData *= 0.9;

// TODO: Make proper sensitivty setting and GUI controls
		m_AnalogMouseData.m_X += m_RawMouseMovement.m_X * 3;
		m_AnalogMouseData.m_Y += m_RawMouseMovement.m_Y * 3;
        // Cap the mouse input in a circle
        m_AnalogMouseData.CapMagnitude(m_MouseTrapRadius);

        // Only mess with the mouse pos if the original mouse position is not above the screen and may be grabbing the title bar of the game window
        if (!m_DisableMouseMoving)
        {
			if (!m_OverrideInput && !m_IsInMultiplayerMode)
			{
				// Trap the (invisible) mouse cursor in the middle of the screen, so it doens't fly out in windowed mode and some other window gets clicked
				if (m_TrapMousePos)
					position_mouse(g_FrameMan.GetResX() / 2, g_FrameMan.GetResY() / 2);
				// The mouse cursor is visible and can move about the screen/window, but it shuold still be contained within the mouse player's part of the window
				else if (g_InActivity)
					ForceMouseWithinPlayerScreen(mousePlayer);
			}
        }

        // Mouse wheel update, translate motion into discrete ticks
        if (abs(mouse_z) >= 1)
        {
            // Save the direction
            m_MouseWheelChange = mouse_z;
            // Reset the position
            position_mouse_z(0);
        }
        // Or just leave as no change
        else
            m_MouseWheelChange = 0;

        // Enable the mouse cursor positioning again after having been disabled. Only do this when the mouse is within the drawing area so it
        // won't cause the whole window to move if the user clicks the title bar and unintentionally drags it due to programmatic positioning.
        float mouseDenominator = g_FrameMan.IsFullscreen() ? g_FrameMan.NxFullscreen() : g_FrameMan.NxWindowed();
        int mX = (float)mouse_x / mouseDenominator;
        int mY = (float)mouse_y / mouseDenominator;
        if (m_DisableMouseMoving && m_PrepareToEnableMouseMoving && (mX >= 0 && mX < g_FrameMan.GetResX() && mY >= 0 && mY < g_FrameMan.GetResY()))
            m_DisableMouseMoving = m_PrepareToEnableMouseMoving = false;
    }

    // If Escape is pressed, go to the mainmenu or close the app
    if (KeyPressed(KEY_ESC))
    {
        // Do nothing if the intro is playing, has own handling
// Now handled in MainMenuGUI
//        else if (g_IntroState <= 0 || g_IntroState >= 17)
//            g_Quit = true;
    }
/*
    if (KeyPressed(KEY_Q)) {
        m_DebugArmed = true;
    }
*/


    // Screendump - Ctrl+S
    if ((((key_shifts & KB_CTRL_FLAG)/* && (key_shifts & KB_SHIFT_FLAG)*/ && KeyHeld(KEY_S)) || KeyHeld(KEY_PRTSCR)))// && g_TimerMan.DrawnSimUpdate())
    {
        g_FrameMan.SaveScreenToBMP("ScreenDump");
// TEMP!!
//        g_FrameMan.SaveBitmapToBMP(g_SceneMan.GetTerrain()->GetBGColorBitmap(), "SceneBG");
    }

	// Dump entire map, Ctrl+W
    if ((key_shifts & KB_CTRL_FLAG) && KeyHeld(KEY_W))
    {
        g_FrameMan.SaveWorldToBMP("WorldDump");
    }

    // Perf stats display toggle
    if ((key_shifts & KB_CTRL_FLAG) && KeyPressed(KEY_P))
        g_FrameMan.ShowPerformanceStats(!g_FrameMan.IsShowingPerformanceStats());

    // Force one sim update per graphics frame
    if ((key_shifts & KB_CTRL_FLAG) && KeyPressed(KEY_O))
        g_TimerMan.SetOneSimUpdatePerFrame(!g_TimerMan.IsOneSimUpdatePerFrame());

	// Dump all shortcuts to console window
	if (KeyPressed(KEY_F1))
	{
		/*
		if (!g_ConsoleMan.IsEnabled())
			g_ConsoleMan.SetEnabled();

		g_ConsoleMan.PrintString("--- SHORTCUTS ---");
		g_ConsoleMan.PrintString("~ - This console. CTRL + ~ - Console without input capture");
		g_ConsoleMan.PrintString("CTRL + S / Prnt Scrn - Make a screenshot");
		g_ConsoleMan.PrintString("CTRL + W - Make a screenshot of the entire level");
		g_ConsoleMan.PrintString("CTRL + P - Show performance stats");
		g_ConsoleMan.PrintString("CTRL + R - Reset activity");
		g_ConsoleMan.PrintString("CTRL + M - Switch display mode: Draw -> Material -> MO");
		g_ConsoleMan.PrintString("CTRL + O - Toggle one sim update per frame");

		g_ConsoleMan.PrintString("F2 - Reload all scripts");
		g_ConsoleMan.PrintString("F3 - Save Console.log");
		g_ConsoleMan.PrintString("F4 - Save input log");
		g_ConsoleMan.PrintString("F5 - Clear console log ");
		*/
	}

	if (KeyPressed(KEY_F2))
	{
		//g_PresetMan.ReloadAllScripts();
		//g_ConsoleMan.PrintString("Scripts reloaded");
	}

	if (KeyPressed(KEY_F3))
	{
		//g_ConsoleMan.SaveAllText("Console.dump.log");
	}

	if (KeyPressed(KEY_F4))
	{
		//g_ConsoleMan.SaveInputLog("Console.input.log");
	}

	if (KeyPressed(KEY_F5))
	{
		//g_ConsoleMan.ClearLog();
	}


    // Save the current state of the keyboard so that we can compare it
    // next frame and see which key states have been changed in the mean time.
    memcpy(s_aLastKeys, const_cast<const char *>(key), KEY_MAX);

    // Save the mouse button states so taht we can compare it next frame and see which buttons have changed.
    for (int mbutton = 0; mbutton < MAX_MOUSE_BUTTONS; ++mbutton)
    {
        m_aMousePrevButtonState[mbutton] = m_aMouseButtonState[mbutton];
    }


    return 0;
}







} // namespace RTE
